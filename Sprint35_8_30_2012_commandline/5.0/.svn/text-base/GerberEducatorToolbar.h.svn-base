// $Header: /CAMCAD/4.5/GerberEducatorToolbar.h 11    12/14/05 10:50a Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#if ! defined (__GerberEducatorToolbar_h__)
#define __GerberEducatorToolbar_h__

#pragma once

#include "GerberEducatorSetupDataType.h"
#include "GerberEducatorReviewGeometries.h"
#include "GerberEducatorReviewInserts.h"


#define QGerberEducatorPCB                "Gerber Educator PCB"
#define QDataSourceGerber                 "Gerber"
#define QDataSourceCentroid               "Centroid"
#define QDataSourceDrill                  "Drill"
#define QDataSourceUnknown                "Unknown"
#define QDataSourceGerberEducatorInternal "Gerber Educator Internal"
#define QDataTypeUnknown						"Unknown"
#define QDataTypeTopPad                   "Pad - Top"
#define QDataTypeBottomPad                "Pad - Bottom"
#define QDataTypeTopSilkscreen            "Silkscreen - Top"
#define QDataTypeBottomSilkscreen         "Silkscreen - Bottom"
#define QDataTypeBoardOutline             "Board Outline"
#define QDataTypeTop                      "Top"
#define QDataTypeBottom                   "Bottom"
#define QDataTypeThru                     "Thru"
#define QDataTypeBlind                    "Blind"
#define QDataTypeBurried                  "Burried"


class CGELoadDataDlg;
class CGECleanUpGerberDlg;
class CGECreateGeometriesDlg;
class CGEReviewInsertsDlg;

//-----------------------------------------------------------------------------
// CGerberEducatorToolbar dialog
//-----------------------------------------------------------------------------
class CGerberEducatorToolbar : public CDialogBar
{
   DECLARE_DYNAMIC(CGerberEducatorToolbar)

private:
   bool m_bEnableLoadDataFile;
   bool m_bEnableSetupDataType;
   bool m_bEnableCleanUpGerber;
   bool m_bEnableCreateGeometries;
   bool m_bEnableReviewGeometries;
   bool m_bEnableReviewInserts;
   bool m_bEnableDeriveNetlist;

   CGESetupDataType* m_setupDataTypeDlg;
   CGEReviewGeometriesDlg* m_reviewGeometriesDlg;
   CGEReviewInsertsDlg* m_insertGeometriesDlg;

public:
   CGerberEducatorToolbar(CWnd* pParent = NULL);  
   virtual ~CGerberEducatorToolbar();
   enum { IDD = IDD_GE_Toolbar };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()

private:
   void OnInitDialog();

public:
   void InitializeButtons();
   void updateSelectedEntity();

   bool GetEnableLoadDataFiles() const          { return m_bEnableLoadDataFile;     }
   bool GetEnableSetupDataType() const          { return m_bEnableSetupDataType;    }
   bool GetEnableCleanUpGerber() const          { return m_bEnableCleanUpGerber;    }
   bool GetEnableCreateGeometries() const       { return m_bEnableCreateGeometries; }
   bool GetEnableReviewGeometries() const       { return m_bEnableReviewGeometries; }
   bool GetEnableReviewInserts() const          { return m_bEnableReviewInserts;    }

   bool checkStartTool(bool mustHavePCBFile);

   afx_msg void OnBnClickedLoadDataFiles();
   afx_msg void OnBnClickedSetupDataType();
   afx_msg void OnBnClickedCleanUpGerber();
   afx_msg void OnBnClickedCreateGeometries();
   afx_msg void OnBnClickedReviewGeometries();
   afx_msg void OnBnClickedReviewInserts();

   afx_msg void OnTerminateSetupDataType();
   afx_msg void OnTerminateReviewGeometries();
   afx_msg void OnTerminateReviewInserts();
};


//-----------------------------------------------------------------------------
// CGELoadDataDlg dialog
//-----------------------------------------------------------------------------
class CGELoadDataDlg : public CDialog
{
   DECLARE_DYNAMIC(CGELoadDataDlg)

public:
   CGELoadDataDlg(CWnd* pParent = NULL);
   virtual ~CGELoadDataDlg();
   enum { IDD = IDD_GE_LoadDataFiles };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
};


//-----------------------------------------------------------------------------
// CGECleanUpGerberDlg dialog
//-----------------------------------------------------------------------------
class CGECleanUpGerberDlg : public CDialog
{
   DECLARE_DYNAMIC(CGECleanUpGerberDlg)

public:
   CGECleanUpGerberDlg(CWnd* pParent = NULL);
   virtual ~CGECleanUpGerberDlg();
   enum { IDD = IDD_GE_CleanUpGerber };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()
};


//-----------------------------------------------------------------------------
// CGECreateGeometriesDlg dialog
//-----------------------------------------------------------------------------
class CGECreateGeometriesDlg : public CDialog
{
   DECLARE_DYNAMIC(CGECreateGeometriesDlg)

public:
   CGECreateGeometriesDlg(CWnd* pParent = NULL); 
   virtual ~CGECreateGeometriesDlg();
   enum { IDD = IDD_GE_CreateGeometries };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()
};

#endif
