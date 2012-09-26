//FILE: SaxBasic.cpp

// Copyright 1993-1997 Sax Software, Inc.
// Copyright 1993-1997 Polar Engineering and Consulting.
// All rights reserved.

// wwedit32 c:\dev\sax\include\makeww.wwb

#include "SaxBasic.h"

#define SB_VERSION _T("5.1.05")

#include <stdarg.h>

#ifndef WIN32
	#include <olenls.h>
	typedef char FAR* LPOLESTR;
#endif

#ifndef EXPORT
	#ifdef WIN32
		#define EXPORT
	#else
		#define EXPORT _export
	#endif
#endif

#if !defined(_DEBUG) && !defined(SB_TRACE_PROD)
	#undef SB_TRACE_ACTION_CALLS
	#undef SB_TRACE_QUERY_CALLS
	#undef SB_TRACE_ACTION_CALLBACKS
	#undef SB_TRACE_QUERY_CALLBACKS
	#undef SB_TRACE_INTERNAL
	#undef SB_TRACE_EXECUTION
	#undef SB_TRACE_ALL
	#undef SB_TRACE_DEBUG
#else
	#ifdef SB_TRACE_CALLBACKS
		#define SB_TRACE_ACTION_CALLBACKS
		#define SB_TRACE_QUERY_CALLBACKS
	#endif
#endif

#if defined(SB_TRACE_ACTION_CALLS) || defined(SB_TRACE_QUERY_CALLS) || \
	defined(SB_TRACE_ACTION_CALLBACKS) || defined(SB_TRACE_QUERY_CALLBACKS) || \
	defined(SB_TRACE_INTERNAL) || defined(SB_TRACE_EXECUTION) || \
	defined(SB_TRACE_ALL) || \
	defined(SB_TRACE_PROD) || defined(SB_TRACE_DEBUG)
	#define SB_TRACE_ANY
#endif

#ifdef SB_TRACE_ANY
	#define SB_TRACE_ACTION(psz) CSaxBasicTraceCall tracecall(m_pInstance,CSaxBasic::catAction,psz)
	#define SB_TRACE_QUERY(psz) CSaxBasicTraceCall tracecall(m_pInstance,CSaxBasic::catQuery,psz)
	#define SB_TRACE_ACTION_CALLBACK(psz) CSaxBasicTraceCall tracecall(pThis->GetInstance(),CSaxBasic::catActionCallback,psz)
	#define SB_TRACE_QUERY_CALLBACK(psz) CSaxBasicTraceCall tracecall(pThis->GetInstance(),CSaxBasic::catQueryCallback,psz)
	#define SB_TRACE_PARM(v) tracecall.parm(_T(#v)##_T("="),v)
	#define SB_TRACE_PARMI2(v) tracecall.parm(_T(#v)##_T("="),(long)v)
	#define SB_TRACE_PARMLITERAL(v,f) tracecall.parm(_T(#v)##_T("="),v,f)
	#define SB_TRACE_OUTPUT tracecall.output()
#else
	#define SB_TRACE_ACTION(psz) (void)0
	#define SB_TRACE_QUERY(psz) (void)0
	#define SB_TRACE_ACTION_CALLBACK(psz) (void)0
	#define SB_TRACE_QUERY_CALLBACK(psz) (void)0
	#define SB_TRACE_PARM(v) (void)0
	#define SB_TRACE_PARMI2(v) (void)0
	#define SB_TRACE_PARMLITERAL(v,f) (void)0
	#define SB_TRACE_OUTPUT (void)0
#endif

#define SB_TRACE_BUFFER_SIZE 256

#ifndef WW_GWL_USER
#define SB_GWL_USER				0

#define SB_MCM_ADDEXTENSION			(WM_USER+0x100)
#define SB_MCM_GETCODE				(WM_USER+0x101)
#define SB_MCM_GETERRORFILE			(WM_USER+0x102)
#define SB_MCM_GETERRORLINE			(WM_USER+0x103)
#define SB_MCM_GETERROROFFSET		(WM_USER+0x104)
#define SB_MCM_GETERRORTEXT			(WM_USER+0x105)
#define SB_MCM_GETEXPRESSION		(WM_USER+0x106)
#define SB_MCM_GETEXTENSIONPREFIX	(WM_USER+0x107)
#define SB_MCM_GETFILEDESC			(WM_USER+0x108)
#define SB_MCM_GETFILEEXT			(WM_USER+0x109)
#define SB_MCM_GETFILENAME			(WM_USER+0x10A)
#define SB_MCM_GETHIDDENCODE		(WM_USER+0x10B)
#define SB_MCM_GETPAUSE				(WM_USER+0x10C)
#define SB_MCM_GETRESULT			(WM_USER+0x10D)
#define SB_MCM_GETRUN				(WM_USER+0x10E)
#define SB_MCM_RUNFILE				(WM_USER+0x110)
#define SB_MCM_RUNTHIS				(WM_USER+0x111)
#define SB_MCM_SETCODE				(WM_USER+0x112)
#define SB_MCM_SETEXPRESSION		(WM_USER+0x113)
#define SB_MCM_SETEXTENSIONPREFIX	(WM_USER+0x114)
#define SB_MCM_SETFILEDESC			(WM_USER+0x115)
#define SB_MCM_SETFILEEXT			(WM_USER+0x116)
#define SB_MCM_SETFILENAME			(WM_USER+0x117)
#define SB_MCM_SETHIDDENCODE		(WM_USER+0x118)
#define SB_MCM_SETPAUSE				(WM_USER+0x119)
#define SB_MCM_SETRESULT			(WM_USER+0x11A)
#define SB_MCM_SETRUN				(WM_USER+0x11B)
#define SB_MCM_SETVISIBLE			(WM_USER+0x11C)
#define SB_MCM_STOREFILENAME		(WM_USER+0x11D)
#define SB_MCM_SYNTAXCHECK			(WM_USER+0x11E)
#define SB_MCM_GETCAPTION			(WM_USER+0x120)
#define SB_MCM_SETCAPTION			(WM_USER+0x121)
#define SB_MCM_GETCOLOR				(WM_USER+0x122)
#define SB_MCM_SETCOLOR				(WM_USER+0x123)
#define SB_MCM_GETEVENTMODE			(WM_USER+0x124)
#define SB_MCM_SETEVENTMODE			(WM_USER+0x125)
#define SB_MCM_GETFIRE				(WM_USER+0x126)
#define SB_MCM_CREATEHANDLER		(WM_USER+0x127)
#define SB_MCM_GETLOCKED			(WM_USER+0x128)
#define SB_MCM_SETLOCKED			(WM_USER+0x129)
#define SB_MCM_GETSELLENGTH			(WM_USER+0x12A)
#define SB_MCM_SETSELLENGTH			(WM_USER+0x12B)
#define SB_MCM_GETSELSTART			(WM_USER+0x12C)
#define SB_MCM_SETSELSTART			(WM_USER+0x12D)
#define SB_MCM_GETSELTEXT			(WM_USER+0x12E)
#define SB_MCM_SETSELTEXT			(WM_USER+0x12F)
#define SB_MCM_SETABOUTBOXPROC	 	(WM_USER+0x130)
#define SB_MCM_SETDEBUGPRINTPROC 	(WM_USER+0x131)
#define SB_MCM_SETDOEVENTSPROC		(WM_USER+0x132)
#define SB_MCM_SETHELPPROC			(WM_USER+0x133)
#define SB_MCM_SETNOTIFYPROC		(WM_USER+0x134)
#define SB_MCM_SETWINDOWHANDLEPROC	(WM_USER+0x135)
#define SB_MCM_SETSTATUSBARPROC		(WM_USER+0x136)
#define SB_MCM_SETCLOSEOKPROC		(WM_USER+0x137)
#define SB_MCM_SETGETMACRONAMEPROC	(WM_USER+0x138)
#define SB_MCM_SETREADMACROPROC		(WM_USER+0x139)
#define SB_MCM_SETWRITEMACROPROC	(WM_USER+0x13A)
#define SB_MCM_SETGETMACROCAPTIONPROC (WM_USER+0x13B)
#define SB_MCM_SETOVERRIDEMENUCOMMANDPROC (WM_USER+0x13C)
#define SB_MCM_SETPARSEPROPERTIESPROC (WM_USER+0x13D)
#define SB_MCM_SETNEWINSTANCEPROC	(WM_USER+0x13E)
#define SB_MCM_SETUPDATEPROPERTIESPROC (WM_USER+0x13F)
#define SB_MCM_MENUCOMMAND			(WM_USER+0x140)
#define SB_MCM_LOADMODULE			(WM_USER+0x141)
#define SB_MCM_UNLOADMODULE			(WM_USER+0x142)
#define SB_MCM_SHEETCOUNT			(WM_USER+0x143)
#define SB_MCM_SHEETSELECT			(WM_USER+0x144)
#define SB_MCM_GETACTIVESHEET		(WM_USER+0x145)
#define SB_MCM_SETACTIVESHEET		(WM_USER+0x146)
#define SB_MCM_FINDSHEET			(WM_USER+0x147)
#define SB_MCM_REMOVEEXTENSIONS		(WM_USER+0x148)
#define SB_MCM_ISMODULELOADED		(WM_USER+0x149)
#define SB_MCM_CREATEHANDLERS		(WM_USER+0x14A)
#define SB_MCM_CLOSESHEET			(WM_USER+0x14B)
#define SB_MCM_FINDORCREATEPROC		(WM_USER+0x14C)
#define SB_MCM_MODULEINSTANCE		(WM_USER+0x14D)
#define SB_MCM_PROJECT				(WM_USER+0x14E)
#define SB_MCM_ISMODULEINUSE		(WM_USER+0x14F)
#define SB_MCM_SHUTDOWN				(WM_USER+0x150)
#define SB_MCM_CODEUNITDESTROYED	(WM_USER+0x151)
#define SB_MCM_REPORTERROR			(WM_USER+0x152)
#define SB_MCM_GETCMDDESC256		(WM_USER+0x153)
#define SB_MCM_GETCMDTOOLTIP256		(WM_USER+0x154)
#define SB_MCM_GETCMDCHECK			(WM_USER+0x155)
#define SB_MCM_SETDEBUGTRACEPROC	(WM_USER+0x156)
#define SB_MCM_TRACE				(WM_USER+0x157)
#define SB_MCM_GETVERSIONINFO		(WM_USER+0x158)
#define SB_MCM_GETERRORHELPCONTEXT	(WM_USER+0x159)
#define SB_MCM_GETERRORHELPFILE		(WM_USER+0x15A)
#define SB_MCM_GETERRORNUMBER		(WM_USER+0x15B)
#define SB_MCM_GETERRORSOURCE		(WM_USER+0x15C)
#define SB_MCM_EDITREFERENCES		(WM_USER+0x15D)
#define SB_MCM_DEBUGBROWSE			(WM_USER+0x15E)
#define SB_MCM_SETPROJECTREFSPROC	(WM_USER+0x15F)
#define SB_MCM_GETBLOCKEDKEYWORDS	(WM_USER+0x160)
#define SB_MCM_SETBLOCKEDKEYWORDS	(WM_USER+0x161)
#define SB_MCM_GETDEFAULTMACRONAME	(WM_USER+0x162)
#define SB_MCM_SETDEFAULTMACRONAME	(WM_USER+0x163)
#define SB_MCM_GETDEFAULTOBJECTNAME	(WM_USER+0x164)
#define SB_MCM_SETDEFAULTOBJECTNAME	(WM_USER+0x165)
#define SB_MCM_GETPROCDISPLAYMODE	(WM_USER+0x166)
#define SB_MCM_SETPROCDISPLAYMODE	(WM_USER+0x167)
#define SB_MCM_GETMACROCAPTION		(WM_USER+0x168)
#define SB_MCM_GETBREAKS			(WM_USER+0x169)
#define SB_MCM_SETBREAKS			(WM_USER+0x16A)
#define SB_MCM_GETMACRONAME			(WM_USER+0x16B)
#define SB_MCM_GETDEFAULTDATATYPE	(WM_USER+0x16C)
#define SB_MCM_SETDEFAULTDATATYPE	(WM_USER+0x16D)
#define SB_MCM_GETDEBUGHEIGHT		(WM_USER+0x16E)
#define SB_MCM_SETDEBUGHEIGHT		(WM_USER+0x16F)
#define SB_MCM_GETPROJECTWIDTH		(WM_USER+0x170)
#define SB_MCM_SETPROJECTWIDTH		(WM_USER+0x171)
#define SB_MCM_GETTOOLBARHEIGHT		(WM_USER+0x172)
#define SB_MCM_GETSTATUSBARHEIGHT	(WM_USER+0x173)
#define SB_MCM_GETHEADERLINECOUNT	(WM_USER+0x174)
#define SB_MCM_GETMODULEKIND		(WM_USER+0x175)
#define SB_MCM_GETATTRIBUTENAME		(WM_USER+0x176)
#define SB_MCM_SETATTRIBUTENAME		(WM_USER+0x177)
#define SB_MCM_INDEXFROMLINE		(WM_USER+0x178)
#define SB_MCM_LINEFROMINDEX		(WM_USER+0x179)
#define SB_MCM_GETTABASSPACES		(WM_USER+0x17A)
#define SB_MCM_SETTABASSPACES		(WM_USER+0x17B)
#define SB_MCM_GETTABWIDTH			(WM_USER+0x17C)
#define SB_MCM_SETTABWIDTH			(WM_USER+0x17D)
#define SB_MCM_GETTEXT				(WM_USER+0x17E)
#define SB_MCM_SETTEXT				(WM_USER+0x17F)
// *** new property *** (WW_MCM_[GET|SET]XXX)
// *** new method *** (WW_MCM_XXX)
// *** new event *** (WW_MCM_SETXXXPROC)
//#define WW_MCM_GETXXX				(WM_USER+0x)
//#define WW_MCM_SETXXX				(WM_USER+0x)
//#define WW_MCM_SETXXXPROC			(WM_USER+0x)

#define SB_MCM_ATTACH				(WM_USER+0x180)
#define SB_MCM_DETACH				(WM_USER+0x181)

typedef void (CALLBACK *SB_MC_AboutBoxProc)(HWND);
typedef void (CALLBACK *SB_MC_DebugPrintProc)(HWND, LPCSTR);
typedef void (CALLBACK *SB_MC_DoEventsProc)(HWND);
typedef BOOL (CALLBACK *SB_MC_HelpProc)(HWND, LPCSTR);
typedef void (CALLBACK *SB_MC_NotifyProc)(HWND, short);
typedef HWND (CALLBACK *SB_MC_WindowHandleProc)(HWND);
typedef void (CALLBACK *SB_MC_StatusBarProc)(HWND, LPCSTR, BOOL);
typedef BOOL (CALLBACK *SB_MC_CloseOkProc)(HWND, short, BOOL*);
typedef void (CALLBACK *SB_MC_GetMacroCaptionProc)(HWND, LPSTR, UINT);
typedef BOOL (CALLBACK *SB_MC_GetMacroNameProc)(HWND, BOOL, LPSTR, UINT);
typedef UINT (CALLBACK *SB_MC_ReadMacroProc)(HWND, LPCSTR, LPSTR);
typedef BOOL (CALLBACK *SB_MC_WriteMacroProc)(HWND, LPCSTR, LPCSTR);
typedef BOOL (CALLBACK *SB_MC_OverrideMenuCommandProc)(HWND, short);
typedef BOOL (CALLBACK *SB_MC_ParsePropertiesProc)(HWND, LPCSTR, LPCSTR);
typedef LPVOID (CALLBACK *SB_MC_NewInstanceProc)(HWND, LPCSTR);
typedef void (CALLBACK *SB_MC_UpdatePropertiesProc)(HWND, LPCSTR, BSTR*);
typedef void (CALLBACK *SB_MC_DebugTraceProc)(HWND, short, LPCSTR);
typedef void (CALLBACK *SB_MC_SetProjectRefsProc)(HWND, LPCSTR, LPCSTR);
// *** new event *** (SB_MC_XxxProc)

typedef struct SB_MC_ReportErrorTag
{
	DWORD scode;
	WORD wCode;
	LPCSTR lpszDescription;
	LPCSTR lpszSource;
	DWORD dwHelpContext;
	LPCSTR lpszHelpFile;
} SB_MC_ReportError;

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

#ifdef SB_ONE_DLL_PREFIX
	#define SB_BASICEDITORCLASS		"__WinWrapBasicEditor"
#elif defined(SB_FND)
	#define SB_BASICEDITORCLASS		"_WinWrapBasicEditor"
#else
	#define SB_BASICEDITORCLASS		"WinWrapBasicEditor"
#endif
#endif

#ifdef WIN32
	#ifdef SB_ONE_DLL_PREFIX
		#define SB_BASICEDITORDLL	_T(SB_ONE_DLL_PREFIX)_T("_32.DLL")
	#elif defined(SB_FND)
		#define SB_BASICEDITORDLL	_T("WW_FND32.DLL")
	#else
		#define SB_BASICEDITORDLL	_T("WW_MC532.DLL")
	#endif
