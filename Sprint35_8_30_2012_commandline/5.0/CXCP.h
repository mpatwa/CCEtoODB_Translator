// CXCP.h : Declaration of the CCXCP

/***  USAGE:
	xcp.SetMethod("Func1");
	try{
		XCP_TRY_HR( Func2(), eXCP_THROW);

	}catch(CXCP *pX){
		pX->DoCatch();
		return pX->GetHRESULT();
	}catch(_com_error e){
		return xcp.DoCOMCatch(e);
	}
 ***/

#pragma once
#include "resource.h"       // main symbols
#include <comdef.h>

#define XCP_TRY_HR(expr,opt_eMode) \
	xcp.tryHR( expr, #expr, __LINE__ ); \
	ATLASSERT( !xcp.IsError(#expr) );\
	xcp.OnError(opt_eMode)
	
#define XCP_TRY_NULL(expr,opt_eMode) \
	xcp.tryNULL( expr, #expr, __LINE__); \
	ATLASSERT( !xcp.IsError(#expr) );\
	xcp.OnError(opt_eMode)
	

typedef enum
{
	eXCP_NOTHING		= 0,
	eXCP_TRACE		    = 1,
	eXCP_THROW			= 2,
	eXCP_ERRINFO		= 4,
	eXCP_ALL			= 0xF
}eXCPModes;

typedef enum
{
	eXCPCM_NOTHING		= 0,
	eXCP_LOG			= 2,
	eXCPCM_RETHROW		= 4,
	eXCPCM_ALL			= 0xF
}eXCPCatchModes;

// CCXCP

class CXCP
{
	bool bErrorSet;		//true indicates the class is in use.
	bool bReThrown;		//indicates the class was rethrown
	bool bCaughtOnce;

	CString className;
	CString methodName;
	CString contextErrorMessage;	//Error message defined at try... call
	CString trappedErrorMessage;	//Trapped error message (e.g. _com_error)
	CString ErrorMessageXML;		//CXCP formatted error message (in XML)

	CString errorTimestamp;			//time error occurred
	HRESULT hr;						//HRESULT for COM error

public:
	CXCP() { this->ResetCXCP(); }

	//Each class should call this as part of its initialization
	void SetClass(CString className) { this->className = className; }

	//Each method should call this on entry
	void SetMethod(CString methodName) 
	{ 
		this->ResetCXCP();
		this->methodName = methodName;
	}
	//Resets the class - primarily the bErrorSet flag
	void ResetCXCP()
	{
		bErrorSet = false;
		bReThrown = false;
		bCaughtOnce = false;
		hr= S_OK;
	}

	/// The following methods may be used in the try block

	//Call this method for functions returning HRESULTS (i.e. COM calls)
	void tryHR(HRESULT HResult, CString msg, long lineNo, eXCPModes eMode = eXCP_ALL)
	{
		this->hr = HResult;

		bool bOk = SUCCEEDED( HResult);
		if (!bOk)
		{
			ProcessCOMError( _com_error(HResult));
			XCPProcess( msg, lineNo, eMode);
		}
	}
	/*
		Call this method for all other errors.  Has two uses:
		1.  functions returning NULL for error conditions
		2.  Handles logical expressions.  For example:
		      xcp.tryNULL( 1 != myFunc(), ...);
	 */
	void tryNULL(long NullVal, CString  msg, long lineNo, eXCPModes eMode = eXCP_ALL)
	{
		if (!NullVal)
			 XCPProcess(msg, lineNo, eMode);
	}

	///  The following methods may be used in the catch block

	//Generic catch (CXCP pX) handler
	long DoCatch(eXCPCatchModes eMode = eXCPCM_NOTHING)
	{
		this->bCaughtOnce = true;

		if (eXCPCM_RETHROW & eMode)
		{
			this->bReThrown = true;		
			throw;
		}

		return NULL;
	}
	// catch( _com_error e) handler
	HRESULT DoCOMCatch(_com_error e, eXCPCatchModes eMode = eXCPCM_NOTHING)
	{
		this->ProcessCOMError(e);
		this->XCPProcess( "", 0, eXCP_TRACE);
		this->DoCatch(eMode);

		return e.Error();
	}
	void OnError(eXCPModes eMode = eXCP_ALL)
	{
		if (bErrorSet)
		{
			if (eXCP_THROW & eMode)	//should be last
					throw this;
		}
	}

private:

	void XCPProcess(CString  msg, long lineNo, eXCPModes eMode)
	{
		this->bErrorSet = true;
		this->FormatErrorMsgXML(msg, lineNo);
		if (eXCP_TRACE & eMode)
			ATLTRACE2( ErrorMessageXML);
	}
	void ProcessCOMError( _com_error e)
	{
		trappedErrorMessage = "<TrappedError type= \"HRESULT\"";

		//get com error
		trappedErrorMessage += " comError = \"";
		trappedErrorMessage += e.ErrorMessage();
		trappedErrorMessage += "\" code = \"";
		trappedErrorMessage.AppendFormat("%d", e.WCode() );
			

		//get error info
		IErrorInfo *pEI;
		pEI = e.ErrorInfo();
		if (pEI)
		{
			trappedErrorMessage += "\" ErrorInfo = \"";

			CComBSTR bstrError;
			pEI->GetDescription( &bstrError);
			trappedErrorMessage += bstrError;

			pEI->Release();
		}
		trappedErrorMessage += "\"/>";
	}

	void FormatErrorMsgXML( CString msg, long lineNo)
	{
		ErrorMessageXML = "<Context";
		if (bReThrown)
		{
			ErrorMessageXML += " reThrown = \"true\" ";
		}
		if (bCaughtOnce)
		{
			ErrorMessageXML += " CXCPreCaught = \"true\" ";
		}
		ErrorMessageXML += " class= \"";
		ErrorMessageXML += methodName;
		ErrorMessageXML += "\" line= \"";
		ErrorMessageXML.AppendFormat("%d", lineNo);


		errorTimestamp = __TIMESTAMP__;
		ErrorMessageXML += "\" time= \"";
		ErrorMessageXML += errorTimestamp;

		ErrorMessageXML += "\"/>";

		ErrorMessageXML += "<ContextError text= \"";
		ErrorMessageXML += msg;
		ErrorMessageXML += "\"/>";

		ErrorMessageXML += trappedErrorMessage;
	}

public:
	bool IsSFALSE() { return bErrorSet ? S_FALSE == hr : S_OK;	}
	bool IsError( char *text) { return bErrorSet;}
	CString GetErrorMessage() { return bErrorSet ? ErrorMessageXML : ""; }
	HRESULT GetHRESULT() { return bErrorSet ? this->hr : S_OK; }
};

