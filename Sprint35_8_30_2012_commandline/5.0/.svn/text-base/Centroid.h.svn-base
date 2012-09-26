// $Header: /CAMCAD/4.5/Centroid.h 15    6/07/06 11:03a Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include "block.h"
#include "ccdoc.h"
#include "afxwin.h"

#define CENTROIDGEOM    "$centroid"

//-----------------------------------------------------------------------------
// Algorithm codes
#define Centroid_PinCenters      1
#define Centroid_PinExtents      2
#define Centroid_CompOutline     3
#define Centroid_PinBodyExtents  4
// Not really algorithm codes, but used to test same variables
#define Centroid_XyValue         -1 // Must be less than zero
#define Centroid_None            -5 // Must be less than zero, original code used constant -5, but I don't think there is really anything special about -5

#define IsCentroidCalculatable(alg) (((alg) > 0) && ((alg) < 5))
//-----------------------------------------------------------------------------

void generate_CENTROIDS(CCEtoODBDoc *doc);

int GenerateComponentCentroid(CCEtoODBDoc *doc, FileStruct *file, short method);

DataStruct *centroid_exist_in_block(BlockStruct *block);

// The generate_Centroid_Geom(doc) function has been removed completely and replaced
// with doc->GetCentroidGeometry(). This comment has been left behind in case
// you come looking for the old function and wonder what happened. It was moved
// to resolve a "can't get there from here" problem between block.cpp and centroid.cpp.
/////BlockStruct* generate_Centroid_Geom(CCEtoODBDoc *doc);


BlockStruct *createStandardCentroid(CCEtoODBDoc *doc);                // create std centroid from scratch
void defineStandardCentroid(CCEtoODBDoc *doc, BlockStruct *block);    // put standard centroid data in given block
void redefineStandardCentroid(CCEtoODBDoc *doc, BlockStruct *block);  // get rid of current block data and fill with standard centroid data
void validateCentroidVisibility(CCEtoODBDoc *doc);                    // check if existing centroid geom has visible graphic, if not, offer to fix it

bool CalculateCentroid(CCEtoODBDoc *doc, BlockStruct *block, int method, double *x, double *y);
bool CalculateCentroid_PinCenters(CCEtoODBDoc *doc, BlockStruct *block, double *x, double *y);
bool CalculateCentroid_Extents(CCEtoODBDoc *doc, BlockStruct *block, bool includePins, bool includeBody, double *x, double *y);

bool GetComponentOutlineExtents(CCEtoODBDoc *doc, BlockStruct *block, bool includePins, bool includeBody, CExtent &compExtents);
bool GenerateCentroidRotationOffset(BlockStruct *inputReferenceBlock, DataStruct *inputCentroid, double &outputRotation);
bool AnalyzeCentroidRotationOffset(BlockStruct *inputReferenceBlock, DataStruct *inputCentroid, double &outputRotation, bool& isSingleRow, int& pinCount, CString &outMessage);


// CCentroidValidateDlg dialog

class CCentroidValidateDlg : public CDialog
{
	DECLARE_DYNAMIC(CCentroidValidateDlg)

public:
	CCentroidValidateDlg(CCEtoODBDoc *doc, BlockStruct *block, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCentroidValidateDlg();

// Dialog Data
	enum { IDD = IDD_CENTROID_VALIDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

   CCEtoODBDoc *m_doc;
   BlockStruct *m_block;

   CString m_messageTxt;
   int m_dispositionRadio;
   afx_msg void OnBnClickedOk();
   
};