#else
	#ifdef SB_ONE_DLL_PREFIX
		#define SB_BASICEDITORDLL	SB_ONE_DLL_PREFIX"_16.DLL"
	#else
		#define SB_BASICEDITORDLL	"WW_MC516.DLL"
	#endif
#endif

#ifdef SB_TRACE_ANY
static const LPCTSTR szColor[] =
{
	_T("colorReserved"),
	_T("colorExtension"),
	_T("colorBuiltin"),
	_T("colorComment"),
	_T("colorError"),
	_T("colorExec"),
	_T("colorBreak"),
	_T("colorTextForeground"),
	_T("colorTextBackground"),
	_T("colorSelectionForeground"),
	_T("colorSelectionBackground")
};

static LPCTSTR GetColorLiteral(long i)
{
	return i >= 0 && i < sizeof(szColor)/sizeof(LPCTSTR) ? szColor[i] : _T("");
}

static const LPCTSTR rgszCmd[] =
{
	_T("cmdFileNew"),
	_T("cmdFileOpen"),
	_T("cmdFileSave"),
	_T("cmdFileSaveAs"),
	_T("cmdFilePrint"),
	_T("cmdFilePrintSetup"),
	_T("cmdMacroRun"),
	_T("cmdMacroPause"),
	_T("cmdMacroEnd"),
	_T("cmdDebugStepInto"),
	_T("cmdDebugStepOver"),
	_T("cmdDebugStepTo"),
	_T("cmdDebugBreak"),
	_T("cmdDebugQuickWatch"),
	_T("cmdDebugAddWatch"),
	_T("cmdDebugBrowse"),
	_T("cmdDebugSetNext"),
	_T("cmdDebugShowNext"),
	_T("cmdHelpApp"),
	_T("cmdHelpLanguage"),
	_T("cmdHelpTopic"),
	_T("cmdHelpAbout"),
	_T("cmdEditUndo"),
	_T("cmdEditCut"),
	_T("cmdEditCopy"),
	_T("cmdEditPaste"),
	_T("cmdEditFind"),
	_T("cmdEditReplace"),
	_T("cmdEditAgain"),
	_T("cmdViewFont"),
	_T("cmdEditDelete"),
	_T("cmdEditSelectAll"),
	_T("cmdEditUserDialog"),
	_T("cmdFileClose"),
	_T("cmdFileSaveAll"),
	_T("cmdDebugStepOut"),
	_T("cmdSheetOpenUses"),
	_T("cmdSheetCloseAll"),
	_T("cmdSheet1"),
	_T("cmdSheet2"),
	_T("cmdSheet3"),
	_T("cmdSheet4"),
	_T("cmdSheet5"),
	_T("cmdSheet6"),
	_T("cmdSheet7"),
	_T("cmdSheet8"),
	_T("cmdSheet9"),
	_T("cmdFileNewCodeModule"),
	_T("cmdFileNewObjectModule"),
	_T("cmdFileNewClassModule"),
	_T("cmdEditProperties"),
	_T("cmdEditIndent"),
	_T("cmdEditOutdent"),
	_T("cmdDebugClearAllBreaks"),
	_T("cmdViewMacro"),
	_T("cmdViewImmediate"),
	_T("cmdViewWatch"),
	_T("cmdViewStack"),
	_T("cmdViewLoaded"),
	_T("cmdViewToolbar"),
	_T("cmdViewStatusBar"),
	_T("cmdViewEditButtons"),
	_T("cmdViewAlwaysSplit"),
	_T("cmdEditRedo"),
	_T("cmdEditReferences"),
	_T("cmdEditTabAsSpaces"),
	_T("cmdViewTabWidth1"),
	_T("cmdViewTabWidth2"),
	_T("cmdViewTabWidth3"),
	_T("cmdViewTabWidth4"),
	_T("cmdViewTabWidth5"),
	_T("cmdViewTabWidth6"),
	_T("cmdViewTabWidth7"),
	_T("cmdViewTabWidth8")
	//_T("cmd<MenuItem>")
};

static LPCTSTR GetCmdLiteral(long i)
{
	if (i >= 0 && i < sizeof(rgszCmd)/sizeof(LPCTSTR))
		return rgszCmd[i];

	switch (i)
	{
	case CSaxBasic::cmdPopupNoMouse: return _T("cmdPopupNoMouse");
	case CSaxBasic::cmdShowError: return _T("cmdShowError");
	case CSaxBasic::cmdNewModulePopup: return _T("cmdNewModulePopup");
	case CSaxBasic::cmdPopup: return _T("cmdPopup");
	}

	return _T("");
}

static const LPCTSTR rgszNotify[] =
{
	_T("SB_NOTIFYERRORS"),
	_T("SB_NOTIFYBEGIN"),
	_T("SB_NOTIFYEND"),
	_T("SB_NOTIFYPAUSE"),
	_T("SB_NOTIFYRESUME"),
	_T("SB_NOTIFYERRORLOG"),
	_T("SB_NOTIFYPREMODAL"),
	_T("SB_NOTIFYPOSTMODAL"),
	_T("SB_NOTIFYPENDING"),
	_T("SB_NOTIFYMACROBEGIN"),
	_T("SB_NOTIFYMACROEND"),
	_T("SB_NOTIFYEDITORPREMODAL"),
	_T("SB_NOTIFYEDITORPOSTMODAL"),
	_T("SB_NOTIFYHALTED"),
	_T("SB_NOTIFYPROTOTYPECHANGE"),
	_T("SB_NOTIFYCHANGE"),
	_T("SB_NOTIFYFONTCHANGE"),
	_T("SB_NOTIFYSHOWFORM"),
	_T("SB_NOTIFYDIALOGFORM"),
	_T("SB_NOTIFYHANDLEERROR")
};

static LPCTSTR GetNotifyLiteral(long i)
{
	return i >= 0 && i < sizeof(rgszNotify)/sizeof(LPCTSTR) ?
		   rgszNotify[i] : 0;
}

static const LPCTSTR rgszNotifyProc[] =
{
	_T("OnBasicErrorAlert"),
	_T("OnBasicBegin"),
	_T("OnBasicEnd"),
	_T("OnBasicPause"),
	_T("OnBasicResume"),
	0,
	_T("OnBasicPreModal"),
	_T("OnBasicPostModal"),
	_T("OnBasicPending"),
	_T("OnBasicMacroBegin"),
	_T("OnBasicMacroEnd"),
	0,
	0,
	0,
	0,
	_T("OnChange"),
	0,
	_T("OnBasicShowForm"),
	_T("OnBasicDialogIdle"),
	_T("OnBasicHandleError")
};

static LPCTSTR GetNotifyProcName(short nNotify)
{
	return nNotify >= 0 && nNotify < sizeof(rgszNotifyProc)/sizeof(LPCTSTR) ?
		   rgszNotifyProc[nNotify] : 0;
}

static const LPCTSTR rgszHandler[] =
{
	_T("Handler.Exists [Get]"),
	_T("Handler.Call"),
	_T("Handler.Evaluate"),
	_T("Handler.Start"),
	_T("Handler.StepInto [Get]"),
	_T("Handler.CallPA"),
	_T("Handler.EvaluatePA"),
	_T("Handler.StartPA"),
	_T("Handler.Prototype [Get]"),
	_T("Handler.ProcName [Get]"),
	_T("Handler.IsModuleLoaded [Get]")
};

static LPCTSTR GetHandlerName(long i, WORD wFlags)
{
	if (wFlags&DISPATCH_PROPERTYPUT)
		return i == DISPID_HandlerStepInto ? _T("Handler.StepInto [Let]") : 0;

	return i >= 0 && i < sizeof(rgszHandler)/sizeof(LPCTSTR) ?
		   rgszHandler[i] : 0;
}

static const LPCTSTR rgszHandlers[] =
{
	_T("Handlers.Item [Get]"),
	_T("Handlers.Count [Get]")
};

static LPCTSTR GetHandlersName(long i)
{
	return i >= 0 && i < sizeof(rgszHandlers)/sizeof(LPCTSTR) ?
		   rgszHandlers[i] : 0;
}
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

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic

#if _MFC_VER >= 0x0400
	#ifdef _AFXDLL
		#define CALLBACK_PROLOG(theClass) \
			theClass* pThis = (theClass*)GetWindowLong(hWnd,SB_GWL_USER); \
			AFX_MANAGE_STATE(pThis->ModuleState())
	#endif
#else
	#define AfxGetThread AfxGetApp
	#define CWinThread CWinApp
#endif

#ifndef CALLBACK_PROLOG
	#define CALLBACK_PROLOG(theClass) \
		theClass* pThis = (theClass*)GetWindowLong(hWnd,SB_GWL_USER)
#endif

#ifdef WIN32
	class SafeSectionRes
	{
	public:
		SafeSectionRes() : m_fInitialized(TRUE)
			{ InitializeCriticalSection(&m_cs); }
		~SafeSectionRes()
			{ DeleteCriticalSection(&m_cs); m_fInitialized = FALSE; }
		void Enter()
			{ if (m_fInitialized) EnterCriticalSection(&m_cs); }
		void Leave()
			{ if (m_fInitialized) LeaveCriticalSection(&m_cs); }
	private:
		BOOL m_fInitialized;
		CRITICAL_SECTION m_cs;
	};
	class SafeSectionImp
	{
	public:
		SafeSectionImp(SafeSectionRes& ss) : m_ss(ss) { m_ss.Enter(); }
		~SafeSectionImp() { m_ss.Leave(); }
	private:
		SafeSectionRes& m_ss;
	};
	#define SAFE_SECTION(ss) SafeSectionImp SS_##ss(ss);
	static /*c-init*/ SafeSectionRes g_ssLoader;
	static /*g_ssLoader*/ DWORD g_dwTLS = 0xffffffff;
	static /*g_ssLoader*/ int g_refs = 0;
#else
	#define SAFE_SECTION(ss)
#endif

#ifdef SB_TRACE_ANY
	static /*g_ssLoader*/ int g_iLoaderNumber = 0;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicLoader

#if defined(WIN32) && !defined(SB_FND) && !defined(SB_ONE_DLL_PREFIX)
	#ifndef OA_CALLBACK
		#define OA_CALLBACK _stdcall
		#define OA_PROCDEF(type,name) type (OA_CALLBACK* name)
	#endif
	typedef OA_PROCDEF(void,SB_LOADCUPROC)(BOOL);
#endif

class CSaxBasicLoader
{
public:
	CSaxBasicLoader();
	~CSaxBasicLoader();
	void Attach(CSaxBasic* pBasic);
	void Detach(CSaxBasic* pBasic);
	void Free();
	CSaxBasic* GetBasicFirst() const { return m_pBasicFirst; }
	void Load();
#if defined(WIN32) && !defined(SB_FND) && !defined(SB_ONE_DLL_PREFIX)
	void LoadCU(BOOL fLoading);
#endif
#ifdef SB_TRACE_ANY
	void AdjustCallDepth(int lDelta) { m_iCallDepth += lDelta; }
	int GetCallDepth() const { return m_iCallDepth; }
	int GetNumber() const { return m_iNumber; }
	int NextInstanceNumber() { return ++m_iNextInstanceNumber; }
#endif
private:
	long m_refs;
	HINSTANCE m_hInstance;
	CSaxBasic* m_pBasicFirst;
	CSaxBasic* m_pBasicLast;
#ifdef SB_TRACE_ANY
	int m_iCallDepth;
	int m_iNumber;
	int m_iNextInstanceNumber;
#endif
};

#ifdef WIN32
	static CSaxBasicLoader* GetBasicLoader()
	{
		SAFE_SECTION(g_ssLoader);
		if (g_dwTLS == 0xffffffff)
			g_dwTLS = TlsAlloc();

		CSaxBasicLoader* pLoader = (CSaxBasicLoader*)TlsGetValue(g_dwTLS);
		if (pLoader)
			return pLoader;

		return new CSaxBasicLoader;
	}
#else
	static /*Win16*/ CSaxBasicLoader g_BasicLoader;
	static inline CSaxBasicLoader* GetBasicLoader()
	{ return &g_BasicLoader; }
#endif

CSaxBasicLoader::CSaxBasicLoader() :
	m_refs(0),
	m_hInstance(0),
	m_pBasicFirst(0),
	m_pBasicLast(0)
{
#ifdef WIN32
	TlsSetValue(g_dwTLS,this);
	++g_refs;
#endif

#ifdef SB_TRACE_ANY
	m_iCallDepth = 0;
	m_iNumber = ++g_iLoaderNumber;
	m_iNextInstanceNumber = 0;
#endif
}

CSaxBasicLoader::~CSaxBasicLoader()
{
#ifdef WIN32
	SAFE_SECTION(g_ssLoader);
	TlsSetValue(g_dwTLS,0);

	if (!--g_refs)
	{
		TlsFree(g_dwTLS);
		g_dwTLS = 0xffffffff;
	#ifdef SB_TRACE_ANY
		g_iLoaderNumber = 0;
	#endif
	}
#endif
}

void CSaxBasicLoader::Attach(CSaxBasic* pBasic)
{
	// add control to linked list
	if (m_pBasicLast)
		m_pBasicLast->m_pBasicNext = pBasic;
	else
		m_pBasicFirst = pBasic;

	pBasic->m_pBasicPrev = m_pBasicLast;
	pBasic->m_pBasicNext = 0;
	m_pBasicLast = pBasic;
}

void CSaxBasicLoader::Detach(CSaxBasic* pBasic)
{
	// remove control from linked list
	if (pBasic->m_pBasicPrev)
		pBasic->m_pBasicPrev->m_pBasicNext = pBasic->m_pBasicNext;
	else
		m_pBasicFirst = pBasic->m_pBasicNext;

	if (pBasic->m_pBasicNext)
		pBasic->m_pBasicNext->m_pBasicPrev = pBasic->m_pBasicPrev;
	else
		m_pBasicLast = pBasic->m_pBasicPrev;
}

void CSaxBasicLoader::Free()
{
	if (--m_refs)
		return; // still needed

	if (m_hInstance)
	{
	#if defined(WIN32) && !defined(SB_FND) && !defined(SB_ONE_DLL_PREFIX)
		LoadCU(FALSE);
	#endif
		FreeLibrary(m_hInstance);
		m_hInstance = 0;
	}

#ifdef SB_TRACE_ANY
	m_iNextInstanceNumber = 0;
#endif

#ifdef WIN32
	delete this;
#endif
}

#ifdef WIN32
	#ifndef A2OLE
		#define A2OLE(x) A2W(x)
	#endif
	static HINSTANCE LoadLibA(LPCSTR pszLibName)
	{
		HINSTANCE hInstance = LoadLibraryA(pszLibName);
		if (!hInstance)
			return 0;

	#if defined(SB_FND) || defined(SB_ONE_DLL_PREFIX)
		// CoLoadLibrary(,TRUE) causes DllCanUnloadNow to control unloading
		USES_CONVERSION;
		CoLoadLibrary(A2OLE(pszLibName),TRUE);
	#endif
		return hInstance;
	}

	static HINSTANCE LoadLibW(LPCWSTR pszLibName)
	{
		USES_CONVERSION;
		return LoadLibA(W2A(pszLibName));
	}

	#ifdef UNICODE
		#define LoadLib(pszLibName) LoadLibW(pszLibName)
	#else
		#define LoadLib(pszLibName) LoadLibA(pszLibName)
	#endif
#else
	static HINSTANCE LoadLib(LPCSTR pszLibName)
	{
		HINSTANCE hInstance = LoadLibrary(pszLibName);
		if (hInstance <= HINSTANCE_ERROR)
			return 0;
	
	#if defined(SB_FND) || defined(SB_ONE_DLL_PREFIX)
		CoLoadLibrary(pszLibName,TRUE);
	#endif
		return hInstance;
	}
#endif

#ifdef SB_IMPLICITLINK
	extern "C" UINT __stdcall OA_StrLen(LPCWSTR lpsz);
#endif

