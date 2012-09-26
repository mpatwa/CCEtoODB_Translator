// $Header: /CAMCAD/4.6/License.h 16    1/15/07 5:08p Lynn Phung $

#pragma once

#include "resource.h"
#include "lic.h"

/* Date */
//struct DateStruct
//{  //                          legal range  true range
//   unsigned int   hour:5;     // [0..23]     [0..31]
//   unsigned int   minute:6;   // [0..59]     [0..63]
//   unsigned int   month:4;    // [1..12]     [0..15]
//   unsigned int   day:5;      // [1..31]     [0..31]
//   unsigned int   year:12;    // [1997..)    [0..4095]
//};


// check if the maintenance has expired
//deadcode bool haveMaintenance(CString &errMsg, CString &num_days);

int GetMonth(CString sMonth);
CString getLicensePrefix();

/////////////////////////////////////////////////////////////////////////////
// LicenseDialog dialog

class LicenseDialog : public CDialog
{
// Construction
public:
   LicenseDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LicenseDialog)
   enum { IDD = IDD_LICENSES };
   CListBox m_LicenseLB;
   CString  m_Company;
   CString  m_User;
   CString  m_product;
   int      m_duration;
   BOOL     m_dongle;
   BOOL     m_temporary;
   int      m_day;
   int      m_month;
   int      m_year;
   //}}AFX_DATA

   BOOL Modified;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LicenseDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillListBox();

   // Generated message map functions
   //{{AFX_MSG(LicenseDialog)
   //virtual void OnOK();
   virtual BOOL OnInitDialog();
   //afx_msg void OnChangeUser();
   afx_msg void OnHelp();
   //afx_msg void OnDblclkLicences();
   //afx_msg void OnChangeDate();
   //afx_msg void OnTemporary();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedRescan();
};

// end license.h
/////////////////////////////////////////////////////////////////////////////
// LicenseNumber dialog

class LicenseNumber : public CDialog
{
// Construction
public:
   LicenseNumber(char * code,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LicenseNumber)
   enum { IDD = IDD_LICENSE_NUM };
   CString  m_Text;
   CString  m_AccessCode;
   //}}AFX_DATA

   char *m_code;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LicenseNumber)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(LicenseNumber)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// PasswordDlg dialog

class PasswordDlg : public CDialog
{
// Construction
public:
   PasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(PasswordDlg)
   enum { IDD = IDD_PASSWORD };
   CString  m_password;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PasswordDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(PasswordDlg)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// ChangePassword dialog

class ChangePassword : public CDialog
{
// Construction
public:
   ChangePassword(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(ChangePassword)
   enum { IDD = IDD_PASSWORD_CHANGE };
   CString  m_new1;
   CString  m_new2;
   CString  m_old;
   //}}AFX_DATA
   CString password;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(ChangePassword)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(ChangePassword)
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

