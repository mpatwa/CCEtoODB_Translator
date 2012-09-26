// $Header: /CAMCAD/4.4/TA.cpp 15    3/29/04 8:38a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ta.h"
#include "graph.h"
#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PinkooArray pinkooarray;
int pinkoocnt;

ViakooArray viakooarray;
int viakoocnt;

FILE *taLog;
int taDisplayError;

int SHOWOUTLINE = TRUE;

CString T_OUTLINE_TOP = "T_OUTLINE_TOP";
CString T_OUTLINE_BOT = "T_OUTLINE_BOT";

/******************************************************************************
* DeleteTestAccessData
*/
void DeleteTestAccessData(CCEtoODBDoc *doc, CDataList *DataList)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
   
      Attrib *attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_TESTACCESS, 0);
      if (attrib)
         RemoveAttrib(doc->IsKeyWord(ATT_TESTACCESS, TRUE), &data->getAttributesRef());

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_TEST_ACCESSPOINT)
            RemoveOneEntityFromDataList(doc, DataList, data);
   }
}

/*************************************************************************
* DeleteTestAccess
*/
void TA_DeleteTestAccess(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   if (ErrorMessage("Warning - This function can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nDo you wish to continue?", "Delete Test Access Info", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      doc->UnselectAll(TRUE);

      // BeginWaitCursor();
      // need to make a component outline for every component
      DeleteTestAccessData(doc, &(pcbfile->getBlock()->getDataList())); 
      // delete test access in the netlist
      DeleteTestAccessNetlist(doc, &(pcbfile->getNetList()));
      doc->UpdateAllViews(NULL);
   }
}

/******************************************************************************
* DeleteTestAccessNetlist
*/
void DeleteTestAccessNetlist(CCEtoODBDoc *doc, CNetList *NetList)
{
   int testAccessKeywordIndex     = doc->IsKeyWord(ATT_TESTACCESS,1);
   int testAccessFailKeywordIndex = doc->IsKeyWord(ATT_TESTACCESS_FAIL,1);
   
   for (POSITION netPos = NetList->GetHeadPosition();netPos != NULL;)
   {
      NetStruct *net = NetList->GetNext(netPos);
      
      for (POSITION compPinPos = net->getHeadCompPinPosition();compPinPos != NULL;)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         RemoveAttrib(testAccessKeywordIndex    ,&(compPin->getAttributesRef()));
         RemoveAttrib(testAccessFailKeywordIndex,&(compPin->getAttributesRef()));
      }
   }
}

/******************************************************************************
* TA_Percent
   make sure it nevers end up 100 through rounding if not equal
*/
void TA_Percent(CString *vx, int v1, int v2)
{
   CString  v;

   if (v2 == 0)
   {
      v = "0.0%";
   }
   else
   {
      v.Format("%1.1lf%%", 100.0*v1/v2);
      if (v1 != v2)
      {
         if (strlen(v) > 3 && !STRNICMP(v, "100", 3))
            v = "99.9%";
      }
   }

   *vx = v;
}

/******************************************************************************
* get_pinkoocnt_entity
*/
int get_pinkoocnt_entity( long entity)
{
   for (int i=0; i<pinkoocnt; i++)
   {
      if (pinkooarray[i]->entitynumber == entity)
         return i;
   }

   return -1;
}

