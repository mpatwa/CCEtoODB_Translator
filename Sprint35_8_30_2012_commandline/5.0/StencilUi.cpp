// $Header: /CAMCAD/4.6/StencilUi.cpp 58    6/05/07 3:41p Rick Faltersack $

#include "StdAfx.h"
#include "StencilUi.h" 
#include "StencilDialog.h"
#include "WriteFormat.h" 
#include "Crypt.h" 
#include "StandardApertureDialog.h" 
#include "CCEtoODB.h"
#include "Response.h"
#include "MainFrm.h"
#include "RwUiLib.h"

CWnd* getMainWnd();
bool fileExists(const CString& path);

// Define STENCIL_TIMER in StencilUi.h if you want to enable the timer.
// Should not be enabled in release build.
// Find GetStencilTimer().DumpToFile(filename) and adjust file to suite your situation.
// Timer currently is set up to dump-to-file when you close SG.
#ifdef  STENCIL_TIMER
static CCamcadTimer stencilTimer;
CCamcadTimer &GetStencilTimer() { return stencilTimer; }
#endif

CString stencilUiStateToString(StencilUiStateTag tagValue)
{
   const char* retval = "?";

   switch(tagValue)
   {
   case StencilUiStateEditProperties:      retval = "StencilUiStateEditProperties";      break;
   case StencilUiStateBrowsingErrors:      retval = "StencilUiStateBrowsingErrors";      break;
   case StencilUiStateWaitingForContinue:  retval = "StencilUiStateWaitingForContinue";  break;
   case StencilUiStateUndefined:           retval = "StencilUiStateUndefined";           break;
   }

   return CString(retval);
}

CString stencilStatusToString(StencilStatusTag tagValue)
{
   const char* retval = "Unknown";

   switch(tagValue)
   {
   case StencilStatusPossiblyStale:   retval = "Possibly Stale";  break;
   case StencilStatusStale:           retval = "Stale";           break;
   case StencilStatusUpToDate:        retval = "Up to Date";      break;
   case StencilStatusDoesNotExist:    retval = "Does not exist";  break;
   case StencilStatusStaleUndefined:  retval = "Undefined";       break;
   }

   return CString(retval);
}

/******************************************************************************
* CCEtoODBDoc::OnConnectToStencilSession
*/
void CCEtoODBDoc::OnConnectToStencilSession() 
{
   bool testStandardApertureShapeFlag = false;
   CStandardApertureShape::setDefaultPageUnits(getPageUnits());

   if (testStandardApertureShapeFlag)
   {
      CCamCadDatabase camCadDatabase(*this);
      CStandardApertureDialog dialog(getPageUnits());
      
      dialog.DoModal();
   }
   else
   {
      /*if (!getApp().getCamcadLicense().isLicensed(camcadProductStencilGenerator)) 
      {
         ErrorAccess("You do not have a License for Stencil Generation!");
      }
      else
      {
         CStencilUi::getStencilUi(*this).OnEventConnectToStencilSession();
      }*/
   }
}

void CCEtoODBDoc::OnTerminateStencilSession() 
{
   CStencilUi::getStencilUi(*this).OnEventTerminateStencilSession();
   CStencilUi::deleteStencilUi();

   // Wake up the Navigator (update enabled status)
   // Clicking in the geometry view in SG sometimes causes Navigator to disable, this is to bring it back.
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(this);
}

void CCEtoODBDoc::OnBrowseStencilErrors() 
{
   CStencilUi::getStencilUi(*this).OnEventBrowseStencilErrors();
}

void CCEtoODBDoc::OnTerminateBrowseStencilErrors() 
{
   CStencilUi::getStencilUi(*this).OnEventTerminateBrowseStencilErrors();
}

void CCEtoODBDoc::OnStencilPanZoom() 
{
   CStencilUi::getStencilUi(*this).OnEventStencilPanZoom();
}

void CCEtoODBDoc::OnStencilHidePropertySheet() 
{
   CStencilUi::getStencilUi(*this).OnEventStencilHidePropertySheet();
}

void CCEtoODBDoc::OnModifiedGenerationRule() 
{
   CStencilUi::getStencilUi(*this).OnEventModifiedGenerationRule();
}

void CCEtoODBDoc::OnValidationRulesInSync() 
{
   CStencilUi::getStencilUi(*this).OnEventValidationRulesInSync();
}

void CCEtoODBDoc::OnValidationRulesOutOfSync() 
{
   CStencilUi::getStencilUi(*this).OnEventValidationRulesOutOfSync();
}

void CCEtoODBDoc::OnModifiedValidationRules() 
{
   CStencilUi::getStencilUi(*this).OnEventModifiedValidationRules();
}

void CCEtoODBDoc::OnModifiedGenerationParameters() 
{
   CStencilUi::getStencilUi(*this).OnEventModifiedGenerationParameters();
}

void CCEtoODBDoc::OnStencilRendered() 
{
   CStencilUi::getStencilUi(*this).OnEventStencilRendered();
}

void CCEtoODBDoc::OnValidationPerformed() 
{
   CStencilUi::getStencilUi(*this).OnEventValidationPerformed();
}

void CCEtoODBDoc::OnContinueStencilSession() 
{
   CStencilUi::getStencilUi(*this).OnEventContinueStencilSession();
}

//_____________________________________________________________________________
CStencilSettingEntry::CStencilSettingEntry(int index,const CString& entry,const CString& comment)
: m_index(index)
, m_entry(entry)
, m_comment(comment)
{
   m_hitFlag = false;
}

//_____________________________________________________________________________
CStencilSettingEntryMap::CStencilSettingEntryMap(int size) : 
   m_entries(size)
{
}

void CStencilSettingEntryMap::addEntry(const CString& entryString,const CString& comment)
{
   // pcb <rule>
   // surface top <rule>
   // surface bottom <rule>
   // mount top th <rule>
   // mount top smd <rule>
   // mount bottom th <rule>
   // mount bottom smd <rule>
   // componentGeometry <geometryName> <rule>
   // pad <geometryName>.<pinName> <rule>
   // component <refDes> <rule>
   // pin <pinRef> <rule>

   CStencilEntityRule stencilRule(entryString);
   CString key;

   if (stencilRule.isValid())
   {
      CString entityDesignator = stencilRule.getEntityDesignator();
      key = attributeSourceToString(stencilRule.getEntity()) + (entityDesignator.IsEmpty() ? "" : " " + entityDesignator);
   }

   if (!key.IsEmpty())
   {
      CString normalizedString(entryString);
      normalizedString.Trim();
      CStencilSettingEntry* entry;

      if (m_entries.lookup(key,entry))
      {
         entry->setEntry(normalizedString);
         entry->setComment(comment);
      }
      else
      {
         entry = new CStencilSettingEntry(getCount(),normalizedString,comment);
         m_entries.add(key,entry);
      }
   }
}

