// $Header: /CAMCAD/4.5/Ta_Probe.h 11    1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "GeneralDialog.h"
#include <afxtempl.h>

// used in test probe placement
typedef struct
{
   int      netindex;               // netindex -1 for unconnected pins.
   long     access_entitynum;
   long     via_pin_entitynum;      // from DDLink of the Access, which links to the entity
   double   x,y;
   int      slayer;                 // 1 = top, 2 = bottom
   int      allowedlayer;           // after allowed layer mask (only 1 or 2),this determines the placement layer
   int      preference;             // 0 is highest - 99999 is lowest preference
   int      cost;                   // initial set to preference, but than grows.
   int      layercost;              // non prefered layer gets a LAYERCOST                            
   int      net_numberofaccess_top; // how many access points does this net have.
   int      net_numberofaccess_bot; // how many access points does this net have.
   int      probe_placed;           // a probe was placed on this location
   int      probe_index;            // index into CTestProbeList
   double   probe_size;
   double   probe_drillsize;
   double   probe_headsize;
   double   distance;               // distance to the next obstical (component or board outline).
   CString  probe_prefix;
   CString  probe_name;
} TA_ProbePlace;
typedef CTypedPtrArray<CPtrArray, TA_ProbePlace*> ProbePlaceArray;

// used in test probe placement
typedef struct
{
   CString  netname;
   int      nonconn;                // that flags a single point net
   int      number_required_probes; // attribute in the netlist
   int      number_placed_probes;
   int      number_of_access_top, number_of_access_bot;  // numbers of accesspoints availabe
} TA_ProbePlaceNet;
typedef CTypedPtrArray<CPtrArray, TA_ProbePlaceNet*> ProbePlaceNetArray;

typedef struct // here are the results on probe placement run 
{
   int   probed_nets_total, probed_nets_multi, probed_nets_single;
   int   needed_probes, placed_probes;
} TA_PCBFileProbePlace; // this holds the pcbfile values needed in the new ProbePlaceDialog;

struct TestProbeStruct
{
   int      index;
   int      onoff;   // on is true
   int      cost;
   CString  probename;
   double   size;
   CString  description;
   int      placedtop, placedbot;   // updated during process
   CString  probedrilltool;
   CString  probeprefix;
   double   drillsize;
   double   headsize;
};
typedef CTypedPtrList<CPtrList, TestProbeStruct*> TestProbeList;

class CCEtoODBDoc;
class FileStruct;

/////////////////////////////////////////////////////////////////////////////
// TestProbePlacement dialog
class TestProbePlacement : public CDialog
{
// Construction
public:
   TestProbePlacement(CWnd* pParent = NULL);   // standard constructor
   double Get_smallestprobesize(const char *netname, int layer);

// Dialog Data
   //{{AFX_DATA(TestProbePlacement)
   enum { IDD = IDD_TA_TEST_PROBE_PLACEMENT };
   CListCtrl   m_list;
   CString  m_totalnets;
   CString  m_totalaccess;
   CString  m_singleportaccess;
   CString  m_singleportnets;
   CString  m_multiportaccess;
   CString  m_multiportnets;
   BOOL  m_bottomallowaccess;
   BOOL  m_topallowaccess;
   CString  m_bottomtotalaccess;
   CString  m_toptotalaccess;
   int      m_prefer;
   CString  m_totalaccessPercent;
   CString  m_singleportaccessPercent;
   CString  m_multiportaccessPercent;
   BOOL  m_alwaysOptimize;
   CString  m_placedprobes;
   CString  m_totalprobes;
   CString  m_totalnets_access;
   CString  m_singleportnets_access;
   CString  m_multiportnets_access;
   //}}AFX_DATA

   CCEtoODBDoc *doc;
   FileStruct *pcbFile;
   TestProbeList list;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TestProbePlacement)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void FillList();
   void RemoveAll();

   // Generated message map functions
   //{{AFX_MSG(TestProbePlacement)
   afx_msg void OnDblclkTestProbe(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnProcess();
   afx_msg void OnView();
   afx_msg void OnTaProbeHelp();
   virtual void OnOK();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
   afx_msg void OnClearProbe();
   afx_msg void OnLoad();
   afx_msg void OnSave();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
