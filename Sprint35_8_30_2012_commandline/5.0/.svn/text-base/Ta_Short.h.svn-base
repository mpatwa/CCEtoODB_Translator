// $Header: /CAMCAD/4.4/Ta_Short.h 9     5/25/04 1:57p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"


// this is for vias and other padstacks from the datastruct
typedef struct
{
   CString netname;
   long entitynumber;
   double x;
	double y;
	double rot;
   int mirror;                 // is entity mirrored
   int aperturepadstackindex;  // index into the aperture padstack
   int layer;                  // 1 top , 2 bottom, 3 all
} PS_PadstackKoo;
typedef CTypedPtrArray<CPtrArray, PS_PadstackKoo*> PS_PadstackKooArray;


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CDFTProbableShort
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CDFTProbableShort
{
public:
	CDFTProbableShort(CCEtoODBDoc *pDoc, FileStruct *pPCBFile);
	~CDFTProbableShort();

private:
	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pPCBFile;
	FILE *m_pLogFile;
	PS_PadstackKooArray padstackKooArray;

   BOOL m_bPinPin;
   BOOL m_bPinFeature;
   BOOL m_bFeatureFeature;
   double m_dPinPinSize;
   double m_dPinFeatureSize;
   double m_dFeatureFeatureSize;

	void loadSettingFile();
	void deleteDRCbyType(int drcType);
	void runTestShortsAnalysis();
	double getLargestApertureSize();
	void doAllPadstacks(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
	int DRC_RunTestShortsAnalysis();

public:	
	int DoProbableShortAnalysis();
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// TA_ProbableShorts dialog
////////////////////////////////////////////////////////////////////////////////
class TA_ProbableShorts : public CDialog
{
// Construction
public:
   TA_ProbableShorts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TA_ProbableShorts)
   enum { IDD = IDD_TA_PROBABLE_SHORTS };
   BOOL  m_pin_pin;
   CString  m_pin_pinSize;
   BOOL  m_pin_feature;
   CString  m_pin_featureSize;
   BOOL  m_feature_feature;
   CString  m_feature_featureSize;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TA_ProbableShorts)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TA_ProbableShorts)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