//void CStencilSettingEntryMap::hitEntry(const CString& entryString)
//{
//   CStencilSettingEntry* entry = getAt(entryString);
//
//   if (entry != NULL)
//   {
//      entry->setHitFlag(true);
//   }
//}

int CStencilSettingEntryMap::getCount() const
{
   int count = m_entries.getSize();

   return count;
}

CStencilSettingEntry* CStencilSettingEntryMap::getAt(int index)
{
   CStencilSettingEntry* entry = m_entries.getAt(index);

   return entry;
}

CStencilSettingEntry* CStencilSettingEntryMap::getAt(const CString& entryString)
{
   CString normalizedString(entryString);
   normalizedString.Trim();

   CStencilSettingEntry* entry = NULL;

   if (! m_entries.lookup(normalizedString,entry))
   {
      entry = NULL;
   }

   return entry;
}

void CStencilSettingEntryMap::write(CWriteFormat& writeFormat)
{
   for (int index = 0;index < getCount();index++)
   {
      CStencilSettingEntry* entry = getAt(index);

      if (entry != NULL)
      {
         writeFormat.writef("%s\n",entry->getEntry());
      }
   }
}

void CStencilSettingEntryMap::write(CStencilGeneratorSettingsDatabase& stencilGeneratorSettingsDatabase)
{
   stencilGeneratorSettingsDatabase.emptyStencilRules();
   static int callCount = 0;

   callCount++;

   for (int index = 0;index < getCount();index++)
   {
      CStencilSettingEntry* entry = getAt(index);

      if (entry != NULL)
      {
         CStencilEntityRule stencilEntityRule(entry->getEntry());

         CString entity      = attributeSourceToString(stencilEntityRule.getEntity());
         CString designator1 = stencilEntityRule.getEntityDesignator1();
         CString designator2 = stencilEntityRule.getEntityDesignator2();
         CString designator3 = stencilEntityRule.getEntityDesignator3();
         CString designator4 = stencilEntityRule.getEntityDesignator4();
         CString designator5 = stencilEntityRule.getEntityDesignator5();
         CString ruleString  = stencilEntityRule.getRuleString();
         CString comment     = entry->getComment();
         //comment.Format("Comment %d",callCount);

         stencilGeneratorSettingsDatabase.addStencilRule(entity,designator1,designator2,designator3,designator4,designator5,ruleString,comment);
      }
   }

   if (getCount() > 0)
   {
      stencilGeneratorSettingsDatabase.setStencilRules();
   }
}

//_____________________________________________________________________________
CStencilGeneratorSettingsDatabase::CStencilGeneratorSettingsDatabase()
{
}

void CStencilGeneratorSettingsDatabase::setStencilRule(const CString& entity,
   const CString& designator1,const CString& designator2,const CString& designator3,const CString& designator4,const CString& designator5,
   const CString& ruleString)
{
   ::SetStencilRules2Data(entity,designator1,designator2,designator3,designator4,designator5,ruleString);
}

void CStencilGeneratorSettingsDatabase::emptyStencilRules()
{
   m_stencilRulesString.Empty();
}

void CStencilGeneratorSettingsDatabase::addStencilRule(const CString& entity,
   const CString& designator1,const CString& designator2,const CString& designator3,const CString& designator4,const CString& designator5,
   const CString& ruleString,const CString& comment)
{
   m_stencilRulesString += entity       + getFieldSeparator() + 
                           designator1  + getFieldSeparator() + 
                           designator2  + getFieldSeparator() + 
                           designator3  + getFieldSeparator() + 
                           designator4  + getFieldSeparator() + 
                           designator5  + getFieldSeparator() + 
                           ruleString   + getFieldSeparator() + 
                           comment      + getRecordSeparator();
}

void CStencilGeneratorSettingsDatabase::setStencilRules()
{
   ::SetStencilRules2DataSet(m_stencilRulesString);
}

#ifdef DEADCODE
bool CStencilGeneratorSettingsDatabase::getStencilRules(
   CStencilEntityRuleArray& stencilEntityRules,
   const CString& entityString,const CString& designator1,const CString& designator2)
{
   bool retval = false;

   stencilEntityRules.empty();

   CSupString ruleRows,ruleRow;
   CStringArray ruleRowArray,ruleColumns;
   CString entityColumn,designator1Column,designator2Column,designator3Column,designator4Column,designator5Column,ruleColumn,commentColumn;

   ::GetStencilRules2Data(ruleRows,entityString,designator1,designator2);

   ruleRows.Parse(ruleRowArray,getRecordSeparator());

   for (int index = 0;index < ruleRowArray.GetCount();index++)
   {
      ruleRow = ruleRowArray.GetAt(index);
      ruleRow.Parse(ruleColumns,getFieldSeparator());

      if (ruleColumns.GetCount() > 3)
      {
         entityColumn      = ruleColumns.GetAt(0);
         designator1Column = ruleColumns.GetAt(1);
         designator2Column = ruleColumns.GetAt(2);
         designator3Column = ruleColumns.GetAt(3);
         designator4Column = ruleColumns.GetAt(4);
         designator5Column = ruleColumns.GetAt(5);
         ruleColumn        = ruleColumns.GetAt(6);

         if (ruleColumns.GetCount() > 7)
         {
            commentColumn          = ruleColumns.GetAt(7);
         }
         else
         {
            commentColumn.Empty();
         }

         CStencilEntityRule* stencilEntityRule = new CStencilEntityRule(ruleColumn);
         stencilEntityRule->setEntity(stringToAttributeSourceTag(entityColumn),
                                      designator1Column,designator2Column,designator3Column,designator4Column,designator5Column);
         stencilEntityRule->setComment(commentColumn);

         stencilEntityRules.Add(stencilEntityRule);

         retval = true;
      }
   }

   return retval;
}
#endif

