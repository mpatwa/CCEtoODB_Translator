

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Sep 26 17:29:40 2012
 */
/* Compiler settings for .\CC.odl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Camcad_h_h__
#define __Camcad_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICAMCAD_FWD_DEFINED__
#define __ICAMCAD_FWD_DEFINED__
typedef interface ICAMCAD ICAMCAD;
#endif 	/* __ICAMCAD_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


#ifndef __IAPI_FWD_DEFINED__
#define __IAPI_FWD_DEFINED__
typedef interface IAPI IAPI;
#endif 	/* __IAPI_FWD_DEFINED__ */


#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__

#ifdef __cplusplus
typedef class Application Application;
#else
typedef struct Application Application;
#endif /* __cplusplus */

#endif 	/* __Application_FWD_DEFINED__ */


#ifndef __IDFM_FWD_DEFINED__
#define __IDFM_FWD_DEFINED__
typedef interface IDFM IDFM;
#endif 	/* __IDFM_FWD_DEFINED__ */


#ifndef __DFM_FWD_DEFINED__
#define __DFM_FWD_DEFINED__

#ifdef __cplusplus
typedef class DFM DFM;
#else
typedef struct DFM DFM;
#endif /* __cplusplus */

#endif 	/* __DFM_FWD_DEFINED__ */


#ifndef __IDFTFacade_FWD_DEFINED__
#define __IDFTFacade_FWD_DEFINED__
typedef interface IDFTFacade IDFTFacade;
#endif 	/* __IDFTFacade_FWD_DEFINED__ */


#ifndef __DFTFacade_FWD_DEFINED__
#define __DFTFacade_FWD_DEFINED__

#ifdef __cplusplus
typedef class DFTFacade DFTFacade;
#else
typedef struct DFTFacade DFTFacade;
#endif /* __cplusplus */

#endif 	/* __DFTFacade_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CAMCAD_LIBRARY_DEFINED__
#define __CAMCAD_LIBRARY_DEFINED__

/* library CAMCAD */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_CAMCAD,0xF8DE4B05,0xFBC2,0x11D1,0xBA,0x40,0x00,0x80,0xAD,0xB3,0x6D,0xBB);

#ifndef __ICAMCAD_DISPINTERFACE_DEFINED__
#define __ICAMCAD_DISPINTERFACE_DEFINED__

/* dispinterface ICAMCAD */
/* [uuid] */ 


DEFINE_GUID(DIID_ICAMCAD,0xF8DE4B06,0xFBC2,0x11D1,0xBA,0x40,0x00,0x80,0xAD,0xB3,0x6D,0xBB);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F8DE4B06-FBC2-11D1-BA40-0080ADB36DBB")
    ICAMCAD : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICAMCADVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICAMCAD * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICAMCAD * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICAMCAD * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICAMCAD * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICAMCAD * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICAMCAD * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICAMCAD * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICAMCADVtbl;

    interface ICAMCAD
    {
        CONST_VTBL struct ICAMCADVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICAMCAD_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICAMCAD_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICAMCAD_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICAMCAD_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICAMCAD_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICAMCAD_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICAMCAD_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICAMCAD_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Document,0x5BECA79B,0xEF3B,0x11D1,0xBA,0x40,0x00,0x80,0xAD,0xB3,0x6D,0xBB);

#ifdef __cplusplus

class DECLSPEC_UUID("5BECA79B-EF3B-11D1-BA40-0080ADB36DBB")
Document;
#endif

#ifndef __IAPI_DISPINTERFACE_DEFINED__
#define __IAPI_DISPINTERFACE_DEFINED__

/* dispinterface IAPI */
/* [uuid] */ 


DEFINE_GUID(DIID_IAPI,0xC3DB3A2F,0x0B71,0x11D2,0xBA,0x40,0x00,0x80,0xAD,0xB3,0x6D,0xBB);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C3DB3A2F-0B71-11D2-BA40-0080ADB36DBB")
    IAPI : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAPIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAPI * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAPI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAPI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAPI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAPI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAPI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAPI * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAPIVtbl;

    interface IAPI
    {
        CONST_VTBL struct IAPIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAPI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAPI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAPI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAPI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAPI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAPI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAPI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAPI_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Application,0xC3DB3A2D,0x0B71,0x11D2,0xBA,0x40,0x00,0x80,0xAD,0xB3,0x6D,0xBB);

#ifdef __cplusplus

class DECLSPEC_UUID("C3DB3A2D-0B71-11D2-BA40-0080ADB36DBB")
Application;
#endif

#ifndef __IDFM_DISPINTERFACE_DEFINED__
#define __IDFM_DISPINTERFACE_DEFINED__

/* dispinterface IDFM */
/* [uuid] */ 


DEFINE_GUID(DIID_IDFM,0xDAD9B785,0xA34F,0x4786,0x81,0x11,0x9E,0x6B,0x9D,0xDA,0x6D,0xC1);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DAD9B785-A34F-4786-8111-9E6B9DDA6DC1")
    IDFM : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDFMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDFM * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDFM * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDFM * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDFM * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDFM * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDFM * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDFM * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDFMVtbl;

    interface IDFM
    {
        CONST_VTBL struct IDFMVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDFM_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDFM_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDFM_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDFM_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDFM_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDFM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDFM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDFM_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DFM,0x02CAD7F0,0xA311,0x4157,0xAC,0xFC,0xA6,0x66,0x6C,0x55,0xFF,0x19);

#ifdef __cplusplus

class DECLSPEC_UUID("02CAD7F0-A311-4157-ACFC-A6666C55FF19")
DFM;
#endif

#ifndef __IDFTFacade_DISPINTERFACE_DEFINED__
#define __IDFTFacade_DISPINTERFACE_DEFINED__

/* dispinterface IDFTFacade */
/* [uuid] */ 


DEFINE_GUID(DIID_IDFTFacade,0xD3B6EDD0,0xEE9D,0x4978,0x9C,0x20,0xE1,0x2D,0x2B,0xB7,0x62,0xEB);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D3B6EDD0-EE9D-4978-9C20-E12D2BB762EB")
    IDFTFacade : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDFTFacadeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDFTFacade * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDFTFacade * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDFTFacade * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDFTFacade * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDFTFacade * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDFTFacade * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDFTFacade * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDFTFacadeVtbl;

    interface IDFTFacade
    {
        CONST_VTBL struct IDFTFacadeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDFTFacade_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDFTFacade_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDFTFacade_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDFTFacade_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDFTFacade_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDFTFacade_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDFTFacade_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDFTFacade_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_DFTFacade,0x6AE201D4,0x8B7D,0x49c4,0x95,0x5D,0x45,0x0A,0x56,0x4B,0x26,0xBE);

#ifdef __cplusplus

class DECLSPEC_UUID("6AE201D4-8B7D-49c4-955D-450A564B26BE")
DFTFacade;
#endif
#endif /* __CAMCAD_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


