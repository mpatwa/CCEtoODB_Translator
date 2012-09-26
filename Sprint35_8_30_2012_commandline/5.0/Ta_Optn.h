// $Header: /CAMCAD/4.5/Ta_Optn.h 11    1/27/05 7:43p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#include "GeneralDialog.h"

struct TA_OptionsStruct
{
   BOOL     Unusedpins;       // used for Access and Probe
   double   MaxProbeSize;     
   //int    OutlineGeneration;

   struct ProbePlaceStruct
   {
      BOOL     optimize;      // do the optimization after probe placement
      BOOL     prefernetaccesssurface;
      BOOL     allownetaccessbottom;
      BOOL     allownetaccesstop;
      int      totalaccessnets;     // total nets with access points, single + multinets
      int      singleaccessnets;    // single nets with access points
      int      multiaccessnets;     // nets with access points, which would need probes.
      int      totnets;             // total nets including access or not
      int      snets;               // total single nets including access or not
      int      mnets;               // total multi pin nets including access or not 
   } ProbePlace;

   struct AccessAnalysisStruct
   {
      int      throughPinAccess; // 0=non mount side, 1=both
      BOOL     excludeSinglePin;
      BOOL     soldermaskTop;
      BOOL     soldermaskBot;
      double   testGraphicSize; // size which the ACCESS graphic is displayed in the screen.
      BOOL     allowNetAccessTop;
      BOOL     allowNetAccessBot;
      BOOL     preferNetAccessSurface; // 0 = top, 1 = bottom
      BOOL     boardFeatureTop; // on/off distance to board outline
      BOOL     boardFeatureBot; // on/off distance to board outline
      BOOL     compFeatureTop; // on/off distance to component outline
      BOOL     compFeatureBot; // on/off distance to component outline
      BOOL     featureSizeTop;
      BOOL     featureSizeBot;
      double   boardOutlineDistanceTop;
      double   boardOutlineDistanceBot;
      double   compOutlineDistanceTop;
      double   compOutlineDistanceBot;
      double   featureSizeMinTop;
      double   featureSizeMinBot;

      int topTestattribPriority, topConnectorPriority, topThroughholePriority, topSMDPriority, topViaPriority,
         botTestattribPriority, botConnectorPriority, botThroughholePriority, botSMDPriority, botViaPriority;

      BOOL MFNtopTestattrib, MFNtopConnector, MFNtopThroughhole, MFNtopSMD, MFNtopVia,
         MFNbotTestattrib, MFNbotConnector, MFNbotThroughhole, MFNbotSMD, MFNbotVia;

      BOOL SFNtopTestattrib, SFNtopConnector, SFNtopThroughhole, SFNtopSMD, SFNtopVia,
         SFNbotTestattrib, SFNbotConnector, SFNbotThroughhole, SFNbotSMD, SFNbotVia;

   } AccessAnalysis;

   struct   // this structure is used for the new test Access Analysis
   {
      //BOOL netaccess_allowed;     // 1 == top, 2 = bottom , 3 = all, 0 = none
      BOOL  top, bottom;
   } ProbeAssignment;

   struct ProbableShortsStruct
   {
      BOOL pin_pin;
      double pin_pinSize;
      BOOL pin_feature;
      double pin_featureSize;
      BOOL feature_feature;
      double feature_featureSize;
   } ProbableShorts;
};

class CCEtoODBDoc;
class FileStruct;

/////////////////////////////////////////////////////////////////////////////
// TA_Options_AccessAnalysis dialog
class TA_Options_AccessAnalysis : public CDialog
{
// Construction
public:
   TA_Options_AccessAnalysis(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(TA_Options_AccessAnalysis)
   enum { IDD = IDD_TA_TEST_ACCESS_ANALYSIS };
   CListBox m_topPriorityLB;
   CListBox m_botPriorityLB;
   CString  m_topboardfeaturemin;
   CString  m_topfeaturesizemin;
   CString  m_topphysicalaccess;
   CString  m_topfeaturetypetotal;
   int      m_prefer;
   CString  m_topcomponentfeaturemin;
   BOOL  m_topboardfeature;
   BOOL  m_topcomponentfeature;
   BOOL  m_topfeaturesize;
   int      m_through_pin_access;
   BOOL  m_SFNbotConnector;
   BOOL  m_SFNbotSMD;
   BOOL  m_SFNbotTestattrib;
   BOOL  m_SFNbotThroughhole;
   BOOL  m_SFNbotVia;
   BOOL  m_SFNtopConnector;
   BOOL  m_SFNtopSMD;
   BOOL  m_SFNtopTestattrib;
   BOOL  m_SFNtopThroughhole;
   BOOL  m_SFNtopVia;
   BOOL  m_MFNbotConnector;
   BOOL  m_MFNbotSMD;
   BOOL  m_MFNbotTestattrib;
   BOOL  m_MFNbotThroughhole;
   BOOL  m_MFNbotVia;
   BOOL  m_MFNtopConnector;
   BOOL  m_MFNtopSMD;
   BOOL  m_MFNtopTestattrib;
   BOOL  m_MFNtopThroughhole;
   BOOL  m_MFNtopVia;
   BOOL  m_botAllowAccess;
   CString  m_botBoardFeatureMin;
   BOOL  m_botBoardFeature;
   CString  m_botCompFeatureMin;
   BOOL  m_botCompFeature;
   CString  m_botFeatureSizeMin;
   BOOL  m_botFeatureSize;
   CString  m_botFeatureTypeTotal;
   CString  m_botPhysicalAccess;
   BOOL  m_topSoldermask;
   BOOL  m_botSoldermask;
   CString  m_botTotalAccess;
   CString  m_totalNets;
   CString  m_totalMFNets;
   CString  m_totalSFNets;
   CString  m_totalAccess;
   CString  m_totalAccessPercent;
   CString  m_totalMFAccess;
   CString  m_totalMFAccessPercent;
   CString  m_totalSFAccess;
   CString  m_totalSFAccessPercent;
   CString  m_topTotalAccess;
   BOOL  m_topAllowAccess;
   BOOL  m_excludeSinglePin;
   //}}AFX_DATA

   int topTestattribPriority, topConnectorPriority, topThroughholePriority, topSMDPriority, topViaPriority,
      botTestattribPriority, botConnectorPriority, botThroughholePriority, botSMDPriority, botViaPriority;

   CCEtoODBDoc *doc;
   FileStruct *pcbFile;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TA_Options_AccessAnalysis)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
   void FillTopPriorityLB();
   void FillBotPriorityLB();
protected:

   // Generated message map functions
   //{{AFX_MSG(TA_Options_AccessAnalysis)
   afx_msg void OnProcess();
   afx_msg void OnView();
   virtual void OnOK();
   afx_msg void OnTaProbeHelp();
   virtual void OnCancel();
   afx_msg void OnTopVia();
   afx_msg void OnTopThroughhole();
   afx_msg void OnTopTestAttrib();
   afx_msg void OnTopSmd();
   afx_msg void OnTopMoveUp();
   afx_msg void OnTopMoveDown();
   afx_msg void OnTopConnector();
   afx_msg void OnBotVia();
   afx_msg void OnBotThroughhole();
   afx_msg void OnBotTestAttrib();
   afx_msg void OnBotSmd();
   afx_msg void OnBotMoveUp();
   afx_msg void OnBotMoveDown();
   afx_msg void OnBotConnector();
   virtual BOOL OnInitDialog();
   afx_msg void OnAllowAccessBot();
   afx_msg void OnAllowAccessTop();
   afx_msg void OnEnableSinglePinMountSide();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