void CSaxBasicLoader::Load()
{
	if (m_refs++)
		return; // already loaded

#ifdef SB_IMPLICITLINK
	OA_StrLen(0); // force linker to include DLL
#else
	m_hInstance = LoadLib(SB_BASICEDITORDLL);
#if defined(SB_FND)
	if (!m_hInstance)
		m_hInstance =
		#ifdef NDEBUG
			LoadLib(_T("C:\\DEV\\SAX\\WW_FND32\\RELEASE\\")SB_BASICEDITORDLL);
		#else
			LoadLib(_T("C:\\DEV\\SAX\\WW_FND32\\DEBUG\\")SB_BASICEDITORDLL);
		#endif
#elif defined(SB_ONE_DLL_PREFIX)
	#ifndef WW_GWL_USER
		#ifdef WIN32
		if (!m_hInstance)
		{
			// locate SBENT532.DLL
			char szPath[_MAX_PATH];
			LONG cb = sizeof(szPath);
			if (RegQueryValueA(HKEY_CLASSES_ROOT,
				"Clsid\\{D379AF55-CCEA-11D1-8412-767498095FF0}\\InprocServer32",
				szPath,&cb) == ERROR_SUCCESS)
			{
				LPSTR p = strrchr(szPath,'\\');
				if (p)
				{
				#ifdef UNICODE
					USES_CONVERSION;
					lstrcpyA(p+1,OLE2A(SB_BASICEDITORDLL));
				#else
					lstrcpy(p+1,SB_BASICEDITORDLL);
				#endif
					m_hInstance = LoadLibA(szPath);
				}
			}

			if (!m_hInstance)
				m_hInstance =
				#ifdef NDEBUG
					LoadLib(_T("C:\\DEV\\SAX\\SBE5\\INTELREL\\SBE5_32.DLL"));
				#else
					LoadLib(_T("C:\\DEV\\SAX\\SBE5\\INTELDEB\\SBE5_32.DLL"));
				#endif
		}
		#endif
	#else
		#ifdef WIN32
			if (!m_hInstance)
				m_hInstance =
					LoadLib(_T("C:\\DEV\\WW\\INTELBIN\\WWB5_32.DLL"));
		#else
			if (m_hInstance <= HINSTANCE_ERROR)
				m_hInstance =
					LoadLib("C:\\DEV\\WW\\WWBASIC\\WWB5_16.DLL");
		#endif
	#endif
#endif

#if defined(WIN32) && !defined(SB_FND) && !defined(SB_ONE_DLL_PREFIX)
	LoadCU(TRUE);
#endif

	// CreateWindow will fail if the window class isn't registered
	ASSERT(m_hInstance != NULL);
#endif
}

#if defined(WIN32) && !defined(SB_FND) && !defined(SB_ONE_DLL_PREFIX)
void CSaxBasicLoader::LoadCU(BOOL fLoading)
{
	if (m_hInstance)
	{
		SB_LOADCUPROC lpfnLoadCU =
			(SB_LOADCUPROC)GetProcAddress(m_hInstance,"_WW_LoadCU@4");
		if (lpfnLoadCU)
			(*lpfnLoadCU)(fLoading);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMapEntry class

class CSaxBasicMapEntry
{
public:
	CSaxBasicMapEntry() { }
	CSaxBasicMapEntry(LPDISPATCH pdispInner, LPDISPATCH pdispOuter)
	{ m_pdispInner = pdispInner; m_pdispOuter = pdispOuter; }
	LPDISPATCH GetInner() const { return m_pdispInner; }
	LPDISPATCH GetOuter() const { return m_pdispOuter; }
private:
	LPDISPATCH m_pdispInner;
	LPDISPATCH m_pdispOuter;
};

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicMap class

class CSaxBasicMap
{
public:
	CSaxBasicMap();
	~CSaxBasicMap();
	void Add(LPDISPATCH pdispInner, LPDISPATCH pdispOuter);
	LPDISPATCH Find(LPDISPATCH pdispInner) const;
	void Remove(LPDISPATCH pdispInner);
private:
	int m_iLen;
	int m_iMax;
	CSaxBasicMapEntry* m_rgEntry;
};

CSaxBasicMap::CSaxBasicMap() :
	m_iLen(0),
	m_iMax(0),
	m_rgEntry(0)
{
}

CSaxBasicMap::~CSaxBasicMap()
{
	delete [] m_rgEntry;
}

void CSaxBasicMap::Add(LPDISPATCH pdispInner, LPDISPATCH pdispOuter)
{
	if (m_iLen == m_iMax)
	{
		int iMax = m_iMax ? m_iMax << 1 : 16;
		CSaxBasicMapEntry* rgEntry = new CSaxBasicMapEntry[iMax];
		if (!rgEntry)
			return;

		memcpy(rgEntry,m_rgEntry,m_iLen*sizeof(CSaxBasicMapEntry));
		delete [] m_rgEntry;
		m_iMax = iMax;
		m_rgEntry = rgEntry;
	}

	m_rgEntry[m_iLen++] = CSaxBasicMapEntry(pdispInner,pdispOuter);
}

LPDISPATCH CSaxBasicMap::Find(LPDISPATCH pdispInner) const
{
	for (int i = 0; i < m_iLen; ++i)
		if (m_rgEntry[i].GetInner() == pdispInner)
		{
			LPDISPATCH pdispOuter = m_rgEntry[i].GetOuter();
			pdispOuter->AddRef();
			return pdispOuter;
		}

	return 0;
}

void CSaxBasicMap::Remove(LPDISPATCH pdispInner)
{
	for (int i = 0; i < m_iLen; ++i)
		if (m_rgEntry[i].GetInner() == pdispInner)
		{
			--m_iLen;
			memcpy(&m_rgEntry[i],&m_rgEntry[i]+1,
				   (m_iLen-i)*sizeof(CSaxBasicMapEntry));
			break;
		}
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicInstance class

class CSaxBasicInstance
{
public:
	CSaxBasicInstance(CSaxBasic* pBasic);
	~CSaxBasicInstance();
	void AddRef() { ++m_refs; }
	void Detach();
	int GetCategories() const { return m_categories; }
	CSaxBasicLoader* GetLoader() const { return m_pLoader; }
#ifdef SB_TRACE_ANY
	int GetNumber() const { return m_iNumber; }
#endif
	BOOL IsAttached() const { return m_pBasic != 0; }
	void Release() { if (!--m_refs) delete this; }
	void SetCategories(int categories);
#ifdef SB_TRACE_ANY
	void Trace(int trace, LPCTSTR psz);
	void TraceEnter(LPCTSTR pszProcName);
	void TraceLeave();
	void TraceParm(int trace, LPCTSTR pszName, BOOL b);
	void TraceParm(int trace, LPCTSTR pszName, long i);
	void TraceParm(int trace, LPCTSTR pszName, LPDISPATCH pdisp);
	void TraceParm(int trace, LPCTSTR pszName, LPCSTR psz);
#ifdef UNICODE
	void TraceParm(int trace, LPCTSTR pszName, LPCTSTR psz);
#endif
	void TraceParm(int trace, LPCTSTR pszName, LPVARIANT pvar, BOOL bAsBool);
	void TraceParm(int trace, LPCTSTR pszName, long i, LPCTSTR (*f)(long));
#endif

	CSaxBasicMap Map;
private:
	LPTSTR TracePrefix(LPTSTR pszBuffer, int trace, LPCTSTR pszName);
	LPCTSTR TraceSpaces() const;
	long m_refs;
	CSaxBasicLoader* m_pLoader;
	CSaxBasic* m_pBasic;
	int m_categories;
#ifdef SB_TRACE_ANY
	int m_iNumber;
	DWORD m_dwBaseTime;
#endif
};

#ifdef SB_TRACE_ANY
static const TCHAR szSpaces[] = _T("| | | | | | | | | | | | | | | | ");

static const LPCTSTR rgszSep[] =
{
	_T("|\\ "),
	_T("|/"),
	_T("| -> "),
	_T("| <- "),
	_T("| ** ")
};
#endif

CSaxBasicInstance::CSaxBasicInstance(CSaxBasic* pBasic) :
	m_refs(1),
	m_pLoader(GetBasicLoader()),
	m_pBasic(pBasic),
	m_categories(0)
{
	m_pLoader->Load();
#ifdef SB_TRACE_ANY
	m_iNumber = m_pLoader->NextInstanceNumber();
	m_dwBaseTime = GetTickCount();
#endif
	m_pLoader->Attach(m_pBasic);
}

CSaxBasicInstance::~CSaxBasicInstance()
{
	m_pLoader->Free();
}

void CSaxBasicInstance::Detach()
{
	m_pLoader->Detach(m_pBasic);
	m_pBasic = 0;
	Release();
}

void CSaxBasicInstance::SetCategories(int categories)
{
	m_categories = categories;

#ifdef SB_TRACE_ANY
	const LPCTSTR rgszCatName[8] =
	{
		_T("*Action="),
		_T("*Query="),
		_T("*ActionCallback="),
		_T("*QueryCallback="),
		0,
		0,
		_T("*Internal="),
		_T("*Execution=")
	};

	TraceEnter(_T("Trace"));

	for (int i = 0; i < 8; ++i)
	{
		if (rgszCatName[i])
			TraceParm(SB_TRACEINPUT,rgszCatName[i],(BOOL)(categories&1));

		categories >>= 1;
	}

	TraceLeave();
#endif
}

#ifdef SB_TRACE_ANY
void CSaxBasicInstance::Trace(int trace, LPCTSTR psz)
{
	switch (trace)
	{
	case SB_TRACEENTER: TraceEnter(psz); break;
	case SB_TRACELEAVE: TraceLeave(); break;
	default: TraceParm(trace,(LPCTSTR)0,psz);
	}
}

void CSaxBasicInstance::TraceEnter(LPCTSTR pszProcName)
{
	AddRef();

	if (m_pBasic)
	{
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		TracePrefix(szBuffer,SB_TRACEENTER,pszProcName);
		m_pLoader->AdjustCallDepth(1);
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

void CSaxBasicInstance::TraceLeave()
{
	if (m_pBasic)
	{
		m_pLoader->AdjustCallDepth(-1);
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		TracePrefix(szBuffer,SB_TRACELEAVE,0);
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}

	Release();
}

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, BOOL b)
{
	if (IsAttached())
	{
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		lstrcpy(TracePrefix(szBuffer,trace,pszName),
				b ? _T("True") : _T("False"));
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, long i)
{
	if (m_pBasic)
	{
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		wsprintf(TracePrefix(szBuffer,trace,pszName),_T("%ld"),i);
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, LPDISPATCH pdisp)
{
	if (m_pBasic)
	{
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		wsprintf(TracePrefix(szBuffer,trace,pszName),_T("0x%lx"),(long)pdisp);
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

#ifdef UNICODE
void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, LPCSTR psz)
{
	if (m_pBasic)
	{
		if (psz)
			TraceParm(trace,pszName,CString(psz));
		else
			TraceParm(trace,pszName,(LPCTSTR)0);
	}
}
#endif

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, LPCTSTR psz)
{
	if (m_pBasic)
	{
		TCHAR sz[SB_TRACE_BUFFER_SIZE/2];
		if (!psz)
			lstrcpy(sz,_T("(null)"));
		else
		{
			lstrcpyn(sz,psz,sizeof(sz)-3*sizeof(TCHAR));
			for (LPTSTR p = sz; *p != _T('\0'); ++p)
				if (*p < 32)
				{
					*p = _T('\0');
					break;
				}

			if (lstrlen(sz) != lstrlen(psz))
				lstrcat(sz,_T("..."));
		}

		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		wsprintf(TracePrefix(szBuffer,trace,pszName),
				 pszName ? _T("\"%s\"") : _T("%s"),sz);
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName,
								  LPVARIANT pvar, BOOL bAsBool)
{
	if (IsAttached())
	{
		VARIANT var;
		VariantInit(&var);
		HRESULT hr = VariantChangeType(&var,pvar,0,bAsBool ? VT_BOOL : VT_BSTR);
		if (SUCCEEDED(hr))
		{
			if (bAsBool)
				TraceParm(trace,pszName,(BOOL)V_BOOL(&var));
			else
			{
				CString s = V_BSTR(&var);
				TraceParm(trace,pszName,s);
			}

			VariantClear(&var);
		}
		else
		{
			TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
			lstrcpy(TracePrefix(szBuffer,trace,pszName),_T("?"));
			m_pBasic->OnBasicDebugTrace(szBuffer);
		}
	}
}

void CSaxBasicInstance::TraceParm(int trace, LPCTSTR pszName, long i,
								  LPCTSTR (*f)(long))
{
	if (IsAttached())
	{
		TCHAR szBuffer[SB_TRACE_BUFFER_SIZE];
		wsprintf(TracePrefix(szBuffer,trace,pszName),_T("%ld (%s)"),i,(*f)(i));
		m_pBasic->OnBasicDebugTrace(szBuffer);
	}
}

LPTSTR CSaxBasicInstance::TracePrefix(LPTSTR pszBuffer, int trace,
									  LPCTSTR pszName)
{
	DWORD dwDelta = (GetTickCount()-m_dwBaseTime)/100;
	wsprintf(pszBuffer,_T("*SaxBasic(%d-%d) %04ld.%ld %s%s"),
			 m_pLoader->GetNumber(),m_iNumber,dwDelta/10,dwDelta%10,
			 TraceSpaces(),rgszSep[trace]);
	if (pszName)
	{
		BOOL bTruncated = FALSE;
		int len1 = lstrlen(pszBuffer);
		int len2 = lstrlen(pszName)+len1;
		if (len2 >= SB_TRACE_BUFFER_SIZE/2)
		{
			bTruncated = TRUE;
			len2 = SB_TRACE_BUFFER_SIZE/2-4;
		}

		lstrcpyn(pszBuffer+len1,pszName,len2-len1+1);
		if (bTruncated)
			lstrcat(pszBuffer,_T("..."));
	}

	return pszBuffer+lstrlen(pszBuffer);
}

LPCTSTR CSaxBasicInstance::TraceSpaces() const
{
	int n = m_pLoader->GetCallDepth()*2;
	if (n < 0)
		n = 0;
	else if (n >= sizeof(szSpaces)/sizeof(TCHAR))
		n = sizeof(szSpaces)/sizeof(TCHAR)-1;

	return szSpaces+sizeof(szSpaces)/sizeof(TCHAR)-1-n;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicTraceCall class

#ifdef SB_TRACE_ANY
class CSaxBasicTraceCall
{
public:
	CSaxBasicTraceCall(CSaxBasicInstance* pInstance, int category,
					   LPCTSTR pszProcName);
	~CSaxBasicTraceCall();
	void output() { m_trace = SB_TRACEOUTPUT; }
	void parm(LPCTSTR pszName, BOOL b); // int b
	void parm(LPCTSTR pszName, long i);
	void parm(LPCTSTR pszName, unsigned long i) { parm(pszName,(long)i); }
	void parm(LPCTSTR pszName, unsigned int i) { parm(pszName,(long)i); }
	void parm(LPCTSTR pszName, HWND hwnd) { parm(pszName,(long)(int)hwnd); }
	void parm(LPCTSTR pszName, LPDISPATCH pdisp);
#ifdef UNICODE
	void parm(LPCTSTR pszName, LPCSTR psz);
#endif
	void parm(LPCTSTR pszName, LPCTSTR psz);
	void parm(LPCTSTR pszName, LPVARIANT pvar, BOOL bAsBool);
	void parm(LPCTSTR pszName, long i, LPCTSTR (*f)(long));
	void parm(DISPPARAMS* pdispparams);
	BOOL tracing() const { return m_pInstance != 0; }
private:
	CSaxBasicInstance* m_pInstance;
	int m_trace;
};

CSaxBasicTraceCall::CSaxBasicTraceCall(CSaxBasicInstance* pInstance,
									   int category, LPCTSTR pszProcName) :
	m_pInstance(pszProcName && pInstance->IsAttached() &&
				(category&pInstance->GetCategories()) ? pInstance : 0),
	m_trace(SB_TRACEINPUT)
{
	if (m_pInstance)
		m_pInstance->TraceEnter(pszProcName);
}

CSaxBasicTraceCall::~CSaxBasicTraceCall()
{
	if (m_pInstance)
		m_pInstance->TraceLeave();
}

void CSaxBasicTraceCall::parm(LPCTSTR pszName, BOOL b)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,b);
}

void CSaxBasicTraceCall::parm(LPCTSTR pszName, long i)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,i);
}

void CSaxBasicTraceCall::parm(LPCTSTR pszName, LPDISPATCH pdisp)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,pdisp);
}

#ifdef UNICODE
void CSaxBasicTraceCall::parm(LPCTSTR pszName, LPCSTR psz)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,psz);
}
#endif

void CSaxBasicTraceCall::parm(LPCTSTR pszName, LPCTSTR psz)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,psz);
}

void CSaxBasicTraceCall::parm(LPCTSTR pszName, LPVARIANT pvar, BOOL bAsBool)
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,pvar,bAsBool);
}

void CSaxBasicTraceCall::parm(LPCTSTR pszName, long i, LPCTSTR (*f)(long))
{
	if (m_pInstance)
		m_pInstance->TraceParm(m_trace,pszName,i,f);
}

void CSaxBasicTraceCall::parm(DISPPARAMS* pdispparams)
{
	UINT cArgs = pdispparams->cArgs;
	for (UINT iArg = 0; iArg < cArgs; ++iArg)
	{
		TCHAR szName[6];
		lstrcpy(szName,_T("Arg0="));
		szName[3] += (iArg+1)%10;
		parm(szName,&pdispparams->rgvarg[cArgs-iArg-1],FALSE);
	}
}
#endif


///// CSaxBasicObject ///////////////////////////////////////////////////////

