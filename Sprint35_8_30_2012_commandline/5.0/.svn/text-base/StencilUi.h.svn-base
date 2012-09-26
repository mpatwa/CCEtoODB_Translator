// $Header: /CAMCAD/4.6/StencilUi.h 24    4/16/07 8:55p Kurt Van Ness $

#if !defined(__StencilUi_h__)
#define __StencilUi_h__

#pragma once

#include "CcDoc.h"

//#define STENCIL_TIMER
#ifdef STENCIL_TIMER
#include "ProfileLib.h"
CCamcadTimer &GetStencilTimer();
#endif

#define QGeometryPin      "geometryPin"
#define QComponentPin     "componentPin"
#define QComponentSubPin  "componentSubPin"
#define QComponent        "component"
#define QPadstack         "padstack"
#define QSubPadstack      "subPadstack"
#define QGeometry         "geometry"
#define QMount            "mount"
#define QSurface          "surface"
#define QPcb              "pcb"
#define QNone             "none"
#define QTop              "top"
#define QBottom           "bottom"
#define QBoth             "both"
#define QSmd              "smd"
#define QTh               "th"
#define QVia              "via"
#define QFiducial         "fiducial"

class CCamCadDatabase;
class CStencilGeneratorPropertySheet;
class CStencilGenerator;
class CStencilGenerationParameters;
class CStencilValidationParameters;
class CStencilErrorBrowseDialog;
class CStencilWaitDialog;
class CStencilViolations;
class CHtmlFileWriteFormat;
class CStencilGeneratorSettingsDatabase;
class CStencilEntityRuleArray;

enum StencilUiStateTag
{
   StencilUiStateEditProperties,
   StencilUiStateBrowsingErrors,
   StencilUiStateWaitingForContinue,
   StencilUiStateWaitingForContinueEdit,
   StencilUiStateUndefined
};

CString stencilUiStateToString(StencilUiStateTag tagValue);

enum StencilStatusTag
{
   StencilStatusPossiblyStale,
   StencilStatusStale,
   StencilStatusUpToDate,
   StencilStatusDoesNotExist,
   StencilStatusStaleUndefined
};

CString stencilStatusToString(StencilStatusTag tagValue);

enum StencilStateTag
{
   StencilStateInitialized,
   StencilStateInSync,
   StencilStateGenerationOutOfSync,
   StencilStateValidationOutOfSync,
   StencilStateGenerationParametersAndValidationOutOfSync,
   StencilStateGenerationParametersOutOfSync,
   StencilStateInitializedAndGenerationParametersOutOfSync,
   StencilStateUndefined
};

//_____________________________________________________________________________
class CStencilSettingEntry
{
private:
   CString m_entry;
   CString m_comment;
   bool m_hitFlag;
   int m_index;

public:
   CStencilSettingEntry(int index,const CString& entry,const CString& comment=CString());

   bool getHitFlag() const { return m_hitFlag; }
   void setHitFlag(bool flag) { m_hitFlag = flag; }

   CString getEntry() const { return m_entry; }
   void setEntry(const CString& entry) { m_entry = entry; }

   CString getComment() const { return m_comment; }
   void setComment(const CString& comment) { m_comment = comment; }

   int getIndex() const { return m_index; }
};

//_____________________________________________________________________________
class CStencilSettingEntryMap
{
private:
   CTypedPtrArrayWithMapContainer<CStencilSettingEntry> m_entries;

public:
   CStencilSettingEntryMap(int size);

   void addEntry(const CString& entryString,const CString& comment=CString());
   //void hitEntry(const CString& entryString);

   int getCount() const;
   CStencilSettingEntry* getAt(int index);
   CStencilSettingEntry* getAt(const CString& entryString);
   void write(CWriteFormat& writeFormat);
   void write(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase);
};

//_____________________________________________________________________________
class CStencilGeneratorSettingsDatabase
{
private:
   // future members - e.g. database name
   CString m_stencilRulesString;
   CString m_stencilEntitiesString;

public:
   CStencilGeneratorSettingsDatabase();

   CString getFieldSeparator() { return CString("\06"); }
   CString getRecordSeparator() { return CString("\05"); }

   void setStencilRule(const CString& entity,
      const CString& designator1,const CString& designator2,const CString& designator3,const CString& designator4,const CString& designator5,
      const CString& ruleString);

   void emptyStencilRules();
   void addStencilRule(const CString& entity,
      const CString& designator1,const CString& designator2,const CString& designator3,const CString& designator4,const CString& designator5,
      const CString& ruleString,const CString& comment);
   void setStencilRules();

   void emptyStencilEntities();
   void addStencilEntity(const CString& entity,const CString& designator1,const CString& designator2);
   bool getStencilEntityRules(bool propagateRules, CStencilEntityRuleArray& stencilEntityRules);

   void setStencilSetting(const CString& settingName,const CString& settingValue);
   void setStencilSetting(const CString& settingName,int settingValue);
   void setStencilSetting(const CString& settingName,double settingValue);
   void setStencilSetting(const CString& settingName,double settingValue,PageUnitsTag pageUnits);
   bool getStencilSetting(CString& settingValue,const CString& settingName);
   bool getStencilSettings(
      CStencilGenerationParameters& stencilGenerationParameters,
      CStencilValidationParameters& stencilValidationParameters);
};

