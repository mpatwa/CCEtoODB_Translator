// $Header: /CAMCAD/4.5/Export.h 24    4/05/06 6:17p Lynn Phung $

#if !defined(AFX_EXPORT_H__C69C2450_6F86_11D3_865D_004005408E44__INCLUDED_)
#define AFX_EXPORT_H__C69C2450_6F86_11D3_865D_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// export.h : header file
//

#include <afxtempl.h>

typedef struct
{
   int      filenumber;       // allows multifiles in panel
   BOOL     onoff;            // on = 1, 0 = off
   BOOL     topbottom;        // 0 = top, 1 = bottom, indicate the surface of test
   CString  refname;
   CString  tkrefname;        // takaya refname
   CString  value;
   CString  comment;
   CString  location;         // Grid location, otherwise '*'
   CString  element;          // Type of test: R=resistance, C=capacitance, L=inductance, D=diode, Z=Zener Diode(Takaya CA8)
   long     x1, y1, x2, y2;   // absolute koos on takaya machine. (bottom is -x mirrored
   CString  netName1;         // "N" + the probe numbers in the Pin Coordinated Data Lines section of Takaya CA9
   CString  netName2;         // "N" + the probe numbers in the Pin Coordinated Data Lines section of Takaya CA9
   CString  option;
	CString  mergedStatus;
	CString  mergedValue;
	CString	testStrategy;
	int		deviceType;       //device type of the component
	CString	subClass;
	bool		writeSelectVision;
	bool		isICOpenTest;

	// Ground net names and locations
	long		gx1, gy1, gx2, gy2;
	CString  gNetName1;
	CString  gNetName2;
	int		compSurfaceBottom;// 0 = top, 1 = bottom, indicate the surface of component, can be different than topbottom variable, use for vision test

	// entity number of test access points
	long testAccessEntityNumber1;		// entity number of frist access point
	long testAccessEntityNumber2;		// entity number of second access point
	long gTestAccessEntityNubmer1;		// entity number of frist ground access point
	long gTestAccessEntityNubmer2;		// entity number of second  ground access point
   long compX, compY;                  // location of component centroid, if no centroid then location of component insert, use for version test only
}TKoutput;
typedef CTypedPtrArray<CPtrArray, TKoutput*> TKOutputArray;

typedef struct
{
   int      probeNumber;   // the probe number in the Pin Coordinated Data Lines section of Takaya CA9
   CString  netName;       // the netname of the net that hits by this probe
   long     x;             // absolute koos on takaya machine. (bottom is -x mirrored
   long     y;             // absolute koos on takaya machine. (bottom is -x mirrored
   BOOL     onOff;         // on = 1, 0 = off
   BOOL     topBottom;     // 0 = top, 1 = bottom
	long		testAccessEntityNumber;
}TKoutputProbe;
typedef CTypedPtrArray<CPtrArray, TKoutputProbe*> TKOutputProbeArray;

typedef struct
{
   BOOL     onoff;      // on = 1, 0 = off
   BOOL     topbottom;  // 0 = top, 1 = bottom
   CString  refname;
   CString  value;
   double   ptol, ntol;
   CString  file;       // DAT or ICN file
   CString  tri_type;
   int      testpin[7]; // hi, lo, gp1, gp2, gp3, gp4, gp5
}TRIoutput;
typedef CTypedPtrArray<CPtrArray, TRIoutput*> TRIOutputArray;

/////////////////////////////////////////////////////////////////////////////
// TakayaOutput dialog

class TakayaOutput : public CDialog
{
// Construction
public:
   TakayaOutput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TakayaOutput)
   enum { IDD = IDD_TAKAYA_OUTPUT };
   CListCtrl   m_list;
   //}}AFX_DATA
   TKOutputArray *array;
   int count;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TakayaOutput)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillItem(int actualItem, TKoutput *tk);

   // Generated message map functions
   //{{AFX_MSG(TakayaOutput)
   afx_msg void OnEdit();
   virtual BOOL OnInitDialog();
   afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// TAKAYAEdit dialog

class TAKAYAEdit : public CDialog
{
// Construction
public:
   TAKAYAEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TAKAYAEdit)
   enum { IDD = IDD_TAKAYA_EDIT };
   BOOL  m_onoff;
   CString  m_rd;
   CString  m_refname;
   CString  m_star;
   int      m_topbottom;
   CString  m_trefname;
   CString  m_val1;
   CString  m_val2;
   CString  m_x1;
   CString  m_x2;
   CString  m_y1;
   CString  m_y2;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TAKAYAEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(TAKAYAEdit)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// TRIOutput dialog

class TRIOutput : public CDialog
{
// Construction
public:
   TRIOutput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TRIOutput)
   enum { IDD = IDD_TRI_OUTPUT };
   CListCtrl   m_list;
   //}}AFX_DATA
   TRIOutputArray *array;
   int count;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TRIOutput)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillItem(int actualItem, TRIoutput *tri);
protected:

   // Generated message map functions
   //{{AFX_MSG(TRIOutput)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORT_H__C69C2450_6F86_11D3_865D_004005408E44__INCLUDED_)