class CSaxBasicObject : public IDispatch
{
public:
	STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
    						 LCID lcid, DISPID* rgdispid);
protected:
	CSaxBasicObject(CSaxBasicInstance* pInstance, LPDISPATCH pdisp);
	~CSaxBasicObject();
    ULONG m_refs;
	CSaxBasicInstance* m_pInstance;
	LPDISPATCH m_pdisp;
};

STDMETHODIMP
CSaxBasicObject::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IDispatch)
	{
		*ppv = this;
		AddRef();
		return NOERROR;
	}

	*ppv = 0;
	return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG)
CSaxBasicObject::AddRef()
{
	return ++m_refs;
}

STDMETHODIMP_(ULONG)
CSaxBasicObject::Release()
{
	if (--m_refs == 0)
	{
		delete this;
		return 0;
	}

	return m_refs;
}

STDMETHODIMP
CSaxBasicObject::GetTypeInfoCount(UINT* pctinfo)
{
	return m_pdisp->GetTypeInfoCount(pctinfo);
}

STDMETHODIMP
CSaxBasicObject::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
{
	return m_pdisp->GetTypeInfo(itinfo,lcid,pptinfo);
}

STDMETHODIMP
CSaxBasicObject::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
							   LCID lcid, DISPID* rgdispid)
{
	return m_pdisp->GetIDsOfNames(riid,rgszNames,cNames,lcid,rgdispid);
}

// CSaxBasicObject takes ownership of pdisp
CSaxBasicObject::CSaxBasicObject(CSaxBasicInstance* pInstance, LPDISPATCH pdisp) :
	m_refs(1),
	m_pInstance(pInstance),
	m_pdisp(pdisp)
{
	m_pInstance->AddRef();
	m_pInstance->Map.Add(m_pdisp,this);
}

CSaxBasicObject::~CSaxBasicObject()
{
	m_pInstance->Map.Remove(m_pdisp);
	m_pdisp->Release();
	m_pInstance->Release();
}

///// CSaxBasicHandler //////////////////////////////////////////////////////

class CSaxBasicHandler : public CSaxBasicObject
{
public:
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
    				  DISPPARAMS* pdispparams, VARIANT* pvarResult,
    				  EXCEPINFO* pexcepinfo, UINT* puArgErr);

	CSaxBasicHandler(CSaxBasicInstance* pInstance, LPDISPATCH pdisp) :
		CSaxBasicObject(pInstance,pdisp) { }

	static LPDISPATCH Create(CSaxBasicInstance* pInstance, LPDISPATCH pdisp);
};

STDMETHODIMP
CSaxBasicHandler::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
						 DISPPARAMS* pdispparams, VARIANT* pvarResult,
						 EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
#ifdef SB_TRACE_ANY
	BOOL bPropPut = (wFlags&DISPATCH_PROPERTYPUT) != 0;
	BOOL bPropGet = dispidMember == DISPID_HandlerExists ||
					dispidMember == DISPID_HandlerStepInto ||
					dispidMember >= DISPID_HandlerPrototype;
	BOOL bAsBool = dispidMember != DISPID_HandlerPrototype &&
				   dispidMember != DISPID_HandlerProcName;
	CSaxBasicTraceCall tracecall(m_pInstance,
								 bPropPut || !bPropGet ? CSaxBasic::catAction :
														 CSaxBasic::catQuery,
								 GetHandlerName(dispidMember,wFlags));
	tracecall.parm(_T("Me="),m_pdisp);
	if (bPropPut)
	{
		if (dispidMember == DISPID_HandlerStepInto && pdispparams->cArgs == 1)
			tracecall.parm(_T("NewValue="),pdispparams->rgvarg,bAsBool);
	}
	else if (!bPropGet)
		tracecall.parm(pdispparams);
#endif
	HRESULT hr = m_pdisp->Invoke(dispidMember,riid,lcid,wFlags,pdispparams,
								 pvarResult,pexcepinfo,puArgErr);
#ifdef SB_TRACE_ANY
	tracecall.output();
	if (FAILED(hr))
		tracecall.parm(_T("Error="),(LPDISPATCH)hr);
	else if (pvarResult)
		tracecall.parm(_T("Result="),pvarResult,bAsBool && bPropGet);
#endif
	return hr;
}

LPDISPATCH CSaxBasicHandler::Create(CSaxBasicInstance* pInstance,
									LPDISPATCH pdisp)
{
	if (!pdisp)
		return 0;

	LPDISPATCH pdispOuter = pInstance->Map.Find(pdisp);
	if (pdispOuter)
		return pdispOuter;

	return new CSaxBasicHandler(pInstance,pdisp);
}

///// CSaxBasicHandlers /////////////////////////////////////////////////////

class CSaxBasicHandlers : public CSaxBasicObject
{
public:
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
    				  DISPPARAMS* pdispparams, VARIANT* pvarResult,
    				  EXCEPINFO* pexcepinfo, UINT* puArgErr);

	CSaxBasicHandlers(CSaxBasicInstance* pInstance, LPDISPATCH pdisp) :
		CSaxBasicObject(pInstance,pdisp) { }

	static LPDISPATCH Create(CSaxBasicInstance* pInstance, LPDISPATCH pdisp);
};

STDMETHODIMP
CSaxBasicHandlers::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
						  DISPPARAMS* pdispparams, VARIANT* pvarResult,
						  EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
#ifdef SB_TRACE_ANY
	CSaxBasicTraceCall tracecall(m_pInstance,
								 dispidMember == DISPID_HandlersItem ?
								 CSaxBasic::catAction : CSaxBasic::catQuery,
								 GetHandlersName(dispidMember));
	tracecall.parm(_T("Me="),m_pdisp);
	if (dispidMember == DISPID_HandlersItem && pdispparams->cArgs == 1)
		tracecall.parm(_T("Index="),pdispparams->rgvarg,FALSE);
#endif
	HRESULT hr = m_pdisp->Invoke(dispidMember,riid,lcid,wFlags,pdispparams,
								 pvarResult,pexcepinfo,puArgErr);
	if (SUCCEEDED(hr))
		switch (dispidMember)
		{
		case DISPID_HandlersItem:
		#ifdef SB_TRACE_ANY
			tracecall.output();
			tracecall.parm(_T("Result="),V_DISPATCH(pvarResult));
		#endif
			V_DISPATCH(pvarResult) =
				CSaxBasicHandler::Create(m_pInstance,V_DISPATCH(pvarResult));
			break;
		#ifdef SB_TRACE_ANY
		case DISPID_HandlersCount:
			tracecall.output();
			tracecall.parm(_T("Result="),(long)V_I2(pvarResult));
			break;
		#endif
		}

	return hr;
}

LPDISPATCH CSaxBasicHandlers::Create(CSaxBasicInstance* pInstance,
									 LPDISPATCH pdisp)
{
	if (!pdisp)
		return 0;

	LPDISPATCH pdispOuter = pInstance->Map.Find(pdisp);
	if (pdispOuter)
		return pdispOuter;

	return new CSaxBasicHandlers(pInstance,pdisp);
}

///// CSaxBasicModule ///////////////////////////////////////////////////////

class CSaxBasicModule : public CSaxBasicObject
{
public:
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
    				  DISPPARAMS* pdispparams, VARIANT* pvarResult,
    				  EXCEPINFO* pexcepinfo, UINT* puArgErr);

	CSaxBasicModule(CSaxBasicInstance* pInstance, LPDISPATCH pdisp) :
		CSaxBasicObject(pInstance,pdisp) { }

	static LPDISPATCH Create(CSaxBasicInstance* pInstance, LPDISPATCH pdisp);
};

STDMETHODIMP
CSaxBasicModule::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
						DISPPARAMS* pdispparams, VARIANT* pvarResult,
						EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
#ifdef SB_TRACE_ANY
	CSaxBasicTraceCall tracecall(m_pInstance,CSaxBasic::catAction,_T("*Invoke"));
	tracecall.parm(_T("Me="),m_pdisp);
	tracecall.parm(_T("DispID="),dispidMember);
	tracecall.parm(pdispparams);
#endif
	HRESULT hr = m_pdisp->Invoke(dispidMember,riid,lcid,wFlags,pdispparams,
								 pvarResult,pexcepinfo,puArgErr);
#ifdef SB_TRACE_ANY
	tracecall.output();
	if (FAILED(hr))
		tracecall.parm(_T("Error="),(LPDISPATCH)hr);
	else if (pvarResult)
		tracecall.parm(_T("Result="),pvarResult,FALSE);
#endif
	return hr;
}

