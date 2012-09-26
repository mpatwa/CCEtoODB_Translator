
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "Lic.h"
#include "crypt.h"
#include "drc.h"
#include "Drc_Util.h"
#include "ta_short.h"
#include "Gauge.h"
#include "Measure.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern DRC_AperturePadstackArray DRCaperturepadstackarray; // from DRC.cpp
extern int DRCaperturepadstackcnt;

extern DRC_PinkooArray DRCpinkooarray; // from DRC.cpp
extern int DRCpinkoocnt;


/******************************************************************************
* CCEtoODBDoc::OnProbableShortAnalysis
*/
void CCEtoODBDoc::OnProbableShortAnalysis()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft) && !getApp().getCamcadLicense().isLicensed(camcadProductAgilent5DxWrite))  // 5dx output hat the Probable short analysis
   {
      ErrorAccess("You do not have a License for DFT Analyzer!");
      return;
   }*/

	FileStruct *pcbFile = this->getFileList().GetOnlyShown(blockTypePcb);

	if (pcbFile)
	{
		CDFTProbableShort probableShort(this, pcbFile);
		probableShort.DoProbableShortAnalysis();
		UpdateAllViews(NULL);
	}
	else
	{
		int pcbVisible =	this->getFileList().GetVisibleCount();

		if (pcbVisible < 1)
			ErrorMessage("No visible PCB file detected.\n\nProbable Shorts Analysis only supports single visible PCB file.", "Probable Shorts Analysis");
		else if (pcbVisible > 1)
			ErrorMessage("Multiple visible PCB files detected.\n\nProbable Shorts Analysis only supports single visible PCB file.", "Probable Shorts Analysis");
	}
}


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CDFTProbableShort
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CDFTProbableShort::CDFTProbableShort(CCEtoODBDoc *pDoc, FileStruct *pPCBFile)
{
	m_pDoc = pDoc;
	m_pPCBFile = pPCBFile;

	// Default settings
	m_bPinPin = TRUE;
   m_bPinFeature = TRUE;
   m_bFeatureFeature = TRUE;
   m_dPinPinSize = 0.025 * Units_Factor(UNIT_INCHES, pDoc->getSettings().getPageUnits());
   m_dPinFeatureSize = 0.025 * Units_Factor(UNIT_INCHES, pDoc->getSettings().getPageUnits());
   m_dFeatureFeatureSize = 0.025 * Units_Factor(UNIT_INCHES, pDoc->getSettings().getPageUnits());
}

CDFTProbableShort::~CDFTProbableShort()
{
}

int CDFTProbableShort::DoProbableShortAnalysis()
{

   if (!m_pDoc->ta_options_loaded)
		loadSettingFile();

   m_pDoc->OnGeneratePinloc();

	int unitDecimals = GetDecimals(m_pDoc->getSettings().getPageUnits());
   TA_ProbableShorts dlg;
   dlg.m_pin_pin = m_bPinPin;
   dlg.m_pin_feature = m_bPinFeature;
   dlg.m_feature_feature = m_bFeatureFeature;
   dlg.m_pin_pinSize.Format("%.*lf", unitDecimals, m_dPinPinSize);
   dlg.m_pin_featureSize.Format("%.*lf", unitDecimals, m_dPinFeatureSize);
   dlg.m_feature_featureSize.Format("%.*lf", unitDecimals, m_dFeatureFeatureSize);

   if (dlg.DoModal() == IDOK)
   {
      m_bPinPin = dlg.m_pin_pin;
      m_bPinFeature = dlg.m_pin_feature;
      m_bFeatureFeature = dlg.m_feature_feature;
      m_dPinPinSize = atof(dlg.m_pin_pinSize);
      m_dPinFeatureSize = atof(dlg.m_pin_featureSize);
      m_dFeatureFeatureSize = atof(dlg.m_feature_featureSize);

      deleteDRCbyType(DRC_ALG_PROBABLESHORT_PINtoPIN);
      deleteDRCbyType(DRC_ALG_PROBABLESHORT_PINtoFEATURE);
      deleteDRCbyType(DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE);

      runTestShortsAnalysis();
   }

	return 1;
}