void CStencilGeneratorSettingsDatabase::emptyStencilEntities()
{
   m_stencilEntitiesString.Empty();
}

void CStencilGeneratorSettingsDatabase::addStencilEntity(const CString& entity,const CString& designator1,const CString& designator2)
{
   m_stencilEntitiesString += entity       + getFieldSeparator() + 
                              designator1  + getFieldSeparator() +
                              designator2  + getRecordSeparator();
}

bool CStencilGeneratorSettingsDatabase::getStencilEntityRules(bool propagateRules, CStencilEntityRuleArray& stencilEntityRules)
{
   bool retval = false;

   stencilEntityRules.empty();

   CSupString ruleRows,ruleRow;
   CStringArray ruleRowArray,ruleColumns;
   CString entityColumn,designator1Column,designator2Column,designator3Column,designator4Column,designator5Column,ruleColumn,commentColumn;

   ::GetStencilRules2DataSet(ruleRows,m_stencilEntitiesString);

   ruleRows.Parse(ruleRowArray,getRecordSeparator());

   for (int index = 0;index < ruleRowArray.GetCount();index++)
   {
      ruleRow = ruleRowArray.GetAt(index);
      ruleRow.Parse(ruleColumns,getFieldSeparator());

      if (ruleColumns.GetCount() > 3)
      {
         entityColumn      = ruleColumns.GetAt(0);
         designator1Column = ruleColumns.GetAt(1);
         designator2Column = ruleColumns.GetAt(2);
         designator3Column = ruleColumns.GetAt(3);
         designator4Column = ruleColumns.GetAt(4);
         designator5Column = ruleColumns.GetAt(5);
         ruleColumn        = ruleColumns.GetAt(6);
         ruleColumn        = CStencilRule::fixModifiers(ruleColumn);

         if (ruleColumns.GetCount() > 7)
         {
            commentColumn          = ruleColumns.GetAt(7);
         }
         else
         {
            commentColumn.Empty();
         }

         CStencilEntityRule* stencilEntityRule = new CStencilEntityRule(ruleColumn);
         stencilEntityRule->setEntity(stringToAttributeSourceTag(entityColumn),
                                      designator1Column,designator2Column,designator3Column,designator4Column,designator5Column);
         stencilEntityRule->setComment(commentColumn);

         stencilEntityRules.Add(stencilEntityRule);

         retval = true;
      }
   }

   ///if (propagateRules) // propagate rules side-to-side
   if (true) // promote side-specific rules to "both" sides rules
   {
      CStencilEntityRuleArray propagatedTopSideStencilEntityRules;
      CStencilEntityRuleArray propagatedBottomSideStencilEntityRules;
      
      for (int i = 0; i < stencilEntityRules.GetCount(); i++)
      {
         CStencilEntityRule* dbStencilEntityRule = stencilEntityRules.GetAt(i);

         CString des1( dbStencilEntityRule->getEntityDesignator1() );


         // We're using no-case compare here, but in reality case matters. There will be
         // item lookups based on string compare, and all else in stencilgen seems to 
         // be using lower case, so stick to lower case when making settings.

         // If is side-specific rule ...
         if (des1.CompareNoCase("top") == 0 || des1.CompareNoCase("bottom") == 0)
         {
            CString bothSideDes("both");

            CStencilEntityRule bothSideRule( *dbStencilEntityRule );
            bothSideRule.setEntityDesignator1( bothSideDes );

            // If not already a "both" side rule in main body of rules...
            if (stencilEntityRules.FindEntityRule(&bothSideRule) == NULL)
            {
               // Add rule to per-side propagated rule collection
               if (des1.CompareNoCase("top") == 0)
               {
                  propagatedTopSideStencilEntityRules.Add(new CStencilEntityRule(bothSideRule));
               }
               else  // only possibility now is:  if (des1.CompareNoCase("bottom") == 0)
               {
                  propagatedBottomSideStencilEntityRules.Add(new CStencilEntityRule(bothSideRule));
               }
            }
         }
      }

      // Append new rules back to main array, since arrays are containers we need to make copies.
      // Check that such rule is not already present.
      // Do top propagated rules first, this makes migration favor top side rules.
      // Do bottom propagated rules second, this causes use of bottom rule only when there was no top rule.

      // Top
      for (int i = 0; i < propagatedTopSideStencilEntityRules.GetCount(); i++)
      {
         CStencilEntityRule *rule = propagatedTopSideStencilEntityRules.GetAt(i);

         if (rule != NULL && stencilEntityRules.FindEntityRule(rule) == NULL)
            stencilEntityRules.Add( new CStencilEntityRule(*rule) );
      }

      // Bottom
      for (int i = 0; i < propagatedBottomSideStencilEntityRules.GetCount(); i++)
      {
         CStencilEntityRule *rule = propagatedBottomSideStencilEntityRules.GetAt(i);

         if (rule != NULL && stencilEntityRules.FindEntityRule(rule) == NULL)
            stencilEntityRules.Add( new CStencilEntityRule(*rule) );
      }
   }

   return retval;
}

void CStencilGeneratorSettingsDatabase::setStencilSetting(const CString& settingName,const CString& settingValue)
{
   ::SetStencilSetting(settingName,settingValue);
}

void CStencilGeneratorSettingsDatabase::setStencilSetting(const CString& settingName,int settingValue)
{
   CString stringValue;
   stringValue.Format("%d",settingValue);

   ::SetStencilSetting(settingName,stringValue);
}

void CStencilGeneratorSettingsDatabase::setStencilSetting(const CString& settingName,double settingValue)
{
   CString stringValue;
   stringValue.Format("%f",settingValue);

   ::SetStencilSetting(settingName,stringValue);
}

void CStencilGeneratorSettingsDatabase::setStencilSetting(const CString& settingName,double settingValue,PageUnitsTag pageUnits)
{
   CString stringValue;
   stringValue.Format("%f",settingValue);

   if (pageUnits != pageUnitsUndefined)
   {
      stringValue += " " + unitStringAbbreviation(pageUnits);
   }

   ::SetStencilSetting(settingName,stringValue);
}

bool CStencilGeneratorSettingsDatabase::getStencilSetting(CString& settingValue,const CString& settingName)
{
   bool retval = false;

   ::GetStencilSetting(settingValue,settingName);

   return ! settingValue.IsEmpty();
}