LPDISPATCH CSaxBasicModule::Create(CSaxBasicInstance* pInstance,
								   LPDISPATCH pdisp)
{
	if (!pdisp)
		return 0;

	LPDISPATCH pdispOuter = pInstance->Map.Find(pdisp);
	if (pdispOuter)
		return pdispOuter;

	return new CSaxBasicModule(pInstance,pdisp);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic constructor

CSaxBasic::CSaxBasic() :
	m_pInstance(NULL),
	m_pEdit(NULL)
{
}

CSaxBasic::~CSaxBasic()
{
	ASSERT(m_pEdit == NULL); // DetachWindow was not called
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic macro commands

BOOL CSaxBasic::ExecuteMenuCommand(CSaxBasic::CmdNum cmd)
{
	SB_TRACE_ACTION(_T("ExecuteMenuCommand"));
	SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

	ASSERT(this != NULL);
	if (this == NULL)
		return FALSE;

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult =
		(BOOL)m_pEdit->SendMessage(SB_MCM_MENUCOMMAND, (WPARAM)cmd, 1l);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CSaxBasic::IsMenuCommandChecked(CSaxBasic::CmdNum cmd) const
{
	SB_TRACE_QUERY(_T("IsMenuCommandChecked"));
	SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

	BOOL bResult = m_pEdit->SendMessage(SB_MCM_GETCMDCHECK, cmd, 0) != 0;
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CSaxBasic::IsMenuCommandEnabled(CSaxBasic::CmdNum cmd) const
{
	SB_TRACE_QUERY(_T("IsMenuCommandEnabled"));
	SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

	if (this == NULL)
		return FALSE;

	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_MENUCOMMAND, (WPARAM)cmd);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic color properties

COLORREF CSaxBasic::GetColor(CSaxBasic::ColorNum color) const
{
	long Color = color;
	SB_TRACE_QUERY(_T("GetColor"));
	SB_TRACE_PARMLITERAL(Color,GetColorLiteral);

	ASSERT(this != NULL);
	if (this == NULL)
		return 0;

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	ASSERT(color >= 0 && color < colorCount);
	COLORREF colorResult =
		m_pEdit->SendMessage(SB_MCM_GETCOLOR, (WPARAM)color);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(colorResult);
	return colorResult;
}

void CSaxBasic::SetColor(CSaxBasic::ColorNum color, COLORREF rgb)
{
	long Color = color;
	SB_TRACE_ACTION(_T("SetColor"));
	SB_TRACE_PARMLITERAL(Color,GetColorLiteral);
	SB_TRACE_PARM(rgb);

	ASSERT(this != NULL);
	if (this == NULL)
		return;

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	ASSERT(color >= 0 && color < colorCount);
	m_pEdit->SendMessage(SB_MCM_SETCOLOR, (WPARAM)color, rgb);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic operations

void CSaxBasic::Activate()
{
	SB_TRACE_ACTION(_T("Activate"));

	CWnd* pWnd = m_pEdit;
	while (pWnd)
	{
		CWnd* pWndParent = pWnd->GetParent();
		if (pWndParent->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
		{
			((CMDIChildWnd*)pWndParent)->MDIActivate();
			pWndParent->ShowWindow(SW_SHOW);
			break;
		}

		if (pWndParent->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			pWndParent->GetLastActivePopup()->BringWindowToTop();
			break;
		}

		pWnd = pWndParent;
	}
}

// return -1:nested, 0:okay, 1:canceled by user
short CSaxBasic::ClosingFrame(CFrameWnd* pFrame)
{
	short iResult = 0;
	CSaxBasic* pBasic = GetBasicLoader()->GetBasicFirst();
	while (pBasic)
	{
		if (pFrame == NULL ||
			pBasic->m_pEdit != NULL && pFrame->IsChild(pBasic->m_pEdit))
		{
			// make sure that the Basic CodeUnit is not executing (halt it)
			short iRes = pBasic->Shutdown();
			if (iRes > 0)
			{
				// canceled by user
				iResult = 1;
				break;
			}

			if (iRes < 0)
				iResult = -1; // nested
		}

		pBasic = pBasic->m_pBasicNext;
	}

	// -1: need to defer close (inside a peek message loop)
	// 0: document::CloseDocument must send WM_CLOSE to the CSaxBasic window
	// 1: canceled by user
	return iResult;
}

void CSaxBasic::DebugTrace(short nType, LPCSTR pszText)
{
#ifdef SB_TRACE_ANY
	CString s = pszText;
	m_pInstance->Trace(nType,s); // calls OnBasicDebugTrace
#endif
}

CSaxBasic* CSaxBasic::FindBasic(CFrameWnd* pFrame, CSaxBasic* pBasicPrev)
{
	CSaxBasic* pBasic = pBasicPrev ? pBasicPrev->m_pBasicNext :
									 GetBasicLoader()->GetBasicFirst();
	while (pBasic && (pBasic->m_pEdit == NULL ||
		   pFrame != NULL && !pFrame->IsChild(pBasic->m_pEdit)))
		pBasic = pBasic->m_pBasicNext;

	return pBasic;
}

void CSaxBasic::Load()
{
	GetBasicLoader()->Load();
}

void CSaxBasic::Free()
{
	GetBasicLoader()->Free();
}

BOOL CSaxBasic::ReportError(CException* e)
{
	SB_TRACE_ACTION(_T("ReportError"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit)
	{
		SB_MC_ReportError re;
		memset(&re,0,sizeof(re));
		if (e->IsKindOf(RUNTIME_CLASS(COleException)))
		{
			re.scode = ((COleException*)e)->m_sc;
			BOOL bResult =
				(BOOL)m_pEdit->SendMessage(SB_MCM_REPORTERROR,0,(LPARAM)&re);
			SB_TRACE_OUTPUT;
			SB_TRACE_PARM(bResult);
			return bResult;
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
			BOOL bResult =
				(BOOL)m_pEdit->SendMessage(SB_MCM_REPORTERROR,0,(LPARAM)&re);
			SB_TRACE_OUTPUT;
			SB_TRACE_PARM(bResult);
			return bResult;
		}
	}

#if _MFC_VER >= 0x0400
	e->ReportError();
#else
	AfxMessageBox(_T("Exception!"));
#endif
	BOOL bResult = FALSE;
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic operations

// *** new property *** (SetXxx(), GetXxx())
// *** new method *** (Xxx())

BOOL CSaxBasic::SetActiveSheet(short iSheet)
{
	SB_TRACE_ACTION(_T("SetActiveSheet"));
	SB_TRACE_PARMI2(iSheet);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = !m_pEdit->SendMessage(SB_MCM_SETACTIVESHEET,0,iSheet);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

short CSaxBasic::GetActiveSheet() const
{
	SB_TRACE_QUERY(_T("GetActiveSheet"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_GETACTIVESHEET);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetAlwaysSplit(BOOL bAlwaysSplit)
{	
	SB_TRACE_ACTION(_T("SetAlwaysSplit"));
	SB_TRACE_PARM(bAlwaysSplit);

	SetStyleBit(SAXBASIC_ALWAYSSPLIT,bAlwaysSplit);
}

BOOL CSaxBasic::GetAlwaysSplit() const
{   
	SB_TRACE_QUERY(_T("GetAlwaysSplit"));

	BOOL bResult = GetStyleBit(SAXBASIC_ALWAYSSPLIT);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetAttributeName(LPCTSTR pszAttributeName, short iSheet)
{
	SB_TRACE_ACTION(_T("SetAttributeName"));
	SB_TRACE_PARM(pszAttributeName);
	SB_TRACE_PARMI2(iSheet);

	if (SheetSelect(iSheet))
		m_pEdit->SendMessage(SB_MCM_SETATTRIBUTENAME, 0, (LPARAM)pszAttributeName);
}

void CSaxBasic::GetAttributeName(CString &sResult, short iSheet) const
{
	SB_TRACE_QUERY(_T("GetAttributeName"));
	SB_TRACE_PARMI2(iSheet);

	GetStringValue(SB_MCM_GETATTRIBUTENAME, sResult, iSheet);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetBlockedKeywords(LPCTSTR pszBlockedKeywords)
{
	SB_TRACE_ACTION(_T("SetBlockedKeywords"));
	SB_TRACE_PARM(pszBlockedKeywords);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETBLOCKEDKEYWORDS, 0, (LPARAM)pszBlockedKeywords);
}

void CSaxBasic::GetBlockedKeywords(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetBlockedKeywords"));

	GetStringValue(SB_MCM_GETBLOCKEDKEYWORDS, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetBreakPoints(const VARIANT& var, short iSheet)
{
	SB_TRACE_ACTION(_T("SetBreakPoints"));
//	SB_TRACE_PARM(var);
	SB_TRACE_PARMI2(iSheet);

	if (SheetSelect(iSheet))
		m_pEdit->SendMessage(SB_MCM_SETBREAKS, 0, (LPARAM)&var);
}

void CSaxBasic::GetBreakPoints(VARIANT& var, short iSheet) const
{
	SB_TRACE_QUERY(_T("GetBreakPoints"));
	SB_TRACE_PARMI2(iSheet);

	VariantClear(&var);
	SAFEARRAYBOUND sab[1];
	sab[0].cElements = 0;
	sab[0].lLbound = 1;
	LPSAFEARRAY psa = SafeArrayCreate(VT_I2,1,sab);
	V_VT(&var) = VT_ARRAY|VT_I2;
	V_ARRAY(&var) = psa;

	if (SheetSelect(iSheet))
		m_pEdit->SendMessage(SB_MCM_GETBREAKS, 0, (LPARAM)&var);

	SB_TRACE_OUTPUT;
//	SB_TRACE_PARM(var);
}

void CSaxBasic::SetCaption(LPCTSTR pszCaption)
{
	SB_TRACE_ACTION(_T("SetCaption"));
	SB_TRACE_PARM(pszCaption);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETCAPTION, 0, (LPARAM)pszCaption);
}

void CSaxBasic::GetCaption(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetCaption"));

	GetStringValue(SB_MCM_GETCAPTION, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetChanged(BOOL bModified, short iSheet)
{
	SB_TRACE_ACTION(_T("SetChanged"));
	SB_TRACE_PARM(bModified);
	SB_TRACE_PARMI2(iSheet);

	if (SheetSelect(iSheet))
		m_pEdit->SetModify(bModified);
}

BOOL CSaxBasic::GetChanged(short iSheet) const
{
	SB_TRACE_QUERY(_T("GetChanged"));
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult = SheetSelect(iSheet) && m_pEdit->GetModify();
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetCode(LPCTSTR pszCode, short iSheet)
{
	SB_TRACE_ACTION(_T("SetCode"));
	SB_TRACE_PARM(pszCode);
	SB_TRACE_PARMI2(iSheet);

	if (SheetSelect(iSheet))
		m_pEdit->SendMessage(SB_MCM_SETCODE, 0, (LPARAM)pszCode);
}

void CSaxBasic::GetCode(CString &sResult, short iSheet) const
{
	SB_TRACE_QUERY(_T("GetCode"));
	SB_TRACE_PARMI2(iSheet);

	GetStringValue(SB_MCM_GETCODE, sResult, iSheet);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetDebugHeight(short iHeight)
{
	SB_TRACE_ACTION(_T("SetDebugHeight"));
	SB_TRACE_PARMI2(iHeight);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETDEBUGHEIGHT, 0, iHeight);
}

short CSaxBasic::GetDebugHeight() const
{
	SB_TRACE_QUERY(_T("GetDebugHeight"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_GETDEBUGHEIGHT);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetDefaultDataType(LPCTSTR pszDefaultDataType)
{
	SB_TRACE_ACTION(_T("SetDefaultDataType"));
	SB_TRACE_PARM(pszDefaultDataType);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETDEFAULTDATATYPE, 0, (LPARAM)pszDefaultDataType);
}

void CSaxBasic::GetDefaultDataType(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetDefaultDataType"));

	GetStringValue(SB_MCM_GETDEFAULTDATATYPE, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetDefaultMacroName(LPCTSTR pszDefaultMacroName)
{
	SB_TRACE_ACTION(_T("SetDefaultMacroName"));
	SB_TRACE_PARM(pszDefaultMacroName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETDEFAULTMACRONAME, 0, (LPARAM)pszDefaultMacroName);
}

void CSaxBasic::GetDefaultMacroName(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetDefaultMacroName"));

	GetStringValue(SB_MCM_GETDEFAULTMACRONAME, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetDefaultObjectName(LPCTSTR pszDefaultObjectName)
{
	SB_TRACE_ACTION(_T("SetDefaultObjectName"));
	SB_TRACE_PARM(pszDefaultObjectName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETDEFAULTOBJECTNAME, 0, (LPARAM)pszDefaultObjectName);
}

void CSaxBasic::GetDefaultObjectName(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetDefaultObjectName"));

	GetStringValue(SB_MCM_GETDEFAULTOBJECTNAME, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetEditTools(BOOL bEditTools)
{	
	SB_TRACE_ACTION(_T("SetEditTools"));
	SB_TRACE_PARM(bEditTools);

	SetStyleBit(SAXBASIC_EDITBUTTONS,bEditTools);
}

BOOL CSaxBasic::GetEditTools() const
{   
	SB_TRACE_QUERY(_T("GetEditTools"));

	BOOL bResult = GetStyleBit(SAXBASIC_EDITBUTTONS);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetEnabled(BOOL bEnable)
{
	SB_TRACE_ACTION(_T("SetEnabled"));
	SB_TRACE_PARM(bEnable);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->EnableWindow(bEnable);
}

BOOL CSaxBasic::GetEnabled() const
{
	SB_TRACE_QUERY(_T("GetEnabled"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = m_pEdit->IsWindowEnabled();
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::GetErrorFile(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetErrorFile"));

    GetStringValue(SB_MCM_GETERRORFILE, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

long CSaxBasic::GetErrorHelpContext() const
{
	SB_TRACE_QUERY(_T("GetErrorHelpContext"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	long lResult = m_pEdit->SendMessage(SB_MCM_GETERRORHELPCONTEXT);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::GetErrorHelpFile(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetErrorHelpFile"));

	GetStringValue(SB_MCM_GETERRORHELPFILE, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

short CSaxBasic::GetErrorLine() const
{
	SB_TRACE_QUERY(_T("GetErrorLine"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_GETERRORLINE);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

long CSaxBasic::GetErrorNumber() const
{
	SB_TRACE_QUERY(_T("GetErrorNumber"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	long lResult = m_pEdit->SendMessage(SB_MCM_GETERRORNUMBER);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

short CSaxBasic::GetErrorOffset() const
{
	SB_TRACE_QUERY(_T("GetErrorOffset"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_GETERROROFFSET);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::GetErrorSource(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetErrorSource"));

	GetStringValue(SB_MCM_GETERRORSOURCE, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::GetErrorText(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetErrorText"));

	GetStringValue(SB_MCM_GETERRORTEXT, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetEvaluate(LPCTSTR pszExpression, LPCTSTR pszResult)
{
	SB_TRACE_ACTION(_T("SetEvaluate"));
	SB_TRACE_PARM(pszExpression);
	SB_TRACE_PARM(pszResult);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	SetExpression(pszExpression);
	SetResult(pszResult);
}

CString CSaxBasic::GetEvaluate(LPCTSTR pszExpression) const
{
	SB_TRACE_ACTION(_T("GetEvaluate"));
	SB_TRACE_PARM(pszExpression);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return _T("");

	CString sResult;
	((CSaxBasic*)this)->SetExpression(pszExpression);
	GetResult(sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
	return sResult;
}

void CSaxBasic::SetEventMode(BOOL bEventMode)
{
	SB_TRACE_ACTION(_T("SetEventMode"));
	SB_TRACE_PARM(bEventMode);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETEVENTMODE, 0, (LPARAM)bEventMode);
}

BOOL CSaxBasic::GetEventMode() const
{
	SB_TRACE_QUERY(_T("GetEventMode"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_GETEVENTMODE);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetExpression(LPCTSTR pszExpression)
{
	SB_TRACE_ACTION(_T("SetExpression"));
	SB_TRACE_PARM(pszExpression);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETEXPRESSION, 0, (LPARAM)pszExpression);
}

void CSaxBasic::GetExpression(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetExpression"));

	GetStringValue(SB_MCM_GETEXPRESSION, sResult);
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetFileChangeDir(BOOL bFileChangeDir)
{
	SB_TRACE_ACTION(_T("SetFileChangeDir"));
	SB_TRACE_PARM(bFileChangeDir);

	SetStyleBit(SAXBASIC_NOCHANGEDIR,!bFileChangeDir);
}

BOOL CSaxBasic::GetFileChangeDir() const
{
	SB_TRACE_QUERY(_T("GetFileChangeDir"));

	BOOL bResult = !GetStyleBit(SAXBASIC_NOCHANGEDIR);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetFileDesc(LPCTSTR pszDescription)
{
	SB_TRACE_ACTION(_T("SetFileDesc"));
	SB_TRACE_PARM(pszDescription);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETFILEDESC, 0, (LPARAM)pszDescription);
}

void CSaxBasic::GetFileDesc(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetFileDesc"));

	GetStringValue(SB_MCM_GETFILEDESC, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetFileExt(LPCTSTR pszExtension)
{
	SB_TRACE_ACTION(_T("SetFileExt"));
	SB_TRACE_PARM(pszExtension);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETFILEEXT, 0, (LPARAM)pszExtension);
}

void CSaxBasic::GetFileExt(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetFileExt"));

	GetStringValue(SB_MCM_GETFILEEXT, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

BOOL CSaxBasic::SetFilename(LPCTSTR pszFileName, short iSheet)
{
	SB_TRACE_ACTION(_T("SetFileName"));
	SB_TRACE_PARM(pszFileName);
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult =
		SheetSelect(iSheet) &&
		!m_pEdit->SendMessage(SB_MCM_SETFILENAME, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::GetFilename(CString &sResult, short iSheet) const
{
	SB_TRACE_QUERY(_T("GetFileName"));
	SB_TRACE_PARMI2(iSheet);

	GetStringValue(SB_MCM_GETFILENAME, sResult, iSheet);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetFileOpenChangeDir(BOOL bFileOpenChangeDir)
{
	SB_TRACE_ACTION(_T("SetFileOpenChangeDir"));
	SB_TRACE_PARM(bFileOpenChangeDir);

	SetStyleBit(SAXBASIC_NOOPENCHANGEDIR,!bFileOpenChangeDir);
}

BOOL CSaxBasic::GetFileOpenChangeDir() const
{
	SB_TRACE_QUERY(_T("GetFileOpenChangeDir"));

	BOOL bResult = !GetStyleBit(SAXBASIC_NOOPENCHANGEDIR);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetFileSaveChangeDir(BOOL bFileSaveChangeDir)
{
	SB_TRACE_ACTION(_T("SetFileSaveChangeDir"));
	SB_TRACE_PARM(bFileSaveChangeDir);

	SetStyleBit(SAXBASIC_NOSAVECHANGEDIR,!bFileSaveChangeDir);
}

BOOL CSaxBasic::GetFileSaveChangeDir() const
{
	SB_TRACE_QUERY(_T("GetFileSaveChangeDir"));

	BOOL bResult = !GetStyleBit(SAXBASIC_NOSAVECHANGEDIR);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetFileTools(BOOL bFileTools)
{
	SB_TRACE_ACTION(_T("SetFileTools"));
	SB_TRACE_PARM(bFileTools);

	SetStyleBit(SAXBASIC_USERFILEIO,bFileTools);
}

BOOL CSaxBasic::GetFileTools() const
{
	SB_TRACE_QUERY(_T("GetFileTools"));

	BOOL bResult = GetStyleBit(SAXBASIC_USERFILEIO);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetFullPopupMenu(BOOL bFullPopupMenu)
{
	SB_TRACE_ACTION(_T("SetFullPopupMenu"));
	SB_TRACE_PARM(bFullPopupMenu);

	SetStyleBit(SAXBASIC_NOPOPUPMENU,!bFullPopupMenu);
}

BOOL CSaxBasic::GetFullPopupMenu() const
{
	SB_TRACE_QUERY(_T("GetFullPopupMenu"));

	BOOL bResult = !GetStyleBit(SAXBASIC_NOPOPUPMENU);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

short CSaxBasic::GetHeaderLineCount(short iSheet) const
{
	SB_TRACE_QUERY(_T("HeaderLineCount"));
	SB_TRACE_PARMI2(iSheet);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = 0;
	if (SheetSelect(iSheet))
		iResult = (short)m_pEdit->SendMessage(SB_MCM_GETHEADERLINECOUNT);

	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetHiddenCode(LPCTSTR pszHiddenCode)
{
	SB_TRACE_ACTION(_T("SetHiddenCode"));
	SB_TRACE_PARM(pszHiddenCode);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETHIDDENCODE, 0, (LPARAM)pszHiddenCode);
}

void CSaxBasic::GetHiddenCode(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetHiddenCode"));

	GetStringValue(SB_MCM_GETHIDDENCODE, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetHighlightBreak(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightBreak"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorBreak,lColor);
}

unsigned long CSaxBasic::GetHighlightBreak() const
{
	SB_TRACE_QUERY(_T("GetHighlightBreak"));

	unsigned long lResult = GetHighlightColor(colorBreak);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightBuiltin(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightBuiltin"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorBuiltin,lColor);
}

unsigned long CSaxBasic::GetHighlightBuiltin() const
{
	SB_TRACE_QUERY(_T("GetHighlightBuiltin"));

	unsigned long lResult = GetHighlightColor(colorBuiltin);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightComment(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightComment"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorComment,lColor);
}

unsigned long CSaxBasic::GetHighlightComment() const
{
	SB_TRACE_QUERY(_T("GetHighlightComment"));

	unsigned long lResult = GetHighlightColor(colorComment);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightError(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightError"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorError,lColor);
}

unsigned long CSaxBasic::GetHighlightError() const
{
	SB_TRACE_QUERY(_T("GetHighlightError"));

	unsigned long lResult = GetHighlightColor(colorError);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightExec(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightExec"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorExec,lColor);
}

unsigned long CSaxBasic::GetHighlightExec() const
{
	SB_TRACE_QUERY(_T("GetHighlightExec"));

	unsigned long lResult = GetHighlightColor(colorExec);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightExtension(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightExtension"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorExtension,lColor);
}

unsigned long CSaxBasic::GetHighlightExtension() const
{
	SB_TRACE_QUERY(_T("GetHighlightExtension"));

	unsigned long lResult = GetHighlightColor(colorExtension);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetHighlightReserved(unsigned long lColor)
{
	SB_TRACE_ACTION(_T("SetHighlightReserved"));
	SB_TRACE_PARM(lColor);

	SetHighlightColor(colorReserved,lColor);
}

unsigned long CSaxBasic::GetHighlightReserved() const
{
	SB_TRACE_QUERY(_T("GetHighlightReserved"));

	unsigned long lResult = GetHighlightColor(colorReserved);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetLocked(BOOL bLocked)
{
	SB_TRACE_ACTION(_T("SetLocked"));
	SB_TRACE_PARM(bLocked);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETLOCKED, 0, (LPARAM)bLocked);
}

BOOL CSaxBasic::GetLocked() const
{
	SB_TRACE_QUERY(_T("GetLocked"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_GETLOCKED);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

CString CSaxBasic::GetMenuCommandDesc(CSaxBasic::CmdNum cmd) const
{
	SB_TRACE_QUERY(_T("GetMenuCommandDesc"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return _T("");

	TCHAR szResult[256];
	*szResult = _T('\0');
	m_pEdit->SendMessage(SB_MCM_GETCMDDESC256, cmd, (LPARAM)(LPCTSTR)szResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(szResult);
	return szResult;
}

CString CSaxBasic::GetMenuCommandTooltip(CSaxBasic::CmdNum cmd) const
{
	SB_TRACE_QUERY(_T("GetMenuCommandTooltip"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return _T("");

	TCHAR szResult[256];
	*szResult = _T('\0');
	m_pEdit->SendMessage(SB_MCM_GETCMDTOOLTIP256, cmd, (LPARAM)(LPCTSTR)szResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(szResult);
	return szResult;
}

short CSaxBasic::GetModuleKind(short iSheet) const
{
	SB_TRACE_QUERY(_T("ModuleKind"));
	SB_TRACE_PARMI2(iSheet);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = 0;
	if (SheetSelect(iSheet))
		iResult = (short)m_pEdit->SendMessage(SB_MCM_GETMODULEKIND);

	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetMultiSheet(BOOL bMultiSheet)
{	
	SB_TRACE_ACTION(_T("SetMultiSheet"));
	SB_TRACE_PARM(bMultiSheet);

	SetStyleBit(SAXBASIC_MULTISHEET,bMultiSheet);
}

BOOL CSaxBasic::GetMultiSheet() const
{   
	SB_TRACE_QUERY(_T("GetMultiSheet"));

	BOOL bResult = GetStyleBit(SAXBASIC_MULTISHEET);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetPause(BOOL bPause)
{
	SB_TRACE_ACTION(_T("SetPause"));
	SB_TRACE_PARM(bPause);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETPAUSE, 0, (LPARAM)bPause);
}

BOOL CSaxBasic::GetPause() const
{
	SB_TRACE_QUERY(_T("GetPause"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_GETPAUSE);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetProcDisplayMode(short iProcDisplayMode)
{
	SB_TRACE_ACTION(_T("SetProcDisplayMode"));
	SB_TRACE_PARMI2(iProcDisplayMode);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETPROCDISPLAYMODE,0,iProcDisplayMode);
}

short CSaxBasic::GetProcDisplayMode() const
{
	SB_TRACE_QUERY(_T("GetProcDisplayMode"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_GETPROCDISPLAYMODE);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetResult(LPCTSTR pszResult)
{
	SB_TRACE_ACTION(_T("SetResult"));
	SB_TRACE_PARM(pszResult);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETRESULT, 0, (LPARAM)pszResult);
}

void CSaxBasic::GetResult(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetResult"));

	GetStringValue(SB_MCM_GETRESULT, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

void CSaxBasic::SetRun(BOOL bRun)
{
	SB_TRACE_ACTION(_T("SetRun"));
	SB_TRACE_PARM(bRun);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETRUN, 0, (LPARAM)bRun);
}

BOOL CSaxBasic::GetRun() const
{
	SB_TRACE_QUERY(_T("GetRun"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_GETRUN);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetSelLength(long lLength)
{
	SB_TRACE_ACTION(_T("SetSelLength"));
	SB_TRACE_PARM(lLength);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETSELLENGTH, 0, lLength);
}

long CSaxBasic::GetSelLength() const
{
	SB_TRACE_QUERY(_T("GetSelLength"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	long lResult = m_pEdit->SendMessage(SB_MCM_GETSELLENGTH);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetSelStart(long lStart)
{
	SB_TRACE_ACTION(_T("SetSelStart"));
	SB_TRACE_PARM(lStart);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETSELSTART, 0, lStart);
}

long CSaxBasic::GetSelStart() const
{
	SB_TRACE_QUERY(_T("GetSelStart"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	long lResult = m_pEdit->SendMessage(SB_MCM_GETSELSTART);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lResult);
	return lResult;
}

void CSaxBasic::SetSelText(LPCTSTR pszText)
{
	SB_TRACE_ACTION(_T("SetSelText"));
	SB_TRACE_PARM(pszText);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETSELTEXT, 0, (LPARAM)pszText);
}

CString CSaxBasic::GetSelText() const
{
	SB_TRACE_QUERY(_T("GetSelText"));

	CString sResult;
	GetStringValue(SB_MCM_GETSELTEXT, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
	return sResult;
}

void CSaxBasic::SetStatusVisible(BOOL bStatusVisible)
{
	SB_TRACE_ACTION(_T("SetStatusVisible"));
	SB_TRACE_PARM(bStatusVisible);

	SetStyleBit(SAXBASIC_STATUSBAR,bStatusVisible);
}

BOOL CSaxBasic::GetStatusVisible() const
{
	SB_TRACE_QUERY(_T("GetStatusVisible"));

	BOOL bResult = GetStyleBit(SAXBASIC_STATUSBAR);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetTabAsSpaces(BOOL bTabAsSpaces)
{
	SB_TRACE_ACTION(_T("SetTabAsSpaces"));
	SB_TRACE_PARM(bTabAsSpaces);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETTABASSPACES, 0, (LPARAM)bTabAsSpaces);
}

BOOL CSaxBasic::GetTabAsSpaces() const
{
	SB_TRACE_QUERY(_T("GetTabAsSpaces"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_GETTABASSPACES);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::SetTabWidth(short iTabWidth)
{
	SB_TRACE_ACTION(_T("SetTabWidth"));
	SB_TRACE_PARMI2(iTabWidth);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETTABWIDTH, 0, (LPARAM)iTabWidth);
}

short CSaxBasic::GetTabWidth() const
{
	SB_TRACE_QUERY(_T("GetTabWidth"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL iResult = (short)m_pEdit->SendMessage(SB_MCM_GETTABWIDTH);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::SetText(LPCTSTR pszText)
{
	SB_TRACE_ACTION(_T("SetText"));
	SB_TRACE_PARM(pszText);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETTEXT, 0, (LPARAM)pszText);
}

CString CSaxBasic::GetText() const
{
	SB_TRACE_QUERY(_T("GetText"));

	CString sResult;
	GetStringValue(SB_MCM_GETTEXT, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
	return sResult;
}

void CSaxBasic::SetToolbarVisible(BOOL bToolbarVisible)
{
	SB_TRACE_ACTION(_T("SetToolbarVisible"));
	SB_TRACE_PARM(bToolbarVisible);

	SetStyleBit(SAXBASIC_TOOLBAR,bToolbarVisible);
}

BOOL CSaxBasic::GetToolbarVisible() const
{
	SB_TRACE_QUERY(_T("GetToolbarVisible"));

	BOOL bResult = GetStyleBit(SAXBASIC_TOOLBAR);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CSaxBasic::SyntaxCheck() const
{
	SB_TRACE_ACTION(_T("SyntaxCheck"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = (BOOL)m_pEdit->SendMessage(SB_MCM_SYNTAXCHECK);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::RunFile(LPCTSTR pszFileName)
{
	SB_TRACE_ACTION(_T("RunFile"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_RUNFILE, 0, (LPARAM)pszFileName);
}

void CSaxBasic::RunThis(LPCTSTR pszRunThis)
{
	SB_TRACE_ACTION(_T("RunThis"));
	SB_TRACE_PARM(pszRunThis);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_RUNTHIS, 0, (LPARAM)pszRunThis);
}

#ifdef SB_ONE_DLL_PREFIX
BOOL CSaxBasic::AddExtensionWithEvents(LPCTSTR pszName, LPDISPATCH pDisp)
{
	SB_TRACE_ACTION(_T("AddExtensionWithEvents"));
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(pDisp);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	m_pEdit->SendMessage(SB_MCM_SETEXTENSIONPREFIX, 0, (LPARAM)pszName);
	BOOL bResult = !m_pEdit->SendMessage(SB_MCM_ADDEXTENSION, 1, (LPARAM)pDisp);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}
#endif

BOOL CSaxBasic::CloseSheet(short iSheet)
{
	SB_TRACE_ACTION(_T("CloseSheet"));
	SB_TRACE_PARMI2(iSheet);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = !m_pEdit->SendMessage(SB_MCM_CLOSESHEET,0,iSheet);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

LPDISPATCH CSaxBasic::CreateHandler(LPCTSTR pszPrototype)
{
	SB_TRACE_ACTION(_T("CreateHandler"));
	SB_TRACE_PARM(pszPrototype);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	LPDISPATCH pResult =
		(LPDISPATCH)m_pEdit->SendMessage(SB_MCM_CREATEHANDLER, 0, (LPARAM)pszPrototype);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pResult);
	return CSaxBasicHandler::Create(m_pInstance,pResult);
}

LPDISPATCH CSaxBasic::CreateHandlers(LPCTSTR pszPrefix, LPDISPATCH pEventObj)
{
	SB_TRACE_ACTION(_T("CreateHandlers"));
	SB_TRACE_PARM(pszPrefix);
	SB_TRACE_PARM(pEventObj);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	LPARAM lParams[2] = {(LPARAM)pszPrefix,(LPARAM)pEventObj};
	LPDISPATCH pResult =
		(LPDISPATCH)m_pEdit->SendMessage(SB_MCM_CREATEHANDLERS, 0, (LPARAM)lParams);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pResult);
	return CSaxBasicHandlers::Create(m_pInstance,pResult);
}

BOOL CSaxBasic::Disconnect()
{
	if (m_pEdit == NULL)
		return TRUE;

	SB_TRACE_ACTION(_T("Disconnect"));

	BOOL bResult = !m_pEdit->SendMessage(WM_CLOSE);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CSaxBasic::FindOrCreateProc(LPCTSTR pszProcName)
{
	SB_TRACE_ACTION(_T("FindOrCreateProc"));
	SB_TRACE_PARM(pszProcName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_FINDORCREATEPROC, 0, (LPARAM)pszProcName);
}

short CSaxBasic::FindSheet(LPCTSTR pszFileName)
{
	SB_TRACE_QUERY(_T("FindSheet"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult =
		(short)m_pEdit->SendMessage(SB_MCM_FINDSHEET, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::GetMacroCaption(CString& sMacroCaption) const
{
	SB_TRACE_QUERY(_T("GetMacroCaption"));
	SB_TRACE_PARM(sMacroCaption);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	LPTSTR pBuffer = sMacroCaption.GetBuffer(_MAX_PATH*sizeof(TCHAR));
	m_pEdit->SendMessage(SB_MCM_GETMACROCAPTION,_MAX_PATH*sizeof(TCHAR),
						 (LPARAM)pBuffer);
	sMacroCaption.ReleaseBuffer();
	SB_TRACE_PARM(sMacroCaption);
}

void CSaxBasic::GetMacroName(CString &sResult) const
{
	SB_TRACE_QUERY(_T("GetMacroName"));

	GetStringValue(SB_MCM_GETMACRONAME, sResult);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sResult);
}

long CSaxBasic::IndexFromLine(short iLine) const
{
	SB_TRACE_QUERY(_T("IndexFromLine"));
	SB_TRACE_PARM(iLine);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	long Result = m_pEdit->SendMessage(SB_MCM_INDEXFROMLINE,0,iLine);

	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(Result);
	return Result;
}

BOOL CSaxBasic::IsModuleLoaded(LPCTSTR pszFileName) const
{
	SB_TRACE_QUERY(_T("IsModuleLoaded"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult =
		!m_pEdit->SendMessage(SB_MCM_ISMODULELOADED, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

short CSaxBasic::LineFromIndex(long lIndex) const
{
	SB_TRACE_QUERY(_T("LineFromIndex"));
	SB_TRACE_PARM(lIndex);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short Result = (short)m_pEdit->SendMessage(SB_MCM_LINEFROMINDEX,0,lIndex);

	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(Result);
	return Result;
}

BOOL CSaxBasic::LoadFilename(LPCTSTR pszFileName, short iSheet)
{
	SB_TRACE_ACTION(_T("LoadFilename"));
	SB_TRACE_PARM(pszFileName);
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult =
		SheetSelect(iSheet) &&
		!m_pEdit->SendMessage(SB_MCM_STOREFILENAME, 1, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CSaxBasic::LoadModule(LPCTSTR pszFileName)
{
	SB_TRACE_ACTION(_T("LoadModule"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult =
		!m_pEdit->SendMessage(SB_MCM_LOADMODULE, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

LPDISPATCH CSaxBasic::ModuleInstance(LPCTSTR pszFileName, BOOL bNewInstance)
{
	SB_TRACE_ACTION(_T("ModuleInstance"));
	SB_TRACE_PARM(pszFileName);
	SB_TRACE_PARM(bNewInstance);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	LPDISPATCH pResult =
		(LPDISPATCH)m_pEdit->SendMessage(SB_MCM_MODULEINSTANCE, bNewInstance, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pResult);
	return CSaxBasicModule::Create(m_pInstance,pResult);
}

BOOL CSaxBasic::RemoveExtensions(LPCTSTR pszFileName)
{
	SB_TRACE_ACTION(_T("RemoveExtensions"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	m_pEdit->SendMessage(SB_MCM_SETEXTENSIONPREFIX, 0, (LPARAM)pszFileName);
	BOOL bResult = !m_pEdit->SendMessage(SB_MCM_REMOVEEXTENSIONS, 0, 0);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CSaxBasic::SaveFilenameAs(LPCTSTR pszFileName, short iSheet)
{
	SB_TRACE_ACTION(_T("SaveFilenameAs"));
	SB_TRACE_PARM(pszFileName);
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult =
		SheetSelect(iSheet) &&
		!m_pEdit->SendMessage(SB_MCM_STOREFILENAME, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

short CSaxBasic::SheetCount() const
{
	SB_TRACE_QUERY(_T("SheetCount"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_SHEETCOUNT);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

short CSaxBasic::Shutdown()
{
	SB_TRACE_ACTION(_T("Shutdown"));

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	short iResult = (short)m_pEdit->SendMessage(SB_MCM_SHUTDOWN);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARMI2(iResult);
	return iResult;
}

void CSaxBasic::Trace(short categories)
{
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pInstance->SetCategories(categories);
	m_pEdit->SendMessage(SB_MCM_TRACE,0,categories);
}

BOOL CSaxBasic::UnloadModule(LPCTSTR pszFileName)
{
	SB_TRACE_ACTION(_T("UnloadModule"));
	SB_TRACE_PARM(pszFileName);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult =
		!m_pEdit->SendMessage(SB_MCM_UNLOADMODULE, 0, (LPARAM)pszFileName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic advanced operations

BOOL CSaxBasic::AddExtensionWide(LPDISPATCH pDisp, LPCTSTR pszPrefix)
{
	SB_TRACE_ACTION(_T("AddExtension"));
	SB_TRACE_PARM(pszPrefix);
	SB_TRACE_PARM(pDisp);

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	m_pEdit->SendMessage(SB_MCM_SETEXTENSIONPREFIX, 0, (LPARAM)pszPrefix);
	BOOL bResult = !m_pEdit->SendMessage(SB_MCM_ADDEXTENSION, 0, (LPARAM)pDisp);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic implementation

void CSaxBasic::AttachWindow(CEdit* pEdit)
{
	ASSERT(m_pEdit == NULL);
	m_pEdit = pEdit;
}

void CSaxBasic::DetachWindow()
{
	if (m_pEdit != NULL)
	{
		// make sure that the Basic engine has be completely shut down
		ASSERT(m_pEdit->SendMessage(SB_MCM_CODEUNITDESTROYED) != 0);
		m_pEdit = NULL;
	}
}

void CSaxBasic::Initialize()
{
#ifdef SB_TRACE_ANY
	m_pInstance->TraceEnter(_T("*Initialize CSaxBasic v") SB_VERSION);
	CString sVersion;
	GetStringValue(SB_MCM_GETVERSIONINFO,sVersion);
#ifdef SB_ONE_DLL_PREFIX
	sVersion = CString(SB_BASICEDITORDLL _T(" "))+sVersion;
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("*EngineDLL="),sVersion);
  #ifdef WIN32
	DWORD dwThreadId = GetCurrentThreadId();
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("*Thread="),(LPDISPATCH)dwThreadId);
  #endif
#else
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("*EngineDLLs="),sVersion);
#endif
	DWORD dwStyle = m_pEdit->GetStyle();
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("EditTools="),(BOOL)((dwStyle&SAXBASIC_EDITBUTTONS) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("FileTools="),(BOOL)((dwStyle&SAXBASIC_USERFILEIO) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("StatusVisible="),(BOOL)((dwStyle&SAXBASIC_STATUSBAR) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("ToolbarVisible="),(BOOL)((dwStyle&SAXBASIC_TOOLBAR) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("AlwaysSplit="),(BOOL)((dwStyle&SAXBASIC_ALWAYSSPLIT) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("MultiSheet="),(BOOL)((dwStyle&SAXBASIC_MULTISHEET) != 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("FullPopupMenu="),(BOOL)((dwStyle&SAXBASIC_NOPOPUPMENU) == 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("FileOpenChangeDir="),(BOOL)((dwStyle&SAXBASIC_NOOPENCHANGEDIR) == 0));
	m_pInstance->TraceParm(SB_TRACEINPUT,_T("FileSaveChangeDir="),(BOOL)((dwStyle&SAXBASIC_NOSAVECHANGEDIR) == 0));
	m_pInstance->TraceLeave();

	short categories = 0;
	#ifdef SB_TRACE_ACTION_CALLS
		categories |= catAction;
	#endif
	#ifdef SB_TRACE_QUERY_CALLS
		categories |= catQuery;
	#endif
	#ifdef SB_TRACE_ACTION_CALLBACKS
		categories |= catActionCallback;
	#endif
	#ifdef SB_TRACE_QUERY_CALLBACKS
		categories |= catQueryCallback;
	#endif
	#ifdef SB_TRACE_INTERNAL
		categories |= catInternal;
	#endif
	#ifdef SB_TRACE_EXECUTION
		categories |= catExecution;
	#endif
	#ifdef SB_TRACE_ALL
		categories |= catAll;
	#endif

	Trace(categories);
#endif

#ifndef WW_GWL_USER
	SetCaption(_T("Sax Basic"));
	SetFileDesc(_T("Sax Basic"));
	SetFileExt(_T("BAS"));
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic protected operation

#if defined(WIN32) && defined(OLE2ANSI)
// WrapIDispatchWFromA defined in MFCANS32.DLL
STDAPI WrapIDispatchWFromA(LPDISPATCH pANSI, LPDISPATCH* pWide);

BOOL CSaxBasic::AddExtensionWrap(LPDISPATCH pDisp, LPCTSTR pszPrefix)
{
	BOOL fOk = FALSE;
	LPDISPATCH pWide = 0;
	HRESULT hr = WrapIDispatchWFromA(pDisp, &pWide);
	if (hr == NOERROR)
	{
		fOk = AddExtensionWide(pWide, pszPrefix);
		pWide->Release();
	}

	return fOk;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic private operation

unsigned long CSaxBasic::GetHighlightColor(short iColor) const
{
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return 0;

	return m_pEdit->SendMessage(SB_MCM_GETCOLOR,iColor);
}

void CSaxBasic::GetStringValue(WORD msg, CString &sResult, short iSheet) const
{
	if (SheetSelect(iSheet))
	{
		int cb = (int)m_pEdit->SendMessage(msg) + 1;
		LPTSTR pBuffer = sResult.GetBuffer(cb);
		if (pBuffer != NULL)
		{
			if (iSheet != 0)
				m_pEdit->SendMessage(SB_MCM_SHEETSELECT,0,iSheet);

			m_pEdit->SendMessage(msg, cb, (LPARAM)pBuffer);
			sResult.ReleaseBuffer();	
		}
	}
}

BOOL CSaxBasic::GetStyleBit(WORD wStyle) const
{   
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	return (m_pEdit->GetStyle() & wStyle) != 0;	
}

void CSaxBasic::SetHighlightColor(short iColor, unsigned long lColor)
{
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETCOLOR,iColor,lColor);
}

void CSaxBasic::SetStyleBit(WORD wStyle, BOOL bSet)
{	
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return;

	m_pEdit->SendMessage(SB_MCM_SETVISIBLE, wStyle, bSet ? wStyle : 0);
}

BOOL CSaxBasic::SheetSelect(short iSheet) const
{
	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	return !iSheet || m_pEdit->SendMessage(SB_MCM_SHEETSELECT,0,iSheet);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasic Virtual call-back functions

// *** new event *** (OnBasicXxx())

void CSaxBasic::OnBasicAboutBox()
{
#ifndef WW_GWL_USER
	AfxMessageBox(_T("Sax Basic Engine\n")
				  _T("Copyright 1993-1997 Sax Software, Inc.\n")
				  _T("Copyright 1993-1997 Polar Engineering and Consulting\n")
				  _T("All rights reserved.\n\n"),MB_ICONINFORMATION);
#else
	AfxMessageBox(_T("WinWrap Basic\n")
				  _T("Copyright 1993-1997 Polar Engineering and Consulting\n")
				  _T("All rights reserved.\n\n")
				  _T("1-800-820-2920 or 1-907-776-5509"),MB_ICONINFORMATION);
#endif
}

void CSaxBasic::OnBasicBegin()
{
}

void CSaxBasic::OnBasicDebugPrint(LPCSTR pszText)
{
}

void CSaxBasic::OnBasicDebugTrace(LPCTSTR pszText)
{
	TRACE1("%s\n",pszText);
}

void CSaxBasic::OnBasicDialogIdle()
{
}

void CSaxBasic::OnBasicDoEvents()
{
	OnUpdateMFCUI(TRUE);

	CWinThread* pThread = AfxGetThread();
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

void CSaxBasic::OnBasicEnd()
{
}

void CSaxBasic::OnBasicErrorAlert()
{
}

void CSaxBasic::OnBasicHandleError()
{
}

BOOL CSaxBasic::OnBasicHelp(LPCSTR pszKeyword)
{
	return FALSE;
}

void CSaxBasic::OnBasicMacroBegin(LPCTSTR pszFileName)
{
}

void CSaxBasic::OnBasicMacroEnd(LPCTSTR pszFileName)
{
}

void CSaxBasic::OnBasicModalDialog(BOOL bDisplayed)
{
	AfxGetApp()->EnableModeless(!bDisplayed);
}

void CSaxBasic::OnBasicNotify(short nNotify)
{
	CSaxBasic* pThis = this;
	SB_TRACE_ACTION_CALLBACK(GetNotifyProcName(nNotify));

	switch (nNotify)
	{
	case SB_NOTIFYERRORS:
		{
			CString ErrorText;
			GetStringValue(SB_MCM_GETERRORTEXT,ErrorText);
			SB_TRACE_PARM(ErrorText);
			OnBasicErrorAlert();
		}

		break;
	case SB_NOTIFYBEGIN:
		OnUpdateMFCUI(FALSE);
		OnBasicBegin();
		break;
	case SB_NOTIFYEND:
		OnUpdateMFCUI(FALSE);
		OnBasicEnd();
		break;
	case SB_NOTIFYPAUSE:
		OnUpdateMFCUI(FALSE);
		OnBasicPause();
		break;
	case SB_NOTIFYRESUME:
		OnUpdateMFCUI(FALSE);
		OnBasicResume();
		break;
	case SB_NOTIFYERRORLOG:
		// do nothing (SB_NOTIFYERRORS will handle it)
		break;
	case SB_NOTIFYPREMODAL:
		{
			BOOL Displayed = TRUE;
			SB_TRACE_PARM(Displayed);
			OnBasicModalDialog(TRUE);
		}

		break;
	case SB_NOTIFYPOSTMODAL:
		{
			BOOL Displayed = FALSE;
			SB_TRACE_PARM(Displayed);
			OnBasicModalDialog(FALSE);
		}

		break;
	case SB_NOTIFYPENDING:
		OnBasicPending();
		break;
	case SB_NOTIFYMACROBEGIN:
	case SB_NOTIFYMACROEND:
		OnUpdateMFCUI(FALSE);
		{
			CString FileName;
			GetStringValue(SB_MCM_GETMACRONAME,FileName);
			SB_TRACE_PARM(FileName);
			if (nNotify == SB_NOTIFYMACROBEGIN)
				OnBasicMacroBegin(FileName);
			else
				OnBasicMacroEnd(FileName);

			break;
		}
	case SB_NOTIFYCHANGE:
		OnChange();
		break;
	case SB_NOTIFYSHOWFORM:
		OnBasicShowForm();
		break;
	case SB_NOTIFYDIALOGIDLE:
		OnBasicDialogIdle();
		break;
	case SB_NOTIFYHANDLEERROR:
		OnBasicHandleError();
		break;
	}
}

void CSaxBasic::OnBasicPause()
{
}

void CSaxBasic::OnBasicPending()
{
}

void CSaxBasic::OnBasicResume()
{
}

void CSaxBasic::OnBasicShowForm()
{
}

void CSaxBasic::OnBasicStatusbar(LPCSTR pszText, BOOL bAlert)
{
	if (bAlert)
		MessageBeep(MB_ICONASTERISK);

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if (pFrame == NULL || pFrame->m_hWnd == NULL ||
		!pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		pFrame = NULL;

	CString s = pszText;
	if (pFrame != NULL && pFrame->GetMessageBar() != NULL)
	{
	#if _MFC_VER >= 0x0300
		pFrame->SetMessageText(s);
	#else
		pFrame->SendMessage(0x0362, 0, (LPARAM)(LPCTSTR)s);
	#endif
	}
	else if (bAlert)
		AfxMessageBox(s,MB_OK);
}

HWND CSaxBasic::OnBasicWindowHandle()
{
	// get parent window for modal dialogs and message boxes.
	CWnd* pParent = AfxGetMainWnd();
	if (pParent == NULL || pParent->m_hWnd == NULL)
		return NULL;
	ASSERT_VALID(pParent);
	return pParent->m_hWnd;
}

void CSaxBasic::OnChange()
{
}

BOOL CSaxBasic::OnCloseSheet(short iSheet, BOOL* pbCancel)
{
	return FALSE;
}

void CSaxBasic::OnGetMacroCaption(LPSTR pszCaption, UINT cbLen)
{
}

BOOL CSaxBasic::OnGetMacroName(BOOL bOpen, LPSTR pszFileName, UINT cbLen)
{
	return FALSE;
}

LPDISPATCH CSaxBasic::OnNewInstance(LPCSTR pszFileName)
{
	return NULL;
}

BOOL CSaxBasic::OnOverrideMenuCommand(CSaxBasic::CmdNum cmd)
{
	return FALSE;
}

BOOL CSaxBasic::OnParseProperties(LPCSTR pszFileName, LPCSTR pszProperties)
{
	return TRUE;
}

UINT CSaxBasic::OnReadMacro(LPCSTR pszFileName, LPSTR pszCode)
{
	return 0;
}

void CSaxBasic::OnUpdateMFCUI(BOOL bForDoEvents)
{
	AfxGetThread()->OnIdle(0); // update tool bar buttons
}

void CSaxBasic::OnUpdateProperties(LPCSTR pszFileName, CString& sProperties)
{
}

BOOL CSaxBasic::OnWriteMacro(LPCSTR pszFileName, LPCSTR pszCode)
{
	return FALSE;
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
// CSaxBasicEdit

IMPLEMENT_DYNAMIC(CSaxBasicEdit, CEdit)

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit call-back procs

void CALLBACK EXPORT SaxBasicEditAboutBoxProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicAboutBox"));

	pThis->OnBasicAboutBox();
}

void CALLBACK EXPORT SaxBasicEditDebugPrintProc(HWND hWnd, LPCSTR pszText)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicDebugPrint"));
	SB_TRACE_PARM(pszText);

	pThis->OnBasicDebugPrint(pszText);
}

void CALLBACK EXPORT SaxBasicEditDebugTraceProc(HWND hWnd, short nType,
												LPCSTR pszText)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	pThis->DebugTrace(nType,pszText);
}

void CALLBACK EXPORT SaxBasicEditDoEventsProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicDoEvents"));

	pThis->OnBasicDoEvents();
}

BOOL CALLBACK EXPORT SaxBasicEditHelpProc(HWND hWnd, LPCSTR pszKeyword)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicHelp"));
	SB_TRACE_PARM(pszKeyword);

	BOOL bResult = pThis->OnBasicHelp(pszKeyword);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CALLBACK EXPORT SaxBasicEditNotifyProc(HWND hWnd, short nNotify)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnBasicNotify"));
	SB_TRACE_PARMLITERAL(nNotify,GetNotifyLiteral);

	pThis->OnBasicNotify(nNotify);
}

HWND CALLBACK EXPORT SaxBasicEditWindowHandleProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicWindowHandle"));

	HWND hwndResult = pThis->OnBasicWindowHandle();
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(hwndResult);
	return hwndResult;
}

void CALLBACK EXPORT SaxBasicEditStatusBarProc(HWND hWnd, LPCSTR pszText,
											   BOOL bAlert)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicStatusbar"));
	SB_TRACE_PARM(pszText);
	SB_TRACE_PARM(bAlert);

	pThis->OnBasicStatusbar(pszText,bAlert);
}

BOOL CALLBACK EXPORT SaxBasicEditCloseOkProc(HWND hWnd, short iSheet,
											 BOOL* pbCancel)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnCloseSheet"));
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult = pThis->OnCloseSheet(iSheet,pbCancel);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(*pbCancel);
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CALLBACK EXPORT SaxBasicEditGetMacroCaptionProc(HWND hWnd,
													 LPSTR pszCaption, UINT cbLen)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnGetMacroCaption"));
	SB_TRACE_PARM(pszCaption);
	SB_TRACE_PARMI2(cbLen);

	pThis->OnGetMacroCaption(pszCaption,cbLen);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszCaption);
}

BOOL CALLBACK EXPORT SaxBasicEditGetMacroNameProc(HWND hWnd, BOOL bOpen,
												  LPSTR pszName, UINT cbLen)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_QUERY_CALLBACK(_T("OnGetMacroName"));
	SB_TRACE_PARM(bOpen);
	SB_TRACE_PARMI2(cbLen);

	BOOL bResult = pThis->OnGetMacroName(bOpen,pszName,cbLen);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(bResult);
	return bResult;
}

UINT CALLBACK EXPORT SaxBasicEditReadMacroProc(HWND hWnd, LPCSTR pszName,
											   LPSTR pszCode)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnReadMacro"));
	SB_TRACE_PARM(pszName);

	UINT lResult = pThis->OnReadMacro(pszName,pszCode);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszCode);
	SB_TRACE_PARMI2(lResult);
	return lResult;
}

BOOL CALLBACK EXPORT SaxBasicEditWriteMacroProc(HWND hWnd, LPCSTR pszName,
												LPCSTR pszCode)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnWriteMacro"));
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(pszCode);

	BOOL bResult = pThis->OnWriteMacro(pszName,pszCode);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CALLBACK EXPORT SaxBasicEditOverrideMenuCommandProc(HWND hWnd, short cmd)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	BOOL bResult;
	{
		SB_TRACE_ACTION_CALLBACK(_T("OnOverrideMenuCommand"));
		SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

		bResult = pThis->OnOverrideMenuCommand((CSaxBasic::CmdNum)cmd);
		SB_TRACE_OUTPUT;
		SB_TRACE_PARM(bResult);
	}

	if (!bResult)
	{
		pThis->ExecuteMenuCommand((CSaxBasic::CmdNum)cmd);
		bResult = TRUE;
	}

	return bResult;
}

BOOL CALLBACK EXPORT SaxBasicEditParsePropertiesProc(HWND hWnd, LPCSTR pszName,
													 LPCSTR pszProperties)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnParseProperties"));
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(pszProperties);

	BOOL bResult = pThis->OnParseProperties(pszName,pszProperties);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

LPDISPATCH CALLBACK EXPORT SaxBasicEditNewInstanceProc(HWND hWnd, LPCSTR pszName)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnNewInstance"));
	SB_TRACE_PARM(pszName);

	LPDISPATCH lpResult = pThis->OnNewInstance(pszName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lpResult);
	return lpResult;
}

void CALLBACK EXPORT SaxBasicEditUpdatePropertiesProc(HWND hWnd, LPCSTR pszName,
													  BSTR* pbstrProperties)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnUpdateProperties"));
	SB_TRACE_PARM(pszName);

	CString sProperties = *pbstrProperties;
	pThis->OnUpdateProperties(pszName,sProperties);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sProperties);
	SysFreeString(*pbstrProperties);
	*pbstrProperties = sProperties.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit construction/destruction

CSaxBasicEdit::CSaxBasicEdit()
{
}

BOOL CSaxBasicEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// Turn off scroll bars, because the Basic Editor already uses them...
	dwStyle &= ~(WS_VSCROLL | WS_HSCROLL | SAXBASIC_PROJECT);
	if (!CWnd::Create(_T(SB_BASICEDITORCLASS), NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	AttachWindow(this);
	SetWindowLong(m_hWnd,SB_GWL_USER,(LONG)this);

	SendMessage(SB_MCM_SETABOUTBOXPROC, 0, (LPARAM)SaxBasicEditAboutBoxProc);
	SendMessage(SB_MCM_SETHELPPROC, 0, (LPARAM)SaxBasicEditHelpProc);
	SendMessage(SB_MCM_SETNOTIFYPROC, 0, (LPARAM)SaxBasicEditNotifyProc);
	SendMessage(SB_MCM_SETDOEVENTSPROC, 0, (LPARAM)SaxBasicEditDoEventsProc);
	SendMessage(SB_MCM_SETDEBUGPRINTPROC, 0, (LPARAM)SaxBasicEditDebugPrintProc);
	SendMessage(SB_MCM_SETWINDOWHANDLEPROC, 0, (LPARAM)SaxBasicEditWindowHandleProc);
	SendMessage(SB_MCM_SETSTATUSBARPROC, 0, (LPARAM)SaxBasicEditStatusBarProc);
	SendMessage(SB_MCM_SETCLOSEOKPROC, 0, (LPARAM)SaxBasicEditCloseOkProc);
	SendMessage(SB_MCM_SETGETMACROCAPTIONPROC, 0, (LPARAM)SaxBasicEditGetMacroCaptionProc);
	SendMessage(SB_MCM_SETGETMACRONAMEPROC, 0, (LPARAM)SaxBasicEditGetMacroNameProc);
	SendMessage(SB_MCM_SETREADMACROPROC, 0, (LPARAM)SaxBasicEditReadMacroProc);
	SendMessage(SB_MCM_SETWRITEMACROPROC, 0, (LPARAM)SaxBasicEditWriteMacroProc);
	SendMessage(SB_MCM_SETOVERRIDEMENUCOMMANDPROC, 0, (LPARAM)SaxBasicEditOverrideMenuCommandProc);
	SendMessage(SB_MCM_SETPARSEPROPERTIESPROC, 0, (LPARAM)SaxBasicEditParsePropertiesProc);
	SendMessage(SB_MCM_SETNEWINSTANCEPROC, 0, (LPARAM)SaxBasicEditNewInstanceProc);
	SendMessage(SB_MCM_SETUPDATEPROPERTIESPROC, 0, (LPARAM)SaxBasicEditUpdatePropertiesProc);
	SendMessage(SB_MCM_SETDEBUGTRACEPROC, 0, (LPARAM)SaxBasicEditDebugTraceProc);

	Initialize();
	return TRUE;
}

CSaxBasicEdit::~CSaxBasicEdit()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit diagnostics

#ifdef _DEBUG
void CSaxBasicEdit::AssertValid() const
{
	CEdit::AssertValid();
}

void CSaxBasicEdit::Dump(CDumpContext& dc) const
{
	CEdit::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit::GetSuperWndProcAddr - Provide storage for window proc

WNDPROC* CSaxBasicEdit::GetSuperWndProcAddr(void)
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}


BEGIN_MESSAGE_MAP(CSaxBasicEdit, CEdit)
	//{{AFX_MSG_MAP(CSaxBasicEdit)
	ON_WM_DESTROY()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicEdit message handlers

BOOL CSaxBasicEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CEdit::PreCreateWindow(cs))
		return FALSE;

	m_pInstance = new CSaxBasicInstance(this);

	return TRUE;
}

void CSaxBasicEdit::PostNcDestroy()
{
	m_pInstance->Detach();
	m_pInstance = NULL;

	CEdit::PostNcDestroy();
}

void CSaxBasicEdit::OnDestroy() 
{
	DetachWindow();
	CEdit::OnDestroy();
}

UINT CSaxBasicEdit::OnGetDlgCode() 
{
	UINT uDlgCode = CEdit::OnGetDlgCode();
	uDlgCode &= ~DLGC_HASSETSEL;
	return uDlgCode | DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTCHARS |
					  DLGC_WANTMESSAGE | DLGC_WANTTAB;
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView

IMPLEMENT_DYNCREATE(CSaxBasicView, CEditView)

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView call-back procs

void CALLBACK EXPORT SaxBasicViewAboutBoxProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicAboutBox"));

	pThis->OnBasicAboutBox();
}

void CALLBACK EXPORT SaxBasicViewDebugPrintProc(HWND hWnd, LPCSTR pszText)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicDebugPrint"));
	SB_TRACE_PARM(pszText);

	pThis->OnBasicDebugPrint(pszText);
}

void CALLBACK EXPORT SaxBasicViewDebugTraceProc(HWND hWnd, short nType,
												LPCSTR pszText)
{
	CALLBACK_PROLOG(CSaxBasicView);

	pThis->DebugTrace(nType,pszText);
}

void CALLBACK EXPORT SaxBasicViewDoEventsProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicDoEvents"));

	pThis->OnBasicDoEvents();
}

BOOL CALLBACK EXPORT SaxBasicViewHelpProc(HWND hWnd, LPCSTR pszKeyword)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicHelp"));
	SB_TRACE_PARM(pszKeyword);

	BOOL bResult = pThis->OnBasicHelp(pszKeyword);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CALLBACK EXPORT SaxBasicViewNotifyProc(HWND hWnd, short nNotify)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnBasicNotify"));
	SB_TRACE_PARMLITERAL(nNotify,GetNotifyLiteral);

	pThis->OnBasicNotify(nNotify);
}

HWND CALLBACK EXPORT SaxBasicViewWindowHandleProc(HWND hWnd)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicWindowHandle"));

	HWND hwndResult = pThis->OnBasicWindowHandle();
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(hwndResult);
	return hwndResult;
}

void CALLBACK EXPORT SaxBasicViewStatusBarProc(HWND hWnd, LPCSTR pszText,
											   BOOL bAlert)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnBasicStatusbar"));
	SB_TRACE_PARM(pszText);
	SB_TRACE_PARM(bAlert);

	pThis->OnBasicStatusbar(pszText,bAlert);
}

BOOL CALLBACK EXPORT SaxBasicViewCloseOkProc(HWND hWnd, short iSheet,
											 BOOL* pbCancel)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnCloseSheet"));
	SB_TRACE_PARMI2(iSheet);

	BOOL bResult = pThis->OnCloseSheet(iSheet,pbCancel);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(*pbCancel);
	SB_TRACE_PARM(bResult);
	return bResult;
}

void CALLBACK EXPORT SaxBasicViewGetMacroCaptionProc(HWND hWnd,
													 LPSTR pszCaption, UINT cbLen)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnGetMacroCaption"));
	SB_TRACE_PARM(pszCaption);
	SB_TRACE_PARMI2(cbLen);

	pThis->OnGetMacroCaption(pszCaption,cbLen);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszCaption);
}

BOOL CALLBACK EXPORT SaxBasicViewGetMacroNameProc(HWND hWnd, BOOL bOpen,
												  LPSTR pszName, UINT cbLen)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_QUERY_CALLBACK(_T("OnGetMacroName"));
	SB_TRACE_PARM(bOpen);
	SB_TRACE_PARMI2(cbLen);

	BOOL bResult = pThis->OnGetMacroName(bOpen,pszName,cbLen);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(bResult);
	return bResult;
}

UINT CALLBACK EXPORT SaxBasicViewReadMacroProc(HWND hWnd, LPCSTR pszName,
											   LPSTR pszCode)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnReadMacro"));
	SB_TRACE_PARM(pszName);

	UINT lResult = pThis->OnReadMacro(pszName,pszCode);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(pszCode);
	SB_TRACE_PARMI2(lResult);
	return lResult;
}

BOOL CALLBACK EXPORT SaxBasicViewWriteMacroProc(HWND hWnd, LPCSTR pszName,
												LPCSTR pszCode)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnWriteMacro"));
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(pszCode);

	BOOL bResult = pThis->OnWriteMacro(pszName,pszCode);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

BOOL CALLBACK EXPORT SaxBasicViewOverrideMenuCommandProc(HWND hWnd, short cmd)
{
	CALLBACK_PROLOG(CSaxBasicView);

	BOOL bResult;
	{
		SB_TRACE_ACTION_CALLBACK(_T("OnOverrideMenuCommand"));
		SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

		bResult = pThis->OnOverrideMenuCommand((CSaxBasic::CmdNum)cmd);
		SB_TRACE_OUTPUT;
		SB_TRACE_PARM(bResult);
	}

	if (!bResult)
	{
		pThis->ExecuteMenuCommand((CSaxBasic::CmdNum)cmd);
		bResult = TRUE;
	}

	return bResult;
}

BOOL CALLBACK EXPORT SaxBasicViewParsePropertiesProc(HWND hWnd, LPCSTR pszName,
													 LPCSTR pszProperties)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnParseProperties"));
	SB_TRACE_PARM(pszName);
	SB_TRACE_PARM(pszProperties);

	BOOL bResult = pThis->OnParseProperties(pszName,pszProperties);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}

LPDISPATCH CALLBACK EXPORT SaxBasicViewNewInstanceProc(HWND hWnd, LPCSTR pszName)
{
	CALLBACK_PROLOG(CSaxBasicView);

	SB_TRACE_ACTION_CALLBACK(_T("OnNewInstance"));
	SB_TRACE_PARM(pszName);

	LPDISPATCH lpResult = pThis->OnNewInstance(pszName);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(lpResult);
	return lpResult;
}

void CALLBACK EXPORT SaxBasicViewUpdatePropertiesProc(HWND hWnd, LPCSTR pszName,
													  BSTR* pbstrProperties)
{
	CALLBACK_PROLOG(CSaxBasicEdit);

	SB_TRACE_ACTION_CALLBACK(_T("OnUpdateProperties"));
	SB_TRACE_PARM(pszName);

	CString sProperties = *pbstrProperties;
	pThis->OnUpdateProperties(pszName,sProperties);
	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(sProperties);
	SysFreeString(*pbstrProperties);
	*pbstrProperties = sProperties.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView construction/destruction

CSaxBasicView::CSaxBasicView()
{
	m_nTabStops = 4*4; // use a default of 4 chars
}

CSaxBasicView::~CSaxBasicView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView overrides

BOOL CSaxBasicView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CEditView::PreCreateWindow(cs))
		return FALSE;

	m_pInstance = new CSaxBasicInstance(this);

	cs.lpszClass = _T(SB_BASICEDITORCLASS);
	// Turn off scroll bars, because the Basic Editor already uses them...
	cs.style &= ~(WS_VSCROLL | WS_HSCROLL | 0xffff);
	// Turn on user file io by default
	cs.style |= SAXBASIC_USERFILEIO | SAXBASIC_NOPOPUPMENU;
	// MFC needs the border to visible (7/12/98)
	cs.style |= WS_BORDER;
	return TRUE;
}

void CSaxBasicView::PostNcDestroy() 
{
	m_pInstance->Detach();
	m_pInstance = NULL;

	CEditView::PostNcDestroy();
}

void CSaxBasicView::OnDraw(CDC* pDC)
{	
}

BOOL CSaxBasicView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CSaxBasicView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing.
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CSaxBasicView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

void CSaxBasicView::Serialize(CArchive& ar)
{
	// Default CEditView serializing
	CEditView::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView diagnostics

#ifdef _DEBUG
void CSaxBasicView::AssertValid() const
{
	CEditView::AssertValid();
}

void CSaxBasicView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView::GetSuperWndProcAddr - Provide storage for window proc

WNDPROC* CSaxBasicView::GetSuperWndProcAddr(void)
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

BOOL CSaxBasicView::ExecuteMenuCommand(CmdNum cmd)
{
	SB_TRACE_ACTION(_T("ExecuteMenuCommand"));
	SB_TRACE_PARMLITERAL(cmd,GetCmdLiteral);

	ASSERT(this != NULL);
	if (this == NULL)
		return FALSE;

	ASSERT_VALID(m_pEdit);
	if (m_pEdit == NULL)
		return FALSE;

	BOOL bResult = TRUE;

	switch (cmd)
	{
	case cmdEditFind:
		CEditView::OnEditFind();
		break;
	case cmdEditReplace:
		CEditView::OnEditReplace();
		break;
	case cmdEditAgain:
		CEditView::OnEditRepeat();
		break;
	default:
		bResult =
			(BOOL)m_pEdit->SendMessage(SB_MCM_MENUCOMMAND, (WPARAM)cmd, 1l);
	}

	SB_TRACE_OUTPUT;
	SB_TRACE_PARM(bResult);
	return bResult;
}


static const UINT nMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CSaxBasicView, CEditView)
	//{{AFX_MSG_MAP(CSaxBasicView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrinting)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrinting)
	//}}AFX_MSG_MAP
	// Special registered message for Find and Replace
	ON_REGISTERED_MESSAGE(nMsgFindReplace, OnFindReplaceCmd)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicView message handlers

int CSaxBasicView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	AttachWindow(&GetEditCtrl());
	SetWindowLong(m_hWnd,SB_GWL_USER,(LONG)this);

	SendMessage(SB_MCM_SETABOUTBOXPROC, 0, (LPARAM)SaxBasicViewAboutBoxProc);
	SendMessage(SB_MCM_SETHELPPROC, 0, (LPARAM)SaxBasicViewHelpProc);
	SendMessage(SB_MCM_SETNOTIFYPROC, 0, (LPARAM)SaxBasicViewNotifyProc);
	SendMessage(SB_MCM_SETDOEVENTSPROC, 0, (LPARAM)SaxBasicViewDoEventsProc);
	SendMessage(SB_MCM_SETDEBUGPRINTPROC, 0, (LPARAM)SaxBasicViewDebugPrintProc);
	SendMessage(SB_MCM_SETWINDOWHANDLEPROC, 0, (LPARAM)SaxBasicViewWindowHandleProc);
	SendMessage(SB_MCM_SETSTATUSBARPROC, 0, (LPARAM)SaxBasicViewStatusBarProc);
	SendMessage(SB_MCM_SETCLOSEOKPROC, 0, (LPARAM)SaxBasicViewCloseOkProc);
	SendMessage(SB_MCM_SETGETMACROCAPTIONPROC, 0, (LPARAM)SaxBasicViewGetMacroCaptionProc);
	SendMessage(SB_MCM_SETGETMACRONAMEPROC, 0, (LPARAM)SaxBasicViewGetMacroNameProc);
	SendMessage(SB_MCM_SETREADMACROPROC, 0, (LPARAM)SaxBasicViewReadMacroProc);
	SendMessage(SB_MCM_SETWRITEMACROPROC, 0, (LPARAM)SaxBasicViewWriteMacroProc);
	SendMessage(SB_MCM_SETOVERRIDEMENUCOMMANDPROC, 0, (LPARAM)SaxBasicViewOverrideMenuCommandProc);
	SendMessage(SB_MCM_SETPARSEPROPERTIESPROC, 0, (LPARAM)SaxBasicViewParsePropertiesProc);
	SendMessage(SB_MCM_SETNEWINSTANCEPROC, 0, (LPARAM)SaxBasicViewNewInstanceProc);
	SendMessage(SB_MCM_SETUPDATEPROPERTIESPROC, 0, (LPARAM)SaxBasicViewUpdatePropertiesProc);
	SendMessage(SB_MCM_SETDEBUGTRACEPROC, 0, (LPARAM)SaxBasicViewDebugTraceProc);

	Initialize();
	return 0;
}

void CSaxBasicView::OnDestroy() 
{
	DetachWindow();
	CEditView::OnDestroy();
}

void CSaxBasicView::OnUpdateFilePrinting(CCmdUI* pCmdUI) 
{
}

LRESULT CSaxBasicView::OnFindReplaceCmd(WPARAM wParam, LPARAM lParam)
{
	return CEditView::OnFindReplaceCmd(wParam,lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CSaxBasicDocTemplate

CSaxBasicDocTemplate::CSaxBasicDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
	CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass),
		m_bLoaded(FALSE)
{
}

CSaxBasicDocTemplate::~CSaxBasicDocTemplate()
{
	if (m_bLoaded)
		GetBasicLoader()->Free();
}

CDocument* CSaxBasicDocTemplate::CreateNewDocument()
{
	if (!m_bLoaded)
	{
		m_bLoaded = TRUE;
		GetBasicLoader()->Load();
	}

	return CMultiDocTemplate::CreateNewDocument();
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

CSaxBasic* CSaxBasicDoc::FindBasic()
{
	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	if (pView == NULL)
		return NULL;

	// assumes one view per document
	CFrameWnd* pFrame = pView->GetParentFrame();
	if (pFrame == NULL)
		return NULL;

	// assumes one CSaxBasic instance per document
	return CSaxBasic::FindBasic(pFrame);
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
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFileNew);

	return TRUE;
}

BOOL CSaxBasicDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic && pBasic->LoadFilename(lpszPathName))
		pBasic->SetActiveSheet(pBasic->FindSheet(lpszPathName));

	return TRUE;
}

void CSaxBasicDoc::OnCloseDocument()
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		// remove all extensions and hiddencode by releasing the Basic CodeUnit
		// the edit window is not destroyed
		VERIFY(pBasic->Disconnect());

	CDocument::OnCloseDocument();
}

BOOL CSaxBasicDoc::SaveModified()
{
	// modifications are stored by controlling application or IDE
	return TRUE;
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
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFileClose);
}

void CSaxBasicDoc::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	pCmdUI->Enable(pBasic &&
		pBasic->IsMenuCommandEnabled(CSaxBasic::cmdFileClose));
}

void CSaxBasicDoc::OnFileSave() 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFileSave);
}

void CSaxBasicDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	pCmdUI->Enable(pBasic &&
		pBasic->IsMenuCommandEnabled(CSaxBasic::cmdFileSave));
}

void CSaxBasicDoc::OnFileSaveAs() 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFileSaveAs);
}

void CSaxBasicDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	pCmdUI->Enable(pBasic &&
		pBasic->IsMenuCommandEnabled(CSaxBasic::cmdFileSaveAs));
}

void CSaxBasicDoc::OnFilePrint() 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFilePrint);
}

void CSaxBasicDoc::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	pCmdUI->Enable(pBasic &&
		pBasic->IsMenuCommandEnabled(CSaxBasic::cmdFilePrint));
}

void CSaxBasicDoc::OnFilePrintSetup() 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	if (pBasic)
		pBasic->ExecuteMenuCommand(CSaxBasic::cmdFilePrintSetup);
}

void CSaxBasicDoc::OnUpdateFilePrintSetup(CCmdUI* pCmdUI) 
{
	CSaxBasic* pBasic = FindBasic();
	ASSERT(pBasic != NULL);
	pCmdUI->Enable(pBasic &&
		pBasic->IsMenuCommandEnabled(CSaxBasic::cmdFilePrintSetup));
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

CSaxBasic* CSaxBasicFrame::FindBasic()
{
	return CSaxBasic::FindBasic(this);
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
	switch (CSaxBasic::ClosingFrame(this))
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
	switch (CSaxBasic::ClosingFrame(this))
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
		PostMessage(WM_CLOSE);

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

CSaxBasic* CSaxBasicMDIFrame::FindBasic()
{
	return CSaxBasic::FindBasic(this);
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
	switch (CSaxBasic::ClosingFrame(this))
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
	
	switch (CSaxBasic::ClosingFrame(this))
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
		PostMessage(WM_CLOSE);

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

CSaxBasic* CSaxBasicMDIChild::FindBasic()
{
	return CSaxBasic::FindBasic(this);
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
	switch (CSaxBasic::ClosingFrame(this))
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
		PostMessage(WM_CLOSE);

	KillTimer(nIDEvent);
}
