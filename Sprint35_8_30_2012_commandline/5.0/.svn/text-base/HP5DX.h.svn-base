// $Header: /CAMCAD/4.4/HP5DX.h 10    8/18/04 2:54p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

#include <afxtempl.h>

#define  MAX_BOARDS        255

#define  NotAllowed        "@#.,:'\"|;<>/ "

typedef struct
{
   int      geomnumber;
   double   xmin, ymin, xmax, ymax;
   CString  geomname;
   CString  boardname;
   CString  topname;
   CString  bottomname;
   CString  tophashname;
   CString  bottomhashname;
}Boardname;

typedef struct
{
   CString  geomname;
   CString  prefix;
   CString  package_id;
   int      used:1;        // used in design
   int      changed:1;     // changed in dialog
}HP5DXF_PackageMap;
typedef CTypedPtrArray<CPtrArray, HP5DXF_PackageMap*> HP5DXF_PackageMapArray;

typedef struct
{
   CString  name;
}HP5DXF_Material;
typedef CTypedPtrArray<CPtrArray, HP5DXF_Material*> HP5DXF_MaterialArray;


/////////////////////////////////////////////////////////////////////////////
// HP_Export dialog
class HP_Export : public CDialog
{
// Construction
public:
   HP_Export(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HP_Export)
   enum { IDD = IDD_HP5DX_EXPORT };
   CString  m_dir;
   CString  m_name;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_Export)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HP_Export)
   afx_msg void OnBrowse();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// HP_DeafultSettingsDlg dialog
class HP_DeafultSettingsDlg : public CDialog
{
// Construction
public:
   HP_DeafultSettingsDlg(CString defaultStageSpeed, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HP_DeafultSettingsDlg)
   enum { IDD = IDD_HP5DX_DEFAULT_SETTINGS };
   CComboBox   m_ctl_othermaterial;
   CString  m_boardThickness;
   CString  m_rotation;
   CString  m_stageSpeed;  // The active setting
   CString  m_defaultStageSpeed; // Possible user setting from .out file
   int      m_units;
   BOOL     m_nonorthogonal;
   BOOL     m_enableAlphaNumericePnNumbers;
   int      m_material;
   CString  m_othermaterial;
   //}}AFX_DATA
   int fromUnits;
   int page_units;
   double thickness;
   CString material;
   HP5DXF_MaterialArray *materialarray;
   int materialcnt;

   CString validatedStageSpeed(CString stageSpeedStr);


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_DeafultSettingsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HP_DeafultSettingsDlg)
   afx_msg void OnSelchangeRotationCb();
   afx_msg void OnSelchangeUnitsCb();
   virtual BOOL OnInitDialog();
   afx_msg void OnReset();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// HP_BoardNames dialog
class HP_BoardNames : public CDialog
{
// Construction
public:
   HP_BoardNames(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HP_BoardNames)
   enum { IDD = IDD_HP5DX_BOARD_NAMES };
   CListCtrl   m_list;
   //}}AFX_DATA
   Boardname *array;
   int maxArray;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_BoardNames)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HP_BoardNames)
   virtual BOOL OnInitDialog();
   afx_msg void OnEdit();
   afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// HP_DirTree dialog
class HP_DirTree : public CDialog
{
// Construction
public:
   HP_DirTree(CWnd* pParent = NULL);   // standard constructor
   ~HP_DirTree(); 

// Dialog Data
   //{{AFX_DATA(HP_DirTree)
   enum { IDD = IDD_HP5DX_DIR_TREE };
   CTreeCtrl   m_tree;
   //}}AFX_DATA
   Boardname *array;
   int maxArray;
   CImageList *m_imageList;
   CString panelHashName;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_DirTree)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HP_DirTree)
   virtual BOOL OnInitDialog();
   afx_msg void OnPrint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// HP_BoardNamesEdit dialog

class HP_BoardNamesEdit : public CDialog
{
// Construction
public:
   HP_BoardNamesEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HP_BoardNamesEdit)
   enum { IDD = IDD_HP5DX_BOARD_NAMES_EDIT };
   CString  m_bottom;
   CString  m_camcad;
   CString  m_top;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_BoardNamesEdit)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(HP_BoardNamesEdit)
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// HP_PackageMap dialog

class HP_PackageMap : public CDialog
{
// Construction
public:
   HP_PackageMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(HP_PackageMap)
   enum { IDD = IDD_HP5DX_PACKAGEMAP };
   CListCtrl   m_list;
   //}}AFX_DATA
   HP5DXF_PackageMapArray *array;
   int maxArray;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(HP_PackageMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillList();

   // Generated message map functions
   //{{AFX_MSG(HP_PackageMap)
   virtual BOOL OnInitDialog();
   afx_msg void OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnAutoGen();
   afx_msg void OnPrint();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