bool CStencilGeneratorSettingsDatabase::getStencilSettings(
   CStencilGenerationParameters& stencilGenerationParameters,
   CStencilValidationParameters& stencilValidationParameters)
{
   bool retval = false;

   CSupString settingRows,settingRow;
   CStringArray settingRowArray,settingColumns;
   CString nameColumn,valueColumn;

   ::GetStencilSettings(settingRows);

   settingRows.Parse(settingRowArray,getRecordSeparator());

   for (int index = 0;index < settingRowArray.GetCount();index++)
   {
      settingRow = settingRowArray.GetAt(index);
      settingRow.Parse(settingColumns,getFieldSeparator());

      if (settingColumns.GetCount() > 1)
      {
         nameColumn  = settingColumns.GetAt(0);
         valueColumn = settingColumns.GetAt(1);

         stencilGenerationParameters.readStencilSettings(settingColumns);
         stencilValidationParameters.readStencilSettings(settingColumns);

         retval = true;
      }
   }

   return retval;
}

//_____________________________________________________________________________
CStencilUi* CStencilUi::m_stencilUi = NULL;

CStencilUi::CStencilUi(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
{
   CStencilValidationParameters::resetKeywordIndices();
   CStencilPin::resetKeywordIndices();
   CStencilRuleAttributes::resetKeywordIndices();
   CPadStackInsert::resetKeywordIndices();

   m_camCadDatabase                      = new CCamCadDatabase(camCadDoc);
   m_stencilGenerationParameters         = new CStencilGenerationParameters(*m_camCadDatabase);
   m_renderedStencilGenerationParameters = new CStencilGenerationParameters(*m_camCadDatabase);
   m_stencilValidationParameters         = new CStencilValidationParameters(*m_camCadDatabase);
   m_renderedStencilValidationParameters = new CStencilValidationParameters(*m_camCadDatabase);
   m_stencilGeneratorPropertySheet       = new CStencilGeneratorPropertySheet(*this);
   m_stencilGenerator                    = new CStencilGenerator(*this);
   m_stencilErrorBrowseDialog            = new CStencilErrorBrowseDialog(*this);
   m_stencilWaitDialog                   = new CStencilWaitDialog(*this);

   m_pcbFileNum = -1;
   m_htmlLogFile      = NULL;
   m_logFile          = NULL;

   setState(StencilStateUndefined);
   setUiState(StencilUiStateUndefined);

   CApertureGeometryDialog::setCamCadDatabase(m_camCadDatabase);
   //Create(_T("STATIC"),"StencilUi",WS_CHILD,CRect(0,0,0,0),getMainWnd(),567);
}

CStencilUi::~CStencilUi()
{
   m_stencilErrorBrowseDialog->DestroyWindow();
   m_stencilWaitDialog->DestroyWindow();

   delete m_camCadDatabase;

   delete m_stencilGenerationParameters;
   delete m_renderedStencilGenerationParameters;

   delete m_stencilValidationParameters;
   delete m_renderedStencilValidationParameters;

   delete m_stencilGeneratorPropertySheet;
   delete m_stencilGenerator;
   delete m_stencilErrorBrowseDialog;
   delete m_stencilWaitDialog;

   delete m_logFile;

   //DestroyWindow();
}

CStencilUi& CStencilUi::getStencilUi(CCEtoODBDoc& camCadDoc)
{
   if (m_stencilUi != NULL)
   {
      if (&camCadDoc != &(m_stencilUi->m_camCadDoc))
      {
         deleteStencilUi();
      }
   }

   if (m_stencilUi == NULL)
   {
      m_stencilUi = new CStencilUi(camCadDoc);
   }

   return *m_stencilUi;
}

bool CStencilUi::deleteStencilUi()
{
   bool retval = false;

   if (m_stencilUi != NULL)
   {
      retval = true;

      m_stencilUi->getCamCadDatabase().getCamCadDoc().purgeUnusedWidthsAndBlocks(false);

      delete m_stencilUi;
      m_stencilUi = NULL;
   }

   return retval;
}

BlockStruct* CStencilUi::getPcbBlock()
{
   BlockStruct* pcbBlock = NULL;

   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      FileStruct* pcbFile   = getCamCadDatabase().getFile(pcbFileNum);
      if (pcbFile != NULL)
         pcbBlock = pcbFile->getBlock();
   }

   return pcbBlock;
}

void CStencilUi::setState(StencilStateTag state)
{
   m_state = state;
}

void CStencilUi::setUiState(StencilUiStateTag uiState)
{
   m_uiState = uiState;
}

StencilStatusTag CStencilUi::getGenerationStatus()
{
   StencilStatusTag status;

   switch (getState())
   {
   case StencilStateInitialized:
      status = StencilStatusPossiblyStale;
      break;
   case StencilStateInSync:
      status = StencilStatusUpToDate;
      break;
   case StencilStateGenerationOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateValidationOutOfSync:
      status = StencilStatusUpToDate;
      break;
   case StencilStateGenerationParametersAndValidationOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateGenerationParametersOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateInitializedAndGenerationParametersOutOfSync:
      status = StencilStatusStale;
      break;
   default:
      status = StencilStatusStaleUndefined;
      break;
   }

   return status;
}

StencilStatusTag CStencilUi::getValidationStatus()
{
   StencilStatusTag status;

   switch (getState())
   {
   case StencilStateInitialized:
      status = StencilStatusStale;
      break;
   case StencilStateInSync:
      status = StencilStatusUpToDate;
      break;
   case StencilStateGenerationOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateValidationOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateGenerationParametersAndValidationOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateGenerationParametersOutOfSync:
      status = StencilStatusStale;
      break;
   case StencilStateInitializedAndGenerationParametersOutOfSync:
      status = StencilStatusStale;
      break;
   default:
      status = StencilStatusStaleUndefined;
      break;
   }

   return status;
}

