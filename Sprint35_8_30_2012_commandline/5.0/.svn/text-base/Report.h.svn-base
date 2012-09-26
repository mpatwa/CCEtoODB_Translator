// $Header: /CAMCAD/4.5/Report.h 13    5/03/06 1:08p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "resource.h"


extern int GetBoardSize(CCEtoODBDoc *Doc, 
                        CDataList *DataList, double *xmin, double *ymin, double *xmax, double *ymax);

extern CString FormatDecimal(double value, int decimal_places = 3);

//Command definition in Components.out
#define  COMPOUTCMD_ATTRIBUTE       ".ATTRIBUTE"
#define  COMPOUTCMD_QUOTEALLFIELDS  ".QUOTE_ALL_FIELDS"

//**************************************************
// Entity Number Reporter

class CMapInt : public CMap<int, int, int, int>
{
};

class CEntityReportRec : public CObject
{
public:
   int count;
   CString desc;
};

class CEntityNumberReporter
{

private:
   CCEtoODBDoc *m_doc;
   CMapInt m_entityMap;
   CTypedObArrayWithMapContainer<CEntityReportRec> m_entityMap2;

   void TallyEntity(int entNum, CString desc, CMapInt &entityMap);
   void CountFindDataEntity(CCEtoODBDoc *doc, CMapInt &entityMap);
   void CountFindCompPinEntity(CCEtoODBDoc *doc, CMapInt &entityMap);
   void CountFindNetEntity(CCEtoODBDoc *doc, CMapInt &entityMap);
   void CountFindDrcEntity(CCEtoODBDoc *doc, CMapInt &entityMap);

public:
   CEntityNumberReporter(CCEtoODBDoc *doc);
   bool HasDuplicates();
   void WriteFile(CString filename);
   void NotifyUser();

};

//**************************************************

enum EReports
{
	reportsManufacturingReport		= 0,
	reportsComponents					= 1,
	reportsGeometries					= 2,
	reportsLayers						= 3,
	reportsNetList						= 4,
	reportsApertures					= 5,
	reportsToolList					= 6,
	reportsViaList						= 7,
	reportsLineLength					= 8,
	reportsTestAttribute				= 9,
	reportsThroughHolePins			= 10,
	reportsPinToPinLength			= 11,
	reportsTestProbe					= 12,
   reportsAdvancedPackaging      = 13,

   // enum max, not a report type.
   // one greater than actual max report type.
	reportsMax							= 14,
};

/////////////////////////////////////////////////////////////////////////////
// ReportSpreadsheet dialog
class ReportSpreadsheet : public CDialog
{
// Construction
public:
   ReportSpreadsheet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ReportSpreadsheet)
   enum { IDD = IDD_REPORTS_SPREADSHEET };
   CListBox m_list;
   CString  m_directory;
   //}}AFX_DATA
   BOOL rs[reportsMax];


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ReportSpreadsheet)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ReportSpreadsheet)
   afx_msg void OnBrowse();
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CComponentOutSetting
class CComponentOutSetting
{
private:
   CString m_SettingFileName;
   bool m_optionQuoteAllFields;
   CStringArray m_compsColumnHeaders;

private:
   void addComponentHeader(CString column);

public:
   CComponentOutSetting();
   void LoadDefaultSettings();
   void LoadCompsReportSettings(CString FileName);

   //Parameters
   bool getOptionQuoteAllFields(){return m_optionQuoteAllFields;}
   void setOptionQuoteAllFields(bool val) {m_optionQuoteAllFields = val;}
   CStringArray &getcompsColumnHeaders(){ return m_compsColumnHeaders;}

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


