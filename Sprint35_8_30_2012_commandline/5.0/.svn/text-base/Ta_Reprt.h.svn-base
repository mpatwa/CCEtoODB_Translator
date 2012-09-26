// $Header: /CAMCAD/4.5/Ta_Reprt.h 13    1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "GeneralDialog.h"
#include <afxtempl.h>

class FileStruct;
class CCEtoODBDoc;

void report_load_netlist(FileStruct *file, CCEtoODBDoc *doc);
int get_report_netnameptr(const char *c);

typedef struct
{
   CString  netname;
   int      probecnt;
   int      probeneeded;   // this is set to 1 per net.
   int      accesscnt;     // access count, which is different than probe count. this is updated in  the
                           // global net access section in TA_report.
   int      nonconn;       // unused or single pin
   long     datalink;      // this is only used (right now) for assign the correct nonconn netname
} TA_Netname;
typedef CTypedPtrArray<CPtrArray, TA_Netname*> TA_NetnameArray;

// Testprobe is from CC file. Used in load_report test probes.
typedef struct
{
   CString  geomname;   
   CString  refname; 
   long     datalink;   // ddlink attribute
   int      probeindex;
   int      netindex;
   char     layer;   // 1 = top, 2 = bottom, 3 = both, 0 = none
   double   x,y;
   int      usecnt;
} TA_TestProbe;
typedef CTypedPtrArray<CPtrArray, TA_TestProbe*> TestProbeArray;

// Testaccess is from CC file. Used in load_report test probes.
typedef struct
{
   CString  geomname;   
   CString  refname; 
   long     datalink;   // 0 is not initialized.
   long     entitynumber;
   int      netindex;
   char     layer;   // 1 = top, 2 = bottom, 3 = both, 0 = none
   double   x,y;
   int      usecnt;
} TA_TestAccess;
typedef CTypedPtrArray<CPtrArray, TA_TestAccess*> TestAccessArray;

typedef struct
{
	CString	name;
	double	x;
	double	y;
	CString	surface;
	CString	violation;
} TestAccessLoc;
typedef CTypedPtrArray<CPtrArray, TestAccessLoc*> TestAccessLocArray;

typedef struct
{
	CString					netname;
	int						topCount;
	int						botCount;
	TestAccessLocArray	accessLocArray;
	int						accessLocArrayCnt;
	TestAccessLocArray	violateLocArray;
	int						violateLocArrayCnt;
} TestAccessNet;
typedef CTypedPtrArray<CPtrArray, TestAccessNet*> TestAccessNetArray;

typedef struct
{
	int						zeroAccess;			// no access
	int						oneAccess;			// one access
	int						twoAccess;			// two accesses
	int						twoPlusAccess;		// more than accesses
	int						totalAccess;
	TestAccessNetArray	netArray;
	int						netArrayCnt;
} TestAccessReport;

/////////////////////////////////////////////////////////////////////////////
// TA_Reports dialog
class TA_Reports : public CDialog
{
// Construction
public:
   TA_Reports(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TA_Reports)
   enum { IDD = IDD_TA_REPORTS };
   BOOL  m_testability;
   CString  m_testabilityFile;
   BOOL  m_testprobe;
   CString  m_testprobeFile;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TA_Reports)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TA_Reports)
   afx_msg void OnCdTestability();
   afx_msg void OnCdTestprobe();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