StencilStatusTag CStencilUi::getReportStatus()
{
   StencilStatusTag status;

   if (fileExists(getPinReportFilePath()))
   {
      switch (getState())
      {
      case StencilStateInitialized:
         status = StencilStatusStale;
         break;
      case StencilStateInSync:
         status = StencilStatusUpToDate;
         break;
      case StencilStateGenerationOutOfSync:
         status = StencilStatusStale;
         break;
      case StencilStateValidationOutOfSync:
         status = StencilStatusStale;
         break;
      case StencilStateGenerationParametersAndValidationOutOfSync:
         status = StencilStatusStale;
         break;
      case StencilStateGenerationParametersOutOfSync:
         status = StencilStatusStale;
         break;
      case StencilStateInitializedAndGenerationParametersOutOfSync:
         status = StencilStatusStale;
         break;
      default:
         status = StencilStatusStaleUndefined;
         break;
      }
   }
   else
   {
      status = StencilStatusDoesNotExist;
   }

   return status;
}

int CStencilUi::getValidationErrorCount()
{
   int errorCount;

   switch (getState())
   {
   case StencilStateInitialized:
      errorCount = -1;
      break;
   case StencilStateInSync:
      errorCount = getStencilGenerator().getStencilViolations().GetCount();
      break;
   case StencilStateGenerationOutOfSync:
      errorCount = -1;
      break;
   case StencilStateValidationOutOfSync:
      errorCount = -1;
      break;
   case StencilStateGenerationParametersAndValidationOutOfSync:
      errorCount = -1;
      break;
   case StencilStateGenerationParametersOutOfSync:
      errorCount = -1;
      break;
   case StencilStateInitializedAndGenerationParametersOutOfSync:
      errorCount = -1;
      break;
   default:
      errorCount = -1;
      break;
   }

   return errorCount;
}

CStencilViolations& CStencilUi::getStencilViolations()
{ 
   return getStencilGenerator().getStencilViolations(); 
}

CStencilGenerator& CStencilUi::getStencilGenerator()
{ 
   if (! m_stencilGenerator->getInitializedFlag())
   {
#ifdef STENCIL_TIMER
      GetStencilTimer().AddMessage("enterINIT CStencilUi::getStencilGenerator()");
#endif
      int pcbFileNum = getPcbFileNum();

      if (pcbFileNum >= 0)
      {
         COperationProgress progress;
         // Case dts0100452294
         // The progress bar constructor above will flush and dispatch events in the queue, which may
         // result in some other stencil generator event handler being called that ends up
         // initializing the stencil entities before we get to the call right below here.
         // Yes, events may be processed out of order. Those progress bars are dangerous!
         // So to keep from calling the initializer twice we need a second check of
         // the initializeFlag after the progress bar construction.
         if (! m_stencilGenerator->getInitializedFlag())
            m_stencilGenerator->initializeStencilEntities(pcbFileNum, &progress);

#ifdef STENCIL_TIMER
         GetStencilTimer().AddMessage("exitINIT CStencilUi::getStencilGenerator()");
#endif
      }
   }

   return *m_stencilGenerator; 
}

int CStencilUi::getPcbFileNum()
{
   CCEtoODBDoc* camCadDoc = getActiveView()->GetDocument();
   const char* errorMessage = "";

   if (camCadDoc != &m_camCadDoc)
   {
      errorMessage = "Document changed, stencil session terminated";
      m_pcbFileNum = -1;
   }
   else
   {
      FileStruct* pcbFileStruct = getCamCadDatabase().getSingleVisiblePcb();

      if (pcbFileStruct == NULL)
      {
         errorMessage = "One and only one board must be visible, stencil session terminated";
         m_pcbFileNum = -1;
      }
      else if (m_pcbFileNum < 0)
      {
         m_pcbFileNum = pcbFileStruct->getFileNumber();

         CStencilPin::registerStencilKeywords(getCamCadDatabase(),getLogFile());
         CStencilRuleAttributes::registerStencilKeywords(getCamCadDatabase(),getLogFile());
         CStencilValidationParameters::registerStencilValidationKeywords(getCamCadDatabase(),getLogFile());
      }
      else if (m_pcbFileNum != pcbFileStruct->getFileNumber())
      {
         errorMessage = "CAMCAD File changed, stencil session terminated";
         m_pcbFileNum = -1;
      }
   }

   if (m_pcbFileNum < 0)
   {
      formatMessageBox(MB_ICONERROR,errorMessage);
      generateEvent(ID_StencilCommand_TerminateStencilSession);
   }

   return m_pcbFileNum;
}

CWriteFormat& CStencilUi::getLogFile()
{
   if (m_logFile == NULL)
   {
      CFilePath logFilePath(getLogFilePath());

      m_htmlLogFile = new CHtmlFileWriteFormat(2048);

      if (! m_htmlLogFile->open(logFilePath.getPath()))
      {
         formatMessageBox("Could not open the log file '%s'",(const char*)logFilePath.getPath());

         delete m_htmlLogFile;
         m_htmlLogFile = NULL;

         m_logFile = new CNullWriteFormat();
      }
      else
      {
         m_logFile = m_htmlLogFile;
      }
   }

   return *m_logFile;
}

CString CStencilUi::getLogFilePath()
{
   // Log file locations standardized throughout CAMCAD for 4.7
   return GetLogfilePath("StencilGeneratorLog.htm");
}

CString CStencilUi::getPinReportFilePath()
{
   // Log file locations standardized throughout CAMCAD for 4.7
   return GetLogfilePath("StencilPinReport.htm");
}

void CStencilUi::showStencilErrorBrowseDialog()
{
   if (! ::IsWindow(getStencilErrorBrowseDialog()))
   {
      getStencilErrorBrowseDialog().Create(IDD_BrowseStencilErrors);
   }

   getStencilErrorBrowseDialog().ShowWindow(SW_SHOW);
   getStencilErrorBrowseDialog().UpdateWindow();
}

void CStencilUi::hideStencilErrorBrowseDialog()
{
   if (::IsWindow(getStencilErrorBrowseDialog()))
   {
      getStencilErrorBrowseDialog().ShowWindow(SW_HIDE);
   }
}

void CStencilUi::showStencilWaitDialog()
{
   if (! ::IsWindow(getStencilWaitDialog()))
   {
      getStencilWaitDialog().Create(IDD_StencilWaitDialog);
   }

   getStencilWaitDialog().ShowWindow(SW_SHOW);
   getStencilWaitDialog().UpdateWindow();
}

void CStencilUi::hideStencilWaitDialog()
{
   if (::IsWindow(getStencilWaitDialog()))
   {
      getStencilWaitDialog().ShowWindow(SW_HIDE);
   }
}