/******************************************************************************
* TAload_defaultsettings
*/
void TAload_defaultsettings(CCEtoODBDoc *doc, const char *fname, TA_OptionsStruct *defaultsetting, int pageunits)
{
   FILE     *fp;
   CString  tmp;
   char     line[255];
   char     *lp;
   int      u;

   // initialize structure
   defaultsetting->Unusedpins = TRUE;
   defaultsetting->MaxProbeSize = 0.2 * Units_Factor(UNIT_INCHES, pageunits);

   defaultsetting->AccessAnalysis.throughPinAccess = 0;
   defaultsetting->AccessAnalysis.excludeSinglePin = TRUE;
   defaultsetting->AccessAnalysis.soldermaskTop = FALSE;
   defaultsetting->AccessAnalysis.soldermaskBot = FALSE;

   defaultsetting->AccessAnalysis.testGraphicSize = 0.03 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.allowNetAccessTop = TRUE;
   defaultsetting->AccessAnalysis.allowNetAccessBot= TRUE;
   defaultsetting->AccessAnalysis.preferNetAccessSurface = 1;
   defaultsetting->AccessAnalysis.boardOutlineDistanceTop = 0.1 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.boardOutlineDistanceBot = 0.1 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.compOutlineDistanceTop = 0.01 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.compOutlineDistanceBot = 0.01 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.featureSizeMinTop = 0.04 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->AccessAnalysis.featureSizeMinBot = 0.04 * Units_Factor(UNIT_INCHES, pageunits);


   defaultsetting->AccessAnalysis.boardFeatureBot = FALSE;
   defaultsetting->AccessAnalysis.boardFeatureTop = FALSE;
   defaultsetting->AccessAnalysis.compFeatureBot = FALSE;
   defaultsetting->AccessAnalysis.compFeatureTop = FALSE;
   defaultsetting->AccessAnalysis.featureSizeBot = FALSE;
   defaultsetting->AccessAnalysis.featureSizeTop = FALSE;

   defaultsetting->AccessAnalysis.topTestattribPriority = 1;
   defaultsetting->AccessAnalysis.topConnectorPriority = 2;
   defaultsetting->AccessAnalysis.topThroughholePriority = 3;
   defaultsetting->AccessAnalysis.topViaPriority = 4;
   defaultsetting->AccessAnalysis.topSMDPriority = 5;

   defaultsetting->AccessAnalysis.MFNtopTestattrib = TRUE;
   defaultsetting->AccessAnalysis.MFNtopConnector = TRUE;
   defaultsetting->AccessAnalysis.MFNtopThroughhole = TRUE;
   defaultsetting->AccessAnalysis.MFNtopSMD = TRUE;
   defaultsetting->AccessAnalysis.MFNtopVia = TRUE;

   defaultsetting->AccessAnalysis.SFNtopTestattrib = TRUE;
   defaultsetting->AccessAnalysis.SFNtopConnector = TRUE;
   defaultsetting->AccessAnalysis.SFNtopThroughhole = TRUE;
   defaultsetting->AccessAnalysis.SFNtopSMD = TRUE;
   defaultsetting->AccessAnalysis.SFNtopVia = TRUE;

   defaultsetting->AccessAnalysis.botTestattribPriority = 1;
   defaultsetting->AccessAnalysis.botConnectorPriority = 2;
   defaultsetting->AccessAnalysis.botThroughholePriority = 3;
   defaultsetting->AccessAnalysis.botViaPriority = 4;
   defaultsetting->AccessAnalysis.botSMDPriority = 5;

   defaultsetting->AccessAnalysis.MFNbotTestattrib = TRUE;
   defaultsetting->AccessAnalysis.MFNbotConnector = TRUE;
   defaultsetting->AccessAnalysis.MFNbotThroughhole = TRUE;
   defaultsetting->AccessAnalysis.MFNbotSMD = TRUE;
   defaultsetting->AccessAnalysis.MFNbotVia = TRUE;

   defaultsetting->AccessAnalysis.SFNbotTestattrib = TRUE;
   defaultsetting->AccessAnalysis.SFNbotConnector = TRUE;
   defaultsetting->AccessAnalysis.SFNbotThroughhole = TRUE;
   defaultsetting->AccessAnalysis.SFNbotSMD = TRUE;
   defaultsetting->AccessAnalysis.SFNbotVia = TRUE;

   defaultsetting->ProbeAssignment.top = TRUE;
   defaultsetting->ProbeAssignment.bottom = TRUE;

   defaultsetting->ProbableShorts.pin_pin = TRUE;
   defaultsetting->ProbableShorts.pin_pinSize = 0.025 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->ProbableShorts.pin_feature = TRUE;
   defaultsetting->ProbableShorts.pin_featureSize = 0.025 * Units_Factor(UNIT_INCHES, pageunits);
   defaultsetting->ProbableShorts.feature_feature = TRUE;
   defaultsetting->ProbableShorts.feature_featureSize = 0.025 * Units_Factor(UNIT_INCHES, pageunits);

   if ((fp = fopen(fname,"rt")) != NULL)
   {
      int version = 1;
      while (fgets(line,255,fp))
      {
         if (!strncmp(line, ".VERSION 2", 10))
         {
            version = 2;
            break;
         }
      }

      if (version < 2)
         ErrorMessage(fname, "You need to save a new .dft file");


      while (fgets(line,255,fp))
      {
         tmp = line;
         if ((lp = strtok(line," \t\n")) == NULL)
            continue;

         if (lp[0] != '.')
            continue;

         if (!STRCMPI(lp,".SHOW_OUTLINE"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
            SHOWOUTLINE = atoi(lp);
         }
         else if (!STRCMPI(lp, ".TOP_OUTLINE"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
            T_OUTLINE_TOP = lp;
         }
         else if (!STRCMPI(lp, ".BOT_OUTLINE"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
            T_OUTLINE_BOT = lp;
         }
         else if (!STRCMPI(lp, ".Unusedpins"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->Unusedpins = atoi(lp);
         }
         else if (!STRCMPI(lp, ".MaxProbeSize"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
            double probeSize = atof(lp);
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if ((u =  GetUnitIndex(lp)) > -1)
               defaultsetting->MaxProbeSize = probeSize * Units_Factor(u, pageunits);
         }
         else if (!STRCMPI(lp, ".AccessAnalysis"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)
               continue;

            if (!STRCMPI(lp, "soldermaskTop") || !STRCMPI(lp, "topsoldermask"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.soldermaskTop = atoi(lp);
            }
            else if (!STRCMPI(lp, "soldermaskBot") || !STRCMPI(lp, "soldermaskbottom") || !STRCMPI(lp, "bottomSoldermask"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.soldermaskBot = atoi(lp);
            }
            else if (!STRCMPI(lp,"testGraphicSize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double testGraphicSize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.testGraphicSize = testGraphicSize * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp,"netaccesstop"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.allowNetAccessTop = atoi(lp);
            }
            else if (!STRCMPI(lp,"netaccessbottom"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.allowNetAccessBot = atoi(lp);
            }
            else if (!STRCMPI(lp, "preferNetAccessSurface") || !STRCMPI(lp, "preferredtestsurface"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.preferNetAccessSurface = atoi(lp);
            }
            else if (!STRCMPI(lp,"boardOutlineDistanceTop"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.boardOutlineDistanceTop = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp,"boardOutlineDistanceBot") || !STRCMPI(lp,"boardOutlineDistanceBottom"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.boardOutlineDistanceBot = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp, "compOutlineDistanceTop") || !STRCMPI(lp,"componentoutlinedistancetop"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.compOutlineDistanceTop = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp, "compOutlineDistanceBot") || !STRCMPI(lp, "componentoutlinedistancebottom"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.compOutlineDistanceBot = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp, "minfeaturesizetop"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.featureSizeMinTop = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp, "minfeaturesizebottom"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double dist = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->AccessAnalysis.featureSizeMinBot = dist * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp, "boardFeatureBot") || !STRCMPI(lp, "bottomboardfeature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.boardFeatureBot = atoi(lp);
            }
            else if (!STRCMPI(lp, "boardFeatureTop") || !STRCMPI(lp, "topboardfeature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.boardFeatureTop = atoi(lp);
            }
            else if (!STRCMPI(lp, "compFeatureBot") || !STRCMPI(lp, "bottomcomponentfeature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.compFeatureBot = atoi(lp);
            }
            else if (!STRCMPI(lp, "compFeatureTop") || !STRCMPI(lp, "topcomponentfeature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.compFeatureTop = atoi(lp);
            }
            else if (!STRCMPI(lp, "featureSizeBot") || !STRCMPI(lp,"bottomfeaturesize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.featureSizeBot = atoi(lp);
            }
            else if (!STRCMPI(lp, "featureSizeTop") || !STRCMPI(lp, "topfeaturesize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.featureSizeTop = atoi(lp);
            }
            else if (!STRCMPI(lp,"topTestattribPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.topTestattribPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"topConnectorPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.topConnectorPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"topThroughholePriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.topThroughholePriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"topSMDPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.topSMDPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"topViaPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.topViaPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNtopTestattrib"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNtopTestattrib = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNtopConnector"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNtopConnector = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNtopThroughhole"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNtopThroughhole = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNtopSMD"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNtopSMD = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNtopVia"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNtopVia = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNtopTestattrib"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNtopTestattrib = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNtopConnector"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNtopConnector = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNtopThroughhole"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNtopThroughhole = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNtopSMD"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNtopSMD = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNtopVia"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNtopVia = atoi(lp);
            }
            else if (!STRCMPI(lp,"botTestattribPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.botTestattribPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"botConnectorPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.botConnectorPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"botThroughholePriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.botThroughholePriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"botSMDPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.botSMDPriority = atoi(lp);
            }
            else if (!STRCMPI(lp,"botViaPriority"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.botViaPriority = atoi(lp);
            }

            else if (!STRCMPI(lp,"MFNbotTestattrib"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNbotTestattrib = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNbotConnector"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNbotConnector = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNbotThroughhole"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNbotThroughhole = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNbotSMD"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNbotSMD = atoi(lp);
            }
            else if (!STRCMPI(lp,"MFNbotVia"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.MFNbotVia = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNbotTestattrib"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNbotTestattrib = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNbotConnector"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNbotConnector = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNbotThroughhole"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNbotThroughhole = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNbotSMD"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNbotSMD = atoi(lp);
            }
            else if (!STRCMPI(lp,"SFNbotVia"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->AccessAnalysis.SFNbotVia = atoi(lp);
            }
            else
            {
               CString err;
               err.Format("Unknown Section [%s] in DFT Settings file [%s]\n", lp, fname);
               ErrorMessage(tmp, err);
            }
         }
         else if (!STRCMPI(lp,".ProbableShorts"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

            if (!STRCMPI(lp,"pin_pin"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->ProbableShorts.pin_pin = atoi(lp);
            }
            else if (!STRCMPI(lp,"pin_pinSize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double pin_pinSize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->ProbableShorts.pin_pinSize = pin_pinSize * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp,"pin_feature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->ProbableShorts.pin_feature = atoi(lp);
            }
            else if (!STRCMPI(lp,"pin_featureSize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double pin_featureSize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->ProbableShorts.pin_featureSize = pin_featureSize * Units_Factor(u, pageunits);
            }
            else if (!STRCMPI(lp,"feature_feature"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->ProbableShorts.feature_feature = atoi(lp);
            }
            else if (!STRCMPI(lp,"feature_featureSize"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               double feature_featureSize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               if ((u =  GetUnitIndex(lp)) > -1)
                  defaultsetting->ProbableShorts.feature_featureSize = feature_featureSize * Units_Factor(u, pageunits);
            }
         }
         else if (!STRCMPI(lp,".ProbeAssignment"))
         {
            if ((lp = strtok(NULL," =\t\n")) == NULL) continue;

            if (!STRCMPI(lp, "top"))
            {
               if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
               defaultsetting->ProbeAssignment.top = atoi(lp);
            }
            else if (!STRCMPI(lp, "bottom"))
            {
               if ((lp = strtok(NULL," =\t\n")) == NULL) continue;
               defaultsetting->ProbeAssignment.bottom = atoi(lp);
            }
         }
         else
         {
            CString err;
            err.Format("Unknown Section [%s] in DFT Settings file [%s]\n", lp, fname);
            ErrorMessage(tmp, err);
         }

      }
      fclose(fp);
   }
}