void CDFTProbableShort::loadSettingFile()
{
	int pageunits = m_pDoc->getSettings().getPageUnits();
	CString settingFile = getApp().getSystemSettingsFilePath("default.dft");
	FILE *fp;

   if ((fp = fopen(settingFile,"rt")) != NULL)
   {
		char line[255];
      int version = 1;

      while (fgets(line, 255, fp))
      {
         if (!strncmp(line, ".VERSION 2", 10))
         {
            version = 2;
            break;
         }
      }

      if (version < 2)
         ErrorMessage(settingFile, "You need to save a new .dft file");

		CString tmp;
		char *lp;
		int units;
		while (fgets(line, 255, fp))
      {
         tmp = line;
         if ((lp = strtok(line, " \t\n")) == NULL)
            continue;

         if (lp[0] != '.')
            continue;

         if (!STRCMPI(lp, ".ProbableShorts"))
         {
            if ((lp = strtok(NULL, " \t\n")) == NULL) 
					continue;

            if (!STRCMPI(lp, "pin_pin"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_bPinPin = atoi(lp);
            }
            else if (!STRCMPI(lp, "pin_pinSize"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_dPinPinSize = atof(lp);

               if ((lp = strtok(NULL, " \t\n")) == NULL)
						continue;
               if ((units =  GetUnitIndex(lp)) > -1)
                  m_dPinPinSize = m_dPinPinSize * Units_Factor(units, pageunits);
            }
            else if (!STRCMPI(lp, "pin_feature"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_bPinFeature = atoi(lp);
            }
            else if (!STRCMPI(lp, "pin_featureSize"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_dPinFeatureSize = atof(lp);

               if ((lp = strtok(NULL, " \t\n")) == NULL)
						continue;
               if ((units =  GetUnitIndex(lp)) > -1)
                  m_dPinFeatureSize = m_dPinFeatureSize * Units_Factor(units, pageunits);
            }
            else if (!STRCMPI(lp, "feature_feature"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_bFeatureFeature = atoi(lp);
            }
            else if (!STRCMPI(lp, "feature_featureSize"))
            {
               if ((lp = strtok(NULL, " =\t\n")) == NULL)
						continue;
               m_dFeatureFeatureSize = atof(lp);

               if ((lp = strtok(NULL, " \t\n")) == NULL)
						continue;
               if ((units =  GetUnitIndex(lp)) > -1)
                  m_dFeatureFeatureSize = m_dFeatureFeatureSize * Units_Factor(units, pageunits);
            }
         }
		}
	}
}

void CDFTProbableShort::deleteDRCbyType(int drcType)
{
   m_pDoc->UnselectAll(TRUE);

   POSITION pos = m_pPCBFile->getDRCList().GetHeadPosition();
   while (pos != NULL)
   {
      DRCStruct *drc = m_pPCBFile->getDRCList().GetNext(pos);
      if (drc->getAlgorithmType() == drcType)
         RemoveOneDRC(m_pDoc, drc, m_pPCBFile);
   }
}

void CDFTProbableShort::runTestShortsAnalysis()
{
	CString logFileName = GetLogfilePath("ta.log");
   if ((m_pLogFile = fopen(logFileName, "wt")) == NULL)
   {
      CString msg = "";
      msg.Format("Error open [%s] file", logFileName);
      ErrorMessage(msg, "Error");
   }
	else
	{
      fprintf(m_pLogFile, "REMARK Shorts Analysis\nREMARK ===============\n");

		m_pDoc->PrepareAddEntity(m_pPCBFile);    
		int res = DRC_RunTestShortsAnalysis();
		if (res < 0)
		{
			CString errMsg;
			errMsg.Format("Error open [%s] file", logFileName);
			ErrorMessage(errMsg, "Error");
		}

		fclose(m_pLogFile);
	}
}

double CDFTProbableShort::getLargestApertureSize()
{
   double size = 0;

   for (int i=0; i<m_pDoc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = m_pDoc->getBlockAt(i);
      if (block == NULL)
			continue;

      // create necessary aperture blocks
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         m_pDoc->validateBlockExtents(block);

         if (size < fabs(block->getXmin())) size = fabs(block->getXmin());
         if (size < fabs(block->getYmin())) size = fabs(block->getYmin());
         if (size < fabs(block->getXmax())) size = fabs(block->getXmax());
         if (size < fabs(block->getYmax())) size = fabs(block->getYmax());
      }
   }

   return size * 2;
}

void CDFTProbableShort::doAllPadstacks(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);     
      if (data->getDataType() != dataTypeInsert)
         continue;

      BlockStruct *block = m_pDoc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (block->getBlockType() != blockTypePadstack)
			continue;

      int pi = drc_get_aperturepadstackindex(block->getName());
      if (pi < 0)
      {
         fprintf(m_pLogFile, "AperturePadstack [%s] not found in index\n", pi);
			continue;
      }


      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();

		Point2 point2;
      point2.x = data->getInsert()->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // here update padstackKooArray 
      PS_PadstackKoo *padstack = new PS_PadstackKoo;
      padstackKooArray.SetAtGrow(padstackKooArray.GetSize(), padstack);  
      padstack->entitynumber = data->getEntityNumber();
      padstack->x = point2.x;
      padstack->y = point2.y;
      padstack->rot = rotation + data->getInsert()->getAngle();
      padstack->mirror = block_mirror;
      padstack->layer = 0;
      padstack->netname = NET_UNUSED_PINS;

      Attrib *attrib;
      if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_NETNAME, 0))   
         padstack->netname = get_attvalue_string(m_pDoc, attrib);

      if (DRCaperturepadstackarray[pi]->typ & 1)
         padstack->layer |= 1;
      if (DRCaperturepadstackarray[pi]->typ & 2)
         padstack->layer |= 2;

      padstack->aperturepadstackindex = pi;
   }
}