void CStencilUi::showStencilMicrocosmFile()
{
   if (m_stencilGenerator != NULL && m_stencilGenerator->getInitializedFlag())
   {
      getStencilGenerator().showStencilMicrocosmFile();
   }
}

void CStencilUi::hideStencilMicrocosmFile()
{
   if (m_stencilGenerator != NULL && m_stencilGenerator->getInitializedFlag())
   {
      getStencilGenerator().hideStencilMicrocosmFile();
   }
}

void CStencilUi::deleteLogFile()
{
   if (m_logFile != m_htmlLogFile)
   {
      delete m_logFile;
   }

   delete m_htmlLogFile;

   m_htmlLogFile = NULL;      
   m_logFile     = NULL;   

   CString logFilePath(getLogFilePath());

   _unlink(logFilePath);
}

void CStencilUi::renderStencils()
{
   if (formatMessageBox(MB_ICONQUESTION | MB_YESNO,
         "Existing Stencil layers will be overwritten.  Is this OK?") == IDYES)
   {
      CHtmlFileWriteFormat* reportFile = NULL;

      deleteLogFile();

      CString reportFilePath = getPinReportFilePath();

      reportFile = new CHtmlFileWriteFormat(2048);

      if (! reportFile->open(reportFilePath))
      {
         delete reportFile;
         reportFile = NULL;
      }

      CStencilValidationParameters::registerStencilValidationKeywords(getCamCadDatabase(),getLogFile());

      //getStencilGenerationParameters() = getStencilGeneratorPropertySheet().getStencilGenerationParameters();
      getStencilGenerationParameters().writeAttributes(getPcbFileNum());
      getRenderedStencilGenerationParameters() = getStencilGenerationParameters();

      //getStencilValidationParameters() = getStencilGeneratorPropertySheet().getStencilValidationParameters();
      getStencilValidationParameters().writeAttributes(getPcbFileNum());
      getRenderedStencilValidationParameters() = getStencilValidationParameters();

      getStencilErrorBrowseDialog().empty();
      getStencilGenerator().renderStencils(getStencilGenerationParameters(),getStencilValidationParameters(),
         getLogFile(),reportFile);

      //getStencilGeneratorPropertySheet().reloadStencilHoles();
      generateEvent(ID_StencilCommand_StencilRendered);
      getCamCadDatabase().getCamCadDoc().OnRedraw();

      if (getHtmlLogFile() != NULL)
      {
         getHtmlLogFile()->flush();

         if (getStencilGeneratorPropertySheet().getAutoOpenLogFile())
         {
            ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",getHtmlLogFile()->getFilePath(),NULL,NULL,SW_SHOW);
         }
      }

      delete reportFile;
   }
}

void CStencilUi::performStencilValidationChecks()
{
   CStencilValidationParameters::registerStencilValidationKeywords(getCamCadDatabase(),getLogFile());

   getStencilValidationParameters() = getStencilGeneratorPropertySheet().getStencilValidationParameters();
   getStencilValidationParameters().writeAttributes(getPcbFileNum());

   getStencilErrorBrowseDialog().empty();
   getStencilGenerator().performStencilValidationChecks(getStencilValidationParameters(),getStencilGenerationParameters());
   generateEvent(ID_StencilCommand_ValidationPerformed);
}

void CStencilUi::updateStatus()
{
   if (::IsWindow(getStencilGeneratorPropertySheet()))
   {
      getStencilGeneratorPropertySheet().updateStatus();
   }
}

bool CStencilUi::saveStencilSettings(const CString& filePath)
{
   bool retval = false;
   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      CStdioFileWriteFormat settingsFile;

      if (settingsFile.open(filePath))
      {
         m_stencilGenerationParameters->readAttributes(pcbFileNum);
         m_stencilValidationParameters->readAttributes(pcbFileNum);

         m_stencilGenerationParameters->writeStencilSettings(settingsFile);
         m_stencilValidationParameters->writeStencilSettings(settingsFile);

         CStencilSettingEntryMap stencilSettingEntryMap(nextPrime2n(1000));
         getStencilGenerator().writeStencilSettings(settingsFile,stencilSettingEntryMap);

         retval = true;
      }
   }

   return retval;
}

bool CStencilUi::updateStencilSettings(const CString& filePath)
{
   bool retval = false;
   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      CStencilSettingEntryMap stencilSettingEntryMap(nextPrime2n(1000));
      CStdioFileWriteFormat settingsFile;

      loadStencilSettings(filePath,&stencilSettingEntryMap);

      if (settingsFile.open(filePath))
      {
         m_stencilGenerationParameters->readAttributes(pcbFileNum);
         m_stencilValidationParameters->readAttributes(pcbFileNum);

         m_stencilGenerationParameters->writeStencilSettings(settingsFile);
         m_stencilValidationParameters->writeStencilSettings(settingsFile);
         getStencilGenerator().writeStencilSettings(settingsFile,stencilSettingEntryMap);

         retval = true;
      }
   }

   return retval;
}

bool CStencilUi::updateStencilSettingsInDatabase()
{
   bool retval = false;
   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      migrateStencilData2();

      CStencilGeneratorSettingsDatabase stencilGeneratorSettingsDatabase;

      m_stencilGenerationParameters->readAttributes(pcbFileNum);
      m_stencilValidationParameters->readAttributes(pcbFileNum);

      m_stencilGenerationParameters->writeStencilSettings(stencilGeneratorSettingsDatabase);
      m_stencilValidationParameters->writeStencilSettings(stencilGeneratorSettingsDatabase);

      getStencilGenerator().writeStencilSettings(stencilGeneratorSettingsDatabase);

      retval = true;
   }

   return retval;
}

bool CStencilUi::loadStencilSettings(const CString& filePath,bool clearFlag)
{
   if (clearFlag)
   {
      clearStencilSettings();
   }

   return loadStencilSettings(filePath);
}

