// $Header: /CAMCAD/4.3/GerberPcb.h 12    1/15/04 1:43p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/  

#if ! defined(__GerberPcb_h__)
#define __GerberPcb_h__
 
#include <afxtempl.h>
#include "ccdoc.h"
#include "afxwin.h"
#include "CamCadDatabase.h"

//class CCamCadDatabase;

/* Struct Definition  ****************************************************************************/

// collects all pins from the netlist
typedef struct
{
   CString  netname; // this includes $UnUsed, so that we can check open pins.
   CString  comp, pin;
   double   x;
   double   y;
   long     entity;
   int      bnum;
   int      used;    // if they could attach to a FREEPAD
   int      layer;
}GPCB_comppinkoo; 
typedef CTypedPtrArray<CPtrArray, GPCB_comppinkoo*> CCompPinkooArray;

#define  GERBERPCBLogFile        "gpcb.log"
#define  MAX_GPCBLAYERS          256

typedef struct
{
   CString  stackname;
}GPCB_padname; 
typedef CTypedPtrArray<CPtrArray, GPCB_padname*> CPadnameArray;

typedef struct
{
   DataStruct     *datastruct;
   BlockStruct    *blockstruct;
   CompPinStruct  *comppin;
}GPCB_padstack_convert; 
typedef CTypedPtrArray<CPtrArray, GPCB_padstack_convert*> CPadstackConvertArray;

// get aperture data as found in Gerber. This array is needed to 
// create padstacks from apertures.
typedef struct
{
   double      x;
   double      y;
   int         layer;   
   int         bnum;
   DataStruct  *data;         // entity pointer
   int         curid;         // temp padstack marker
   int         padstackid;
   int         drill;         // is there a drill hole, this saves the index+1 into the drillkooarray
   char        masks;         // 1 = top, 2 = bottom, 3 = all for Soldermask
                              // 4 = top, 8 = bottom, 12 = all for pastemask
   char        topbottom;     // 1 = top copper, 2 = bottom copper
   DataStruct  *freepad;
   int         pad_usage;  // only drill and appertures with the same pad_usage are combined.
                           // default is 0
}GPCB_appkoo;  
typedef CTypedPtrArray<CPtrArray, GPCB_appkoo*> CAppkooArray;

// collects all drill holes, so that it can be merged with aperture data
// to create padstacks
typedef struct
{
   double      x;
   double      y;
   DataStruct  *data;
   int         bnum;
   int         used;
   int         layer;
   int         pad_usage;  // only drill and appertures with the same pad_usage are combined.
                           // default is 0
}GPCB_drillkoo;   
typedef CTypedPtrArray<CPtrArray, GPCB_drillkoo*> CDrillkooArray;

// this is the resulting padstack array, generated from gpcb.
typedef struct
{
   int      layer;   
   int      bnum;
   int      index;   // into appkoo
   int      drill;   // into drillkooarray+1
}GPCB_padstack;      // used in derive netlist



int exchange_padstacks(CCEtoODBDoc *doc, CProgressDlg *progress, double tol, int drillmode, int drillayer);
int convert_padstacks_pins_via(CCEtoODBDoc *doc, CProgressDlg *progress, double tol);
int DeriveNetlist(CCEtoODBDoc *doc, FileStruct *file); // this si defined in netlist.cpp

/*****************************************************************************/
/*
   end gerberpcb.h
*/
/*****************************************************************************/

///////////////////////////////////////////////////////////////
// PolyDrawToFlash dialog			                            //
///////////////////////////////////////////////////////////////
class PolyDrawToFlash : public CDialog
{
// Construction
public:
   PolyDrawToFlash(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PolyDrawToFlash)
   enum { IDD = IDD_POLYDRAWTOFLASH };
   CString  m_maximum_width;
   CString  m_minimum_width;
   CString  m_tolerance;
   CString  m_exclude_dcodes;
   int      m_process_area;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PolyDrawToFlash)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PolyDrawToFlash)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


///////////////////////////////////////////////////////////////
// CFlashToPolyDlg dialog			                            //
///////////////////////////////////////////////////////////////
class CFlashToPolyDlg : public CDialog
{
	DECLARE_DYNAMIC(CFlashToPolyDlg)

public:
	CFlashToPolyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlashToPolyDlg();

// Dialog Data
	enum { IDD = IDD_PCB_FLASHTOPOLY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();

public:
	int m_Unit;
	double m_Width;

	BOOL m_WidthOption;
	CComboBox m_UnitComboBox;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeUnit();
	afx_msg void OnBnClickedZerowidth();
	afx_msg void OnBnClickedWidth();
};

#if defined(EnableNewPolyToFlashConverter)
//_____________________________________________________________________________
class CPolyToFlashConverter
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;

public:
   CPolyToFlashConverter(CCEtoODBDoc& camCadDoc);
   ~CPolyToFlashConverter();

   void convertPolysToFlashes(bool processAreaFlag,double tolerance,double minWidth,double maxWidth);
   void OptimizeComplexApertures(CDataList& padstackDataList,double accuracy);
   void OptimizeDuplicateApertures(double accuracy);
   void OptimizeTouchingApertures(BlockStruct& fileBlock);
   bool OptimizeComplexApertureBlock(BlockStruct& block,int insertLayer,double accuracy);
};
#endif // EnableNewPolyToFlashConverter

#endif
