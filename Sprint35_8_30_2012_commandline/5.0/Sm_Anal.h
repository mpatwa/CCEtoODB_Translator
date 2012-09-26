// $Header: /CAMCAD/4.6/Sm_Anal.h 22    7/24/06 4:22p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

//#include "DFT.h"
#include "region.h"
#include "polylib.h"
#include "extents.h"

class CExposedDataMap;
class CHeightAnalysis;

void CreateExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile);
void SolderMaskAnalysis(CCEtoODBDoc *doc, FileStruct *pcbFile);
void CopyExplodeMetalLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, CExposedDataMap *exposedDataMap);
void CopySoldermaskAnalysisLayers(CCEtoODBDoc *doc, FileStruct *pcbFile, CExposedDataMap *exposedDataMap);
void xxxDeadCodexxxComponentOutlineDistanceAnalysis(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom, 
			double topCompOutlineDistance, double botCompOutlineDistance, CExposedDataMap *exposedDataMap,
			bool ignore1PinCompOutline, bool ignoreUnloadedCompOutline, bool useRealPartOutline,
			CHeightAnalysis* heightAnalysis, CHeightAnalysis* botHeightAnalysis);
int BoardOutlineDistanceAnalysis(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom, 
			CExposedDataMap *exposedDataMap, double topCompOutlineDistance, double botCompOutlineDistance);
void CleanUpUnusedLayer(CCEtoODBDoc *doc);

/* Define Section *********************************************************/


/* Struct Section *********************************************************/
struct SMApStruct
{
   double x, y;
   CAttributes** map;
};
typedef CTypedPtrList<CPtrList, SMApStruct*> SMApList;

// ERP stands for Extent, Region, PolyList
struct ERPStruct
{
public:
   CString refname;
   ExtentRect extents;
   Region *region;
	CPolyList *pPolyList;
};
typedef CTypedPtrList<CPtrList, ERPStruct*> CSMList;

/////////////////////////////////////////////////////////////////////////////
// SoldermaskAnalyzer dialog
class SoldermaskAnalyzer : public CDialog
{
// Construction
public:
   SoldermaskAnalyzer(CWnd* pParent = NULL);   // standard constructor
   CCEtoODBDoc *doc;

// Dialog Data
   //{{AFX_DATA(SoldermaskAnalyzer)
   enum { IDD = IDD_SOLDERMASK_ANALYZER_PREP };
   CString  m_bottomLayer;
   BOOL  m_deleteAttr;
   BOOL  m_deleteOrig;
   BOOL  m_explode;
   BOOL  m_graphicsFromAttr;
   CString  m_topLayer;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(SoldermaskAnalyzer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(SoldermaskAnalyzer)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