bool CStencilUi::loadStencilSettings(const CString& filePath,CStencilSettingEntryMap* entryMap)
{
   bool retval = false;

   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      CStdioFile settingsFile;

      if (settingsFile.Open(filePath,CFile::modeRead))
      {
         CStencilValidationParameters::registerStencilValidationKeywords(getCamCadDatabase(),getLogFile());
         CSupString line;
         CStringArray params;
         int numpar;

         enum 
         { 
            StencilGenerationParameters , 
            StencilValidationParameters , 
            StencilGenerationRules, 
            Undefined 
         } state = Undefined;

         while (settingsFile.ReadString(line))
         {
            numpar = line.ParseQuote(params," ");

            if (numpar > 0)
            {
               if (numpar == 1)
               {
                  CString param = params[0];

                  if (param.CompareNoCase("[StencilGenerationParameters]") == 0)
                  {
                     state = StencilGenerationParameters;
                  }
                  else if (param.CompareNoCase("[StencilValidationParameters]") == 0)
                  {
                     state = StencilValidationParameters;
                  }
                  else if (param.CompareNoCase("[StencilGenerationRules]") == 0)
                  {
                     state = StencilGenerationRules;
                  }
               }
               else if (entryMap != NULL)
               {
                  if (state == StencilGenerationRules)
                  {
                     entryMap->addEntry(line);
                  }
               }
               else
               {
                  switch (state)
                  {
                  case StencilGenerationParameters:
                     m_stencilGenerationParameters->readStencilSettings(params);
                     break;
                  case StencilValidationParameters:
                     m_stencilValidationParameters->readStencilSettings(params);
                     break;
                  case StencilGenerationRules:
                     // fix modifiers from old versions of CamCad that did not quote the values
                     line = CStencilRule::fixModifiers(line);
                     numpar = line.ParseQuote(params," ");

                     if (numpar > 1)
                     {
                        CStringArray quotedParams;
                        line.ParseQuotePreserveQuotes(quotedParams," ");
                        params.SetAt(1,quotedParams.GetAt(1));
                     }

                     getStencilGenerator().readStencilSettings(params);
                     break;
                  }
               }
            }
         }

         if (entryMap == NULL)
         {
            m_stencilGenerationParameters->writeAttributes(pcbFileNum);
            m_stencilValidationParameters->writeAttributes(pcbFileNum);
         }

         retval = true;
      }
   }

   getStencilGeneratorPropertySheet().updateData(false);

   return retval;
}

bool CStencilUi::loadStencilSettingsFromDatabase()
{
   migrateStencilData2();

   getStencilGenerator().instantiateStencilVessels();

   CStencilGeneratorSettingsDatabase stencilGeneratorSettingsDatabase;
   COperationProgress operationProgress;
   operationProgress.updateStatus("Loading Stencil Settings",1.);

   bool retval = getStencilGenerator().readStencilSettings(stencilGeneratorSettingsDatabase,&operationProgress);

   return retval;
}

void CStencilUi::clearStencilSettings()
{
   int pcbFileNum = getPcbFileNum();

   if (pcbFileNum >= 0)
   {
      FileStruct* fileStruct = getCamCadDatabase().getFile(pcbFileNum);

      m_stencilGenerationParameters->clearStencilSettings(fileStruct);
      m_stencilValidationParameters->clearStencilSettings(fileStruct);
      getStencilGenerator().clearStencilSettings();
   }
}

//void CStencilUi::generateEventTerminateStencilSession()
//{
//   getActiveView()->PostMessage(WM_COMMAND,ID_StencilCommand_TerminateStencilSession,0);
//}

void CStencilUi::reportStencilUiStateError(const char* routine)
{
   formatMessageBox("Unexpected state '%s' encountered in '%s'",
      (const char*)stencilUiStateToString(getUiState()),routine);
}

void CStencilUi::generateEvent(int commandId)
{
   getActiveView()->PostMessage(WM_COMMAND,commandId,0);
}

void CStencilUi::OnEventConnectToStencilSession() 
{
#ifdef STENCIL_TIMER
   // For timer to work reasonably must limit to single CPU, otherwise running
   // clock and therefore time deltas get whacked when CPU switches.
   DWORD prevAffinity = SetThreadAffinityMask(GetCurrentThread(), (ULONG)1);
   GetStencilTimer().SetEnabled(true);
   GetStencilTimer().AddMessage("CStencilUi::OnEventConnectToStencilSession()");
#endif

   // clear selections, stencil gen may alter geometries, camcad can crash 
   // when still referencing obsolete geometries in select stack when SG is finished
   getCamCadDatabase().getCamCadDoc().UnselectAll(FALSE);

   switch (getUiState())
   {
   case StencilUiStateBrowsingErrors:
      showStencilErrorBrowseDialog();
      break;
   case StencilUiStateWaitingForContinue:
      hideStencilWaitDialog();
      showStencilErrorBrowseDialog();
      break;
   case StencilUiStateWaitingForContinueEdit:
      OnEventContinueStencilSession();
      break;
   case StencilUiStateEditProperties:
   case StencilUiStateUndefined:
      {
         int pcbFileNum = getPcbFileNum();

         if (pcbFileNum >= 0)
         {
            // dts0100594073 - Create Pin Pitch automatically on starting Stencil Generator
            getCamCadDatabase().getCamCadDoc().OnGeneratePinPitch();

            setUiState(StencilUiStateEditProperties);

            if (getUiState() == StencilUiStateUndefined ||
                getState()   == StencilStateUndefined      )
            {
               setState(StencilStateInitialized);
            }

            getStencilGenerationParameters().readAttributes(pcbFileNum);
            //getStencilGeneratorPropertySheet().getStencilGenerationParameters() = 
            //   getStencilGenerationParameters();

            getStencilValidationParameters().readAttributes(pcbFileNum);
            //getStencilGeneratorPropertySheet().getStencilValidationParameters() = 
            //   getStencilValidationParameters();

            int status = getStencilGeneratorPropertySheet().DoModal();

            if (status == BrowseValidationErrors)
            {
               getStencilGeneratorPropertySheet().ShowWindow(SW_HIDE);
               //getMainWnd()->PostMessage(WM_COMMAND,ID_StencilCommand_BrowseStencilErrors);
               generateEvent(ID_StencilCommand_BrowseStencilErrors);
               //getStencilErrorBrowseDialog().ShowWindow(SW_SHOW);
            }
            else if (status == HidePropertySheet)
            {
               getStencilGeneratorPropertySheet().ShowWindow(SW_HIDE);
               generateEvent(ID_StencilCommand_HidePropertySheet);
            }
            else
            {
               generateEvent(ID_StencilCommand_TerminateStencilSession);
            }
         }
      }

      break;
   }

}

