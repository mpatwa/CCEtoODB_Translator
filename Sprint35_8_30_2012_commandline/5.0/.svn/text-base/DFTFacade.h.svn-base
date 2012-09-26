// $Header: /CAMCAD/4.6/DFTFacade.h 24    4/03/07 5:46p Lynn Phung $

// uncomment the following to allow CamCad to be built without the DFT module - knv
#define DisableDftModule

#if !defined(DisableDftModule)

#pragma once

#include "ccdoc.h"
#include "..\DFT\common\CXCP.h"
#include "DFTTester.h"
//#import "..\DFT\DFT.tlb" no_namespace
#import "..\DFT\DFT.tlb" rename_namespace("DFT")

class CDFTTester;

// CDFTFacade

class CDFTFacade : public CWnd
{
   DECLARE_DYNCREATE(CDFTFacade)

   CXCP     xcp;     //exception class declared in CXCP.h

   CCEtoODBDoc *pCCDoc;     //passed to the constructor
   DFT::IDFTMainPtr pDFT;     //smart pointer type declared in the typelib
   
   CDFTTester *pdlgDFTTester;
   _bstr_t pbstrDFTData;

public:
   CDFTFacade( CCEtoODBDoc *pCCDoc);
   virtual ~CDFTFacade();

   virtual void OnFinalRelease(); 

protected:
   DECLARE_MESSAGE_MAP()
   DECLARE_OLECREATE(CDFTFacade)
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
public:
   /// CAMCAD 2 DFT Handlers
   afx_msg void OnAdvancedValidatepcb();
   afx_msg void OnUpdateAdvancedValidatepcb(CCmdUI *pCmdUI);
   afx_msg void OnAdvancedDftoptions();
   afx_msg void OnUpdateAdvancedDftoptions(CCmdUI *pCmdUI);


   HRESULT ConnectDFT()
   {
      if ( NULL != pDFT) return S_FALSE;  //one time only

      HRESULT hr;
      XCP_TRY{
         hr = pDFT.CreateInstance(__uuidof(DFT::CDFTMain));
         if (FAILED(hr) ) 
         {
            pDFT = NULL;
            ::MessageBox( NULL, 
               "The requested function requires DFT - \nwhich is NOT available.\nPlease go to http://www.mentor.com/supportnet for technical support.", "DFT Facade", 
               MB_OK);
            return hr;
         }
         XCP_TRY_HR( pDFT->SetDFTFacade( this->GetIDispatch(false) ) );
      }XCP_HR_CATCH
      return S_OK;
   }
   /*  CacheDFTData
      CAMCAD calls this to pass the <DFT Assembly> node from the CC file.
      The DFT Facade will cache this data locally until the user invokes DFT.
    */
   void CacheDFTData( const char *pszDFTData)
   {
      XCP_TRY{
         CString DFTData( pszDFTData);
         this->pbstrDFTData.Attach( DFTData.AllocSysString() );

         //If DFT is already connected, pass the data along
         if ( !(pDFT == NULL) ) 
         {
            XCP_TRY_HR( pDFT->LoadDFTData( pbstrDFTData.GetAddress() ) );
         }
      }XCP_VOID_CATCH
   }
   void RetrieveDFTCache( CString &DFTData)
   {  
      XCP_TRY{
         //If DFT is already connected, retrieve the data from DFT
         if ( !(pDFT == NULL) ) 
         {
            DFTData = (const wchar_t *) pDFT->GetDFTData(); 
         }
         //else if we were sent DFT data, send it back
         else if ( 0 != pbstrDFTData.length() )
         {
            DFTData = (const wchar_t *) pbstrDFTData; 
         }
      }XCP_VOID_CATCH
   }


   //***  DFT Menu Callbacks
   afx_msg void OnDFTValidatePcb()
   {
      XCP_TRY{
         
         //If there is DFT data from a previous DFT session
         if ( 0 != pbstrDFTData.length() )
         {
            //Delegate request with previous DFT data
            if ( SUCCEEDED( this->ConnectDFT() ) )
            { 
               BSTR test = pbstrDFTData.copy(false);
               XCP_TRY_HR( pDFT->LoadDFTData( &test ) );
            }
         }

         // If there is no  CC file loaded into the document
//  Need something other than the filename - if they imported
         if ( pCCDoc->CCFileName.IsEmpty() )
            ::MessageBox( NULL, "Can not Validate, No CC File is loaded.", "Validate PCB", MB_OK);
         else
         {
            //Delegate request to DFT
            if ( SUCCEEDED( this->ConnectDFT() ) )
            { XCP_TRY_HR( pDFT->OnValidatePCB() ); }
         }
      }XCP_VOID_CATCH
   }
   afx_msg void OnDFTTestPlan()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnTestPlanReq() ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnDFTOptions()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnDFTOptions() ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnDFTExportNails()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnExportReq("Nails") ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnDFTImportTester()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnImportReq("Tester") ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnDFTFixtInserts()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnFixtInserts() ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnExportDrills()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnExportReq("Drills") ); }
      }XCP_VOID_CATCH
   }
   afx_msg void OnExportWire()
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { XCP_TRY_HR( pDFT->OnExportReq("Wire") ); }
      }XCP_VOID_CATCH
   }
   //***end menu callbacks


   void OnEditAttributes( CString ToolstackXPath)
   {
      XCP_TRY{
         //Delegate request to DFT
         if ( SUCCEEDED( this->ConnectDFT() ) )
         { 
            _bstr_t pbstrToolstack( ToolstackXPath.AllocSysString(), false);
            XCP_TRY_HR( pDFT->OnAttributeReq( pbstrToolstack.GetBSTR() ) );
         }
      }XCP_VOID_CATCH
   }
protected:
   BSTR GetCCFilePath(void);

   enum 
   {
      dispidInsertToolstacks2 = 17L,      dispidtrash = 16L,      dispidTrash = 16L,      
      dispidSetCompTB = 15L,     
      dispidCalcCompCenterXY = 14L,    
      dispidCalcPadLocation = 13L,     
      dispidSetPinTBX = 12L,     
      dispidCalcMinMetal = 11L,     
      dispidCalcPadSize = 10L,      
      dispidCalcSMaskClear = 9L,    
      dispidCalcComp2Pin = 8L,      
      dispidCalcBoard2Pin = 7L,     
      dispidAddToolstack = 6L,      
      dispidDeleteToolstack = 5L,      
      dispidEditToolstack = 4L,     
      dispidInsertToolstacks = 3L,     
      dispidRemoveToolstacks = 2L,     
      dispidGetCCFilePath = 1L
   };
protected:
   HRESULT RemoveToolstacks(void);
   HRESULT InsertToolstacks( LPCTSTR bstrToolstacksXML);
   HRESULT EditToolstack( LPCTSTR bstrToolstacksXML);
   HRESULT DeleteToolstack( LPCTSTR bstrToolstack);
   HRESULT AddToolstack( LPCTSTR bstrToolstack);
   HRESULT CalcBoard2Pin(long bOverwrite);
   HRESULT CalcComp2Pin(long bOverwrite);
   HRESULT CalcSMaskClear(long bOverwrite);
   HRESULT CalcPadSize(long bOverwrite);
   HRESULT CalcMinMetal(long bOverwrite);
   HRESULT SetPinTBX(long bOverwrite);
   HRESULT CalcPadLocation(long bOverwrite);
   HRESULT CalcCompCenterXY(long bOverwrite);
   HRESULT SetCompTB(long bOverwrite);
   HRESULT trash(LPCTSTR test);
   HRESULT InsertToolstacks2(BSTR* bstrToolstacks);
};

#endif