//_____________________________________________________________________________
class CStencilUi : public CObject
{
private:
   static CStencilUi* m_stencilUi;

   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase* m_camCadDatabase;
   CStencilGeneratorPropertySheet* m_stencilGeneratorPropertySheet;
   CStencilGenerator* m_stencilGenerator;
   CStencilGenerationParameters* m_stencilGenerationParameters;
   CStencilGenerationParameters* m_renderedStencilGenerationParameters;
   CStencilValidationParameters* m_stencilValidationParameters;
   CStencilValidationParameters* m_renderedStencilValidationParameters;
   CStencilErrorBrowseDialog* m_stencilErrorBrowseDialog;
   CStencilWaitDialog* m_stencilWaitDialog;
   int m_pcbFileNum;
   CHtmlFileWriteFormat* m_htmlLogFile;
   CWriteFormat* m_logFile;

   StencilStateTag m_state;
   StencilUiStateTag m_uiState;

private:
   CStencilUi(CCEtoODBDoc& camCadDoc);
   ~CStencilUi();
   
private:
   StencilStateTag getState() { return m_state; }
   void setState(StencilStateTag state); 
   StencilUiStateTag getUiState() { return m_uiState; }
   void setUiState(StencilUiStateTag uiState);

public:
   static CStencilUi* getStencilUi() { return m_stencilUi; }

   CCamCadDatabase& getCamCadDatabase()
      { return *m_camCadDatabase; }
   CStencilGeneratorPropertySheet& getStencilGeneratorPropertySheet()
      { return *m_stencilGeneratorPropertySheet; }
   CStencilGenerator& getStencilGenerator();
   CStencilGenerationParameters& getStencilGenerationParameters()
      { return *m_stencilGenerationParameters; }
   CStencilGenerationParameters& getRenderedStencilGenerationParameters()
      { return *m_renderedStencilGenerationParameters; }
   CStencilValidationParameters& getStencilValidationParameters()
      { return *m_stencilValidationParameters; }
   CStencilValidationParameters& getRenderedStencilValidationParameters()
      { return *m_renderedStencilValidationParameters; }
   CStencilErrorBrowseDialog& getStencilErrorBrowseDialog()
      { return *m_stencilErrorBrowseDialog; }
   CStencilWaitDialog& getStencilWaitDialog()
      { return *m_stencilWaitDialog; }
   CStencilViolations& getStencilViolations();
   CString getLogFilePath();
   CString getPinReportFilePath();
   void deleteLogFile();
   CWriteFormat& getLogFile();
   CHtmlFileWriteFormat* getHtmlLogFile() { return m_htmlLogFile; }
   void renderStencils();
   void performStencilValidationChecks();

   StencilStatusTag getGenerationStatus();
   StencilStatusTag getValidationStatus();
   StencilStatusTag getReportStatus();
   int getValidationErrorCount();

private:
   void reportStencilUiStateError(const char* routine);
   void showStencilErrorBrowseDialog();
   void hideStencilErrorBrowseDialog();
   void showStencilWaitDialog();
   void hideStencilWaitDialog();
   void showStencilMicrocosmFile();
   void hideStencilMicrocosmFile();

public:
   void generateEvent(int commandId);
   void updateStatus();

   void OnEventConnectToStencilSession();       // ID_StencilCommand_ConnectToStencilSession
   void OnEventTerminateStencilSession();       // ID_StencilCommand_TerminateStencilSession
   void OnEventBrowseStencilErrors();           // ID_StencilCommand_BrowseStencilErrors
   void OnEventTerminateBrowseStencilErrors();  // ID_StencilCommand_TerminateBrowseStencilErrors
   void OnEventStencilPanZoom();                // ID_StencilCommand_PanZoom
   void OnEventStencilHidePropertySheet();      // ID_StencilCommand_HidePropertySheet
   void OnEventModifiedGenerationRule();        // ID_StencilCommand_ModifiedGenerationRule
   void OnEventValidationRulesInSync();         // ID_StencilCommand_ValidationRulesInSync
   void OnEventValidationRulesOutOfSync();      // ID_StencilCommand_ValidationRulesOutOfSync
   void OnEventModifiedValidationRules();       // ID_StencilCommand_ModifiedValidationRules
   void OnEventModifiedGenerationParameters();  // ID_StencilCommand_ModifiedGenerationParameters   
   void OnEventStencilRendered();               // ID_StencilCommand_StencilRendered
   void OnEventValidationPerformed();           // ID_StencilCommand_ValidationPerformed
   void OnEventContinueStencilSession();        // ID_StencilCommand_ContinueStencilSession

   //afx_msg void OnBrowseStencilErrors();
   //void generateEventXxxxxx();

public:
   int getPcbFileNum();
   BlockStruct* getPcbBlock();
   bool saveStencilSettings(const CString& filePath);
   bool updateStencilSettings(const CString& filePath);
   bool loadStencilSettings(const CString& filePath,bool clearFlag);
   bool loadStencilSettings(const CString& filePath,CStencilSettingEntryMap* entryMap=NULL);
   void clearStencilSettings();
   bool loadStencilSettingsFromDatabase();
   bool updateStencilSettingsInDatabase();

public:
   static CStencilUi& getStencilUi(CCEtoODBDoc& camCadDoc);
   static bool deleteStencilUi();

//protected:
//	DECLARE_MESSAGE_MAP()

};

#endif