void CStencilUi::OnEventTerminateStencilSession()
{
   hideStencilMicrocosmFile();

#ifdef STENCIL_TIMER
   GetStencilTimer().DumpToFile("C:\\Development\\Cases\\558605 Stencil Gen SLOW\\timerReport.txt");
   ///GetStencilTimer().Reset();   Don't reset if you want to time multiple runs in same session
#endif
}

void CStencilUi::OnEventBrowseStencilErrors()
{
   if (getUiState() == StencilUiStateEditProperties)
   {
      setUiState(StencilUiStateBrowsingErrors);
      showStencilErrorBrowseDialog();
   }
   else
   {
      reportStencilUiStateError("CStencilUi::OnEventBrowseStencilErrors()");
   }
}

void CStencilUi::OnEventTerminateBrowseStencilErrors()
{
   if (getUiState() == StencilUiStateBrowsingErrors)
   {
      setUiState(StencilUiStateEditProperties);
      generateEvent(ID_StencilCommand_ConnectToStencilSession);
   }
   else
   {
      reportStencilUiStateError("CStencilUi::OnEventTerminateBrowseStencilErrors()");
   }
}

void CStencilUi::OnEventStencilPanZoom()
{
   if (getUiState() == StencilUiStateBrowsingErrors)
   {
      setUiState(StencilUiStateWaitingForContinue);
      hideStencilErrorBrowseDialog();
      showStencilWaitDialog();
   }
   else
   {
      reportStencilUiStateError("CStencilUi::OnEventStencilPanZoom()");
   }
}

void CStencilUi::OnEventStencilHidePropertySheet()
{
   if (getUiState() == StencilUiStateEditProperties)
   {
      hideStencilMicrocosmFile();
      setUiState(StencilUiStateWaitingForContinueEdit);
      showStencilWaitDialog();

      // Wake up the Navigator (update enabled status)
      // Clicking in the geometry view in SG sometimes causes Navigator to disable, this is to bring it back.
	   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
      frame->getNavigator().setDoc( &(this->getCamCadDatabase().getCamCadDoc()) );
   }
   else
   {
      reportStencilUiStateError("CStencilUi::OnEventStencilHidePropertySheet()");
   }
}

void CStencilUi::OnEventModifiedGenerationRule()
{
   setState(StencilStateGenerationOutOfSync);  

   updateStatus();
}

void CStencilUi::OnEventValidationRulesInSync()
{
   // not used
}

void CStencilUi::OnEventValidationRulesOutOfSync()
{
   // not used
}

void CStencilUi::OnEventModifiedValidationRules()
{
   //CStencilValidationParameters& localValidationParameters = 
   //   getStencilGeneratorPropertySheet().getStencilValidationParameters();

   bool parametersInSyncFlag = 
      getStencilValidationParameters().allParametersEqual(getRenderedStencilValidationParameters());

   switch (getState())
   {
   case StencilStateValidationOutOfSync:
   case StencilStateInSync:
      if (parametersInSyncFlag)
      {
         setState(StencilStateInSync);
      }
      else
      {
         setState(StencilStateValidationOutOfSync);
      }

      break;
   case StencilStateGenerationParametersAndValidationOutOfSync:
   case StencilStateGenerationParametersOutOfSync:
      if (parametersInSyncFlag)
      {
         setState(StencilStateGenerationParametersOutOfSync);
      }
      else
      {
         setState(StencilStateGenerationParametersAndValidationOutOfSync);
      }

      break;
   }

   updateStatus();
}

void CStencilUi::OnEventModifiedGenerationParameters()
{
   //CStencilGenerationParameters& localGenerationParameters = 
   //   getStencilGeneratorPropertySheet().getStencilGenerationParameters();

   bool parametersInSyncFlag = 
      getStencilGenerationParameters().allParametersEqual(getRenderedStencilGenerationParameters());

   switch (getState())
   {
   case StencilStateGenerationParametersOutOfSync:
   case StencilStateInSync:
      if (parametersInSyncFlag)
      {
         setState(StencilStateInSync);
      }
      else
      {
         setState(StencilStateGenerationParametersOutOfSync);
      }

      break;
   case StencilStateGenerationParametersAndValidationOutOfSync:
   case StencilStateValidationOutOfSync:
      if (parametersInSyncFlag)
      {
         setState(StencilStateValidationOutOfSync);
      }
      else
      {
         setState(StencilStateGenerationParametersAndValidationOutOfSync);
      }

      break;
   case StencilStateInitialized:
   case StencilStateInitializedAndGenerationParametersOutOfSync:
      if (parametersInSyncFlag)
      {
         setState(StencilStateInitialized);
      }
      else
      {
         setState(StencilStateInitializedAndGenerationParametersOutOfSync);
      }

      break;
   }

   updateStatus();
}

void CStencilUi::OnEventStencilRendered()
{
   setState(StencilStateInSync);  

   updateStatus();
}

void CStencilUi::OnEventValidationPerformed()
{
   if (getState() == StencilStateValidationOutOfSync)
   {
      setState(StencilStateInSync);  
   }

   updateStatus();
}

void CStencilUi::OnEventContinueStencilSession()
{
   if (getUiState() == StencilUiStateWaitingForContinue)
   {
      setUiState(StencilUiStateBrowsingErrors);
      showStencilMicrocosmFile();
      hideStencilWaitDialog();
      showStencilErrorBrowseDialog();
   }
   else if (getUiState() == StencilUiStateWaitingForContinueEdit)
   {
      setUiState(StencilUiStateEditProperties);
      showStencilMicrocosmFile();
      hideStencilWaitDialog();
      generateEvent(ID_StencilCommand_ConnectToStencilSession);
   }
   else
   {
      reportStencilUiStateError("CStencilUi::OnEventContinueStencilSession()");
   }
}

//BEGIN_MESSAGE_MAP(CStencilUi,CWnd)
//   ON_COMMAND(ID_StencilCommand_BrowseStencilErrors,OnBrowseStencilErrors)
//END_MESSAGE_MAP()
//
//
//// CStencilGeneratorPropertySheet message handlers
//void CStencilUi::OnBrowseStencilErrors() 
//{
//   getStencilErrorBrowseDialog().ShowWindow(SW_SHOW);
//}