int CDFTProbableShort::DRC_RunTestShortsAnalysis()
{
   CWaitCursor w;
   double smalldelta = 1;
   int decimals = GetDecimals(m_pDoc->getSettings().getPageUnits());
   int pinToPinErrorCnt = 0;
   int pinToFeatureErrorCnt = 0;
   int featureToFeatureErrorCnt = 0;

	int i=0;
   for (i=0; i<decimals; i++)
      smalldelta /= 10;

   padstackKooArray.SetSize(0,100);

	drc_init();
   drc_do_aperturepadstacks(m_pDoc);

   double largestaperturesize = getLargestApertureSize();
   double maxPinSpacingForError = (largestaperturesize * 2) + m_dPinPinSize;

   if (DRCaperturepadstackcnt == 0)
   {
      MessageBox(NULL, "No Padstack was found", "Error", MB_ICONEXCLAMATION | MB_OK);
      return -2;
   }

   drc_load_allcomps(m_pDoc, &m_pPCBFile->getBlock()->getDataList(), 0.0, 0.0, 0.0, m_pPCBFile->isMirrored(), m_pPCBFile->getScale(), 0, -1);

   // now collect all component pins from netlist
   drc_do_aperturenetlistpinloc(m_pLogFile, m_pDoc, &m_pPCBFile->getNetList(), m_pPCBFile->getScale(), &m_pPCBFile->getBlock()->getDataList());
         
   // collect all padstacks, vias, testpoint etc... (all but component pins) from Design file.
   doAllPadstacks(&m_pPCBFile->getBlock()->getDataList(), 0.0, 0.0, 0.0, m_pPCBFile->isMirrored(), m_pPCBFile->getScale(), 0, -1);


   int startstacknum;
   int endstacknum;
   if (drc_check_layers(m_pDoc, &startstacknum, &endstacknum) < 0)
   {
      ErrorMessage("No Electrical Layers found!");
      return -1;
   }

   // prepare DRC definitions
   CTime time;

   if (m_bPinPin)
   {  
      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "Start Pin-Pin Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Pin-Pin Short Calculation");

      for (int p1=0; p1<DRCpinkoocnt; p1++)
      {
         gauge->SetPos(round(100.0 * p1 / DRCpinkoocnt));

         for (int p2=p1 + 1; p2<DRCpinkoocnt; p2++)
         {
            DRC_pinkoo* pinkoo1 = DRCpinkooarray[p1];
            DRC_pinkoo* pinkoo2 = DRCpinkooarray[p2];

            if (pinkoo1->aperturepadstackindex < 0)
               continue;
            if (pinkoo2->aperturepadstackindex < 0)
               continue;
            if (pinkoo1->netname.Compare(NET_UNUSED_PINS) && !pinkoo1->netname.Compare(pinkoo2->netname))
               continue;

            double dx = (pinkoo2->x - pinkoo1->x);
            double dy = (pinkoo2->y - pinkoo1->y);
            if (fabs(dx) > maxPinSpacingForError || fabs(dy) > maxPinSpacingForError    )
               continue;


            // Check top
            if ((pinkoo1->layer & 1) && (pinkoo2->layer & 1))
            {
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_top;
               if (pinkoo2->mirror)
                  num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_bot;

               // Bug: At this point, num1 and num2 contain the block numbers for the top/bottom pads in the DRCaperturepadstackarray.
               // Unfortunately the insert information for the pads are not contained in the DRCaperturepadstackarray elements.
               // The result is that pads inserted with nonzero x, y, and rotation values have incorrect polygons generated
               // for them later during the MeasureApertureToAperture() call.
               // 20030731 - knv
               
               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc,
								b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
								b2, pinkoo2->x, pinkoo2->y, pinkoo2->rotation, 0, &result1, &result2);

                  if (dist > 0 && dist < m_dPinPinSize)
                  {
                     double x = (result1.x + result2.x)/2.;
                     double y = (result1.y + result2.y)/2.;

                     CString comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     // dist is between p1 and p2
                     drc_probable_short_pin_pin(m_pDoc, m_pPCBFile,  "DRC_PIN_PIN_TOP", comment, x, y, LAYTYPE_SIGNAL_TOP,
									startstacknum, m_dPinPinSize, dist,
									pinkoo1->entitynumber, result1.x, result1.y,
									pinkoo2->entitynumber, result2.x, result2.y);

                     pinToPinErrorCnt++;
                  }
               }
            } 


            // Check bottom
            if ((pinkoo1->layer & 2) && (pinkoo2->layer & 2))
            {
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo2->mirror)
                  num2 = DRCaperturepadstackarray[pinkoo2->aperturepadstackindex]->aperture_num_top;

               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc, 
								b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
								b2, pinkoo2->x, pinkoo2->y, pinkoo2->rotation, 0, &result1, &result2);

                  if (dist > 0 && dist < m_dPinPinSize)
                  {
                     double x = (result1.x + result2.x)/2.;
                     double y = (result1.y + result2.y)/2.;

                     CString comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     // dist is between p1 and p2
                     drc_probable_short_pin_pin(m_pDoc, m_pPCBFile,  "DRC_PIN_PIN_BOT", comment, x, y, LAYTYPE_SIGNAL_BOT,
									endstacknum, m_dPinPinSize, dist,
									pinkoo1->entitynumber, result1.x, result1.y,
									pinkoo2->entitynumber, result2.x, result2.y);

                     pinToPinErrorCnt++;
                  }
               } 
            } 
         }
      }

      // deinit gauge
      delete gauge;

      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "End Pin-Pin Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));
   }


   if (m_bPinFeature)
   {
      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "Start Pin-Feature Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Pin-Feature Short Calculation");

      for (int p1=0; p1<DRCpinkoocnt; p1++)
      {
         gauge->SetPos(round(100.0*p1/DRCpinkoocnt));

         for (int p2=0; p2<padstackKooArray.GetSize(); p2++)
         {
            DRC_pinkoo* pinkoo1 = DRCpinkooarray[p1];
            DRC_pinkoo* pinkoo2 = DRCpinkooarray[p2];

            if (pinkoo1->aperturepadstackindex < 0)
               continue;
            if (padstackKooArray[p2]-> aperturepadstackindex < 0)
               continue;
            if (pinkoo1->netname.Compare(NET_UNUSED_PINS) && !pinkoo1->netname.Compare(padstackKooArray[p2]->netname))
               continue;

            double dx = (padstackKooArray[p2]->x - pinkoo1->x);
            double dy = (padstackKooArray[p2]->y - pinkoo1->y);
            if (fabs(dx) > (largestaperturesize + 2 * m_dPinFeatureSize) || fabs(dy) > (largestaperturesize + 2 * m_dPinFeatureSize))
               continue;


            // Check top
            if ((pinkoo1->layer & 1) && (padstackKooArray[p2]->layer & 1))
            {
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;

               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);
               
                  if (dist > 0 && dist < m_dPinFeatureSize)
                  {
                     double x = pinkoo1->x + dx/2;
                     double y = pinkoo1->y + dy/2;

                     CString comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     drc_probable_short_pin_feature(m_pDoc, m_pPCBFile, "DRC_PIN_FEATURE_TOP", comment, x, y, LAYTYPE_SIGNAL_TOP, 
								startstacknum, m_dPinFeatureSize, dist, pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     pinToFeatureErrorCnt++;
                  } 
               }
            }


            // Check bottom
            if ((pinkoo1->layer & 2) && (padstackKooArray[p2]->layer & 2))
            {
               int num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_bot;
               if (pinkoo1->mirror)
                  num1 = DRCaperturepadstackarray[pinkoo1->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;

               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc, 
                        b1, pinkoo1->x, pinkoo1->y, pinkoo1->rotation, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);
         
                  if (dist > 0 && dist < m_dPinFeatureSize)
                  {
                     double x = pinkoo1->x + dx/2;
                     double y = pinkoo1->y + dy/2;

                     CString comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     drc_probable_short_pin_feature(m_pDoc, m_pPCBFile, "DRC_PIN_FEATURE_BOT", comment, x, y, LAYTYPE_SIGNAL_BOT, 
								endstacknum, m_dPinFeatureSize, dist, pinkoo1->entitynumber, pinkoo1->x, pinkoo1->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     pinToFeatureErrorCnt++;
                  }
               }
            }
         }
      }

      // deinit gauge
      delete gauge;

      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "End Pin-Via Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));
   }


	if (m_bFeatureFeature)
   {
      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "Start Feature-Feature Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));

      CProgressDlg *gauge = new CProgressDlg;
      gauge->caption = "DRC Function";
      gauge->Create();
      gauge->SetStatus("Probable Feature-Feature Short Calculation");

      for (int p1=0; p1<padstackKooArray.GetSize(); p1++)
      {
         gauge->SetPos(round(100.0 * p1 / padstackKooArray.GetSize()));

         for (int p2=p1 + 1; p2<padstackKooArray.GetSize(); p2++)
         {
            if (padstackKooArray[p1]->aperturepadstackindex < 0)
               continue;
            if (padstackKooArray[p2]-> aperturepadstackindex < 0)
               continue;
            if (padstackKooArray[p1]->netname.Compare(NET_UNUSED_PINS) && !padstackKooArray[p1]->netname.Compare(padstackKooArray[p2]->netname))
					continue;

            double dx = (padstackKooArray[p2]->x - padstackKooArray[p1]->x);
            double dy = (padstackKooArray[p2]->y - padstackKooArray[p1]->y);
            if (fabs(dx) > (largestaperturesize + 2*m_dFeatureFeatureSize) || fabs(dy) > (largestaperturesize + 2*m_dFeatureFeatureSize))
               continue;


            // Check top
            if ((padstackKooArray[p1]->layer & 1) && (padstackKooArray[p2]->layer & 1))
            {
               int num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p1]->mirror)
                  num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_bot;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;

               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc, 
                        b1, padstackKooArray[p1]->x, padstackKooArray[p1]->y, 0.0, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);

                  if (dist > 0 && dist < m_dFeatureFeatureSize)
                  {
                     double x = padstackKooArray[p1]->x + dx/2;
                     double y = padstackKooArray[p1]->y + dy/2;

                     CString comment;
                     comment.Format("Top %1.*lf", decimals, dist);

                     drc_probable_short_feature_feature(m_pDoc, m_pPCBFile, "DRC_FEATURE_FEATURE_TOP", comment, x, y,
                        LAYTYPE_SIGNAL_TOP, startstacknum, m_dFeatureFeatureSize, dist,
                        padstackKooArray[p1]->entitynumber, padstackKooArray[p1]->x, padstackKooArray[p1]->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     featureToFeatureErrorCnt++;
                  } 
               }
            } 


				// Check bottom
            if ((padstackKooArray[p1]->layer & 2) && (padstackKooArray[p2]->layer & 2))
            {
               int num1 =  DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p1]->mirror)
                  num1 = DRCaperturepadstackarray[padstackKooArray[p1]->aperturepadstackindex]->aperture_num_top;

               int num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_bot;
               if (padstackKooArray[p2]->mirror)
                  num2 = DRCaperturepadstackarray[padstackKooArray[p2]->aperturepadstackindex]->aperture_num_top;

               if (num1 > -1 && num2 > -1)   
               {
                  BlockStruct *b1 = m_pDoc->Find_Block_by_Num(num1);
                  BlockStruct *b2 = m_pDoc->Find_Block_by_Num(num2);

                  Point2 result1, result2;
                  double dist = MeasureApertureToAperture(m_pDoc, 
                        b1, padstackKooArray[p1]->x, padstackKooArray[p1]->y, 0.0, 0,
                        b2, padstackKooArray[p2]->x, padstackKooArray[p2]->y, 0.0, 0,
                        &result1, &result2);

                  if (dist > 0 && dist < m_dFeatureFeatureSize)
                  {
                     double x = padstackKooArray[p1]->x + dx/2;
                     double y = padstackKooArray[p1]->y + dy/2;

                     CString comment;
                     comment.Format("Bottom %1.*lf", decimals, dist);

                     drc_probable_short_feature_feature(m_pDoc, m_pPCBFile, "DRC_FEATURE_FEATURE_BOT", comment, x, y,
                        LAYTYPE_SIGNAL_BOT, endstacknum, m_dFeatureFeatureSize, dist,
                        padstackKooArray[p1]->entitynumber, padstackKooArray[p1]->x, padstackKooArray[p1]->y,
                        padstackKooArray[p2]->entitynumber, padstackKooArray[p2]->x, padstackKooArray[p2]->y);

                     featureToFeatureErrorCnt++;
                  }  
               } 
            }
         }
      }

      // deinit gauge
      delete gauge;

      time = time.GetCurrentTime();
      fprintf(m_pLogFile, "End Via-Via Short calculations at %s\n", time.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   }


   // here set DRC??? layer attributes
   LayerStruct *l;
   if ((l = m_pDoc->FindLayer_by_Name("DRC_PIN_PIN_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = m_pDoc->FindLayer_by_Name("DRC_PIN_PIN_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);
   if ((l = m_pDoc->FindLayer_by_Name("DRC_PIN_FEATURE_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = m_pDoc->FindLayer_by_Name("DRC_PIN_FEATURE_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);
   if ((l = m_pDoc->FindLayer_by_Name("DRC_FEATURE_FEATURE_BOT")) != NULL)
      l->setLayerType(LAYTYPE_BOTTOM);
   if ((l = m_pDoc->FindLayer_by_Name("DRC_FEATURE_FEATURE_TOP")) != NULL)
      l->setLayerType(LAYTYPE_TOP);


   drc_deinit();

   for (i=0; i<padstackKooArray.GetSize(); i++)
      delete padstackKooArray[i];
   padstackKooArray.RemoveAll();

   if (pinToPinErrorCnt == 0 && pinToFeatureErrorCnt == 0 && featureToFeatureErrorCnt == 0)
   {
      formatMessageBox(MB_ICONEXCLAMATION, "No Errors Encountered!");
   }
   else
   {
      formatMessageBox(MB_ICONINFORMATION, "%d pinToPin, %d pinToFeature, and %d featureToFeature errors.",
         pinToPinErrorCnt,pinToFeatureErrorCnt,featureToFeatureErrorCnt);
   }

   return 1;
}



/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// TA_ProbableShorts dialog
////////////////////////////////////////////////////////////////////////////////
TA_ProbableShorts::TA_ProbableShorts(CWnd* pParent /*=NULL*/)
   : CDialog(TA_ProbableShorts::IDD, pParent)
{
   //{{AFX_DATA_INIT(TA_ProbableShorts)
   m_pin_pin = FALSE;
   m_pin_pinSize = _T("");
   m_pin_feature = FALSE;
   m_pin_featureSize = _T("");
   m_feature_feature = FALSE;
   m_feature_featureSize = _T("");
   //}}AFX_DATA_INIT
}

void TA_ProbableShorts::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TA_ProbableShorts)
   DDX_Check(pDX, IDC_PIN_PIN, m_pin_pin);
   DDX_Text(pDX, IDC_PIN_PIN_SIZE, m_pin_pinSize);
   DDX_Check(pDX, IDC_PIN_VIA, m_pin_feature);
   DDX_Text(pDX, IDC_PIN_VIA_SIZE, m_pin_featureSize);
   DDX_Check(pDX, IDC_VIA_VIA, m_feature_feature);
   DDX_Text(pDX, IDC_VIA_VIA_SIZE, m_feature_featureSize);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TA_ProbableShorts, CDialog)
   //{{AFX_MSG_MAP(TA_ProbableShorts)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
