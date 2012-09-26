// $Header: /CAMCAD/5.0/read_wrt/Sony_AOI_o.cpp 33    6/06/07 7:43p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

// Sony AOI

#include "stdafx.h"
#include "ccdoc.h"  
#include "response.h"
//#include "format_s.h"
//#include "lyr_lyr.h"     
//#include "attrib.h"
#include "pcbutil.h"
//#include "dbutil.h"
//#include "graph.h"
//#include <math.h>
//#include <float.h>
//#include "export.h"
//#include "find.h"
//#include "ck.h"
//#include "centroid.h"
//#include "Polygon.h"
//#include "StandardAperture.h"
#include "gauge.h"
#include "CCEtoODB.h"
#include "sony_aoi_o.h"
#include "RwUiLib.h"
#include "sony_aoi_o_gui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// From OdbPpIn.cpp
extern bool fileExists(const CString& path);
extern bool isFolder(const CString& pathname);

#define QCategoryOnly "CategoryOnly"
#define ICategoryOnly -7   // Must be a value other than a "good" model number, i.e. can not be positive, 0, or -1.

extern CProgressDlg *progress;
static long ProgressPosition = 0;


static FILE *logfp;

static CSonyDatabase *MasterDatabase = NULL;
static CSonyRsiXref  *SonyRsiXref = NULL;
static CSonyAoiSettings *SonyAoiSettings = NULL;
static CSonyCatModArray topCatModMap(0);
static CSonyCatModArray botCatModMap(0);

/// Move this to PcbUtil.cpp
int GetPinOuterExtents(CCEtoODBDoc *doc, CDataList *dataList, double *xmin, double *ymin, double *xmax, double *ymax)
{
   int pinCount = 0;

   *xmin = *ymin = DBL_MAX;
   *xmax = *ymax = -DBL_MAX;

   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);
      
		if (data != NULL && data->isInsertType(insertTypePin))
      {
			int pinBlockNum = data->getInsert()->getBlockNumber();
			BlockStruct *pinBlock = doc->getBlockAt(pinBlockNum);
			if (pinBlock != NULL)
			{
				CBasesVector pinBV(data->getInsert()->getBasesVector());

				if (!pinBlock->getExtent().isValid())
               pinBlock->calculateBlockExtents(doc->getCamCadData());

				CExtent pinExtent = pinBlock->getExtent();
				pinExtent.transform(pinBV.getTransformationMatrix());

				if (pinExtent.getXmin() < *xmin)
					*xmin = pinExtent.getXmin();

				if (pinExtent.getYmin() < *ymin)
					*ymin = pinExtent.getYmin();

				if (pinExtent.getXmax() > *xmax)
					*xmax = pinExtent.getXmax();

				if (pinExtent.getYmax() > *ymax)
					*ymax = pinExtent.getYmax();

				pinCount++;
			}
      }
   }

   return pinCount;
}


static bool File_Copy(CString src, CString dst)
{
#define FILECOPY_BUFSZ 16000

	char            *buf;
	FILE            *fi;
	FILE            *fo;
	unsigned        amount;
	unsigned        written;
	bool            copy_ok;

	buf = new char[FILECOPY_BUFSZ];

	fi = fopen( src, "rb" );
	fo = fopen( dst, "wb" );

	copy_ok = true;;
	if  ((fi == NULL) || (fo == NULL) )
	{
		copy_ok = false;
		if (fi != NULL) fclose(fi);
		if (fo != NULL) fclose(fo);
	}

	if (copy_ok)
	{
		do
		{
			amount = fread( buf, sizeof(char), FILECOPY_BUFSZ, fi );
			if (amount >= 0 && amount < FILECOPY_BUFSZ)
			{
				written = fwrite( buf, sizeof(char), amount, fo );
				if (written != amount)
				{
					copy_ok = false; // out of disk space or some other disk err?
				}
			}
		} // when amount read is < FILECOPY_BUFSZ, copy is done
		while ((copy_ok) && (amount == FILECOPY_BUFSZ));

		fclose(fi);
		fclose(fo);
	}
	delete [] buf;
	return(copy_ok);
}

static bool GetGrClassExtent(CCEtoODBDoc *doc, BlockStruct *block, int graphicClass, CExtent &extent)
{
	bool hasGrClass = false;

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

	if (block != NULL)
	{
		Mat2x2 m;
		RotMat2(&m, 0);

		// find panel outline
		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = block->getDataList().GetNext(dataPos);

			// found Panel Outline
			if (data->getDataType() == T_POLY && data->getGraphicClass() == graphicClass)
			{
				hasGrClass = true;

				ExtentRect polyextents;
				PolyExtents(doc, data->getPolyList(), &polyextents, 1, 0, 0, 0, &m, FALSE);

				// get extents
				if (polyextents.left < xmin) xmin = polyextents.left;
				if (polyextents.right > xmax) xmax = polyextents.right;
				if (polyextents.bottom < ymin) ymin = polyextents.bottom;
				if (polyextents.top > ymax) ymax = polyextents.top;
			}
		}
	}

	extent.setXmin(xmin);
	extent.setXmax(xmax);
	extent.setYmin(ymin);
	extent.setYmax(ymax);

	return hasGrClass;
}


static bool GetGrClassExtent(CCEtoODBDoc *doc, DataStruct *partdata, int graphicClass, CExtent &extent)
{
	BlockStruct *insertedBlock = NULL;

	if (partdata != NULL && partdata->getInsert() != NULL)
	{
		int insertedBlockNum = partdata->getInsert()->getBlockNumber();
		insertedBlock = doc->Find_Block_by_Num(insertedBlockNum);
	}

	return GetGrClassExtent(doc, insertedBlock, graphicClass, extent);
}

static int get_insidepadsextents(CCEtoODBDoc *doc, DataStruct *partdata, 
                          double *xmin, double *ymin, double *xmax, double *ymax)
{
	*xmin = *xmax = *ymin = *ymax = 0.0;

	if (partdata != NULL && partdata->getInsert() != NULL)
	{
		int insertedBlockNum = partdata->getInsert()->getBlockNumber();
		BlockStruct *insertedBlock = doc->Find_Block_by_Num(insertedBlockNum);
		if (insertedBlock != NULL)
		{
			return get_insidepadsextents(doc, &insertedBlock->getDataList(), xmin, ymin, xmax, ymax);
		}
	}

	return FALSE;
}

// A utility needed by several classes
// Put here to ease consistency of name handling

CString sonyAoiGetDatastructPartnumber(CCEtoODBDoc *doc, DataStruct *partdata)
{
	CString partnumber;

	if (doc != NULL && partdata != NULL)
	{
		if (partdata->isInsertType(insertTypeFiducial))
		{
#ifdef GROUP_FIDS_BY_GEOM_NAME
			// Use inserted geom name for fid part number, with prefix "Fid" added
			int insertedBlockNum = partdata->getInsert()->getBlockNumber();
			BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
			CString fidBlockName = insertedBlock->getName();
			partnumber.Format("(Fid) %s", fidBlockName);
#else
			// Part of case 1911 says we need to treat each fid indiviually.
			// I don't think it's true, we'll see. It is essentially the same as saying every
			// instance of a part number must have it's own category and model mapping, which
			// is false. Anyway, so we'll use fid Refname for now.
			// NOTE that early on we disallow fids with no Refname, so by the time we
			// are here we do not need to worry about fids with no refname.
			// But we will anyway. Use entity number if refname is blank.
			CString refname = partdata->getInsert()->getRefname();
			if (refname.IsEmpty())
				refname.Format("E%d", partdata->getEntityNumber());

			partnumber.Format("(Fid) %s", refname);
#endif
		}
		else
		{
			// PCB_COMPONENT expected, could be any insert type other than fid
			Attrib *a;
			if ((a = is_attvalue(doc, partdata->getAttributesRef(), ATT_PARTNUMBER, 0)) != NULL)
			{
				partnumber = get_attvalue_string(doc, a);
			}
		}
	}

	return partnumber;
}

/******************************************************************************
*/
void Sony_AOI_WriteFiles(const char *pathname, CCEtoODBDoc *Doc,FormatStruct *format, int page_units)
{
	CString logFile = GetLogfilePath("Sony_AOI.log");
   if ((logfp = fopen(logFile, "wt")) == NULL) 
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

	CSonyAoiSettings sonyAoiSettings;
	SonyAoiSettings = &sonyAoiSettings;

	if (logfp != NULL)
	{
		fprintf(logfp, "Sony AOI Settings\nDummy Type = %d\nPixel Conv = %f\nMax Frame = %d\nMaster Lib = (%s)\nMaster PW = (%s)\nXref Lib = (%s)\nExport Loaded = %s\n\n",
			sonyAoiSettings.getDummyType(),
			sonyAoiSettings.getPixelConversion(),
			sonyAoiSettings.getMaxFrameSize(),
			sonyAoiSettings.getMasterLibraryLocation(),
			sonyAoiSettings.getMasterLibraryPassword(),
			sonyAoiSettings.getRsiXrefLibraryLocation(),
			sonyAoiSettings.getExportLoadedOnly() ? "Yes" : "No");
	}

	if (sonyAoiSettings.getMasterLibraryLocation().IsEmpty())
	{
		CString msg;
		msg = "The Sony Master Database location has not been set.";
		msg += " Check the \".MASTER_DB_LOCATION\" setting in Sony_AOI.out.";
		ErrorMessage(msg, "Sony AOI Export Aborted");
		return;
	}

	if (!fileExists(sonyAoiSettings.getMasterLibraryLocation()))
	{
		CString msg;
		msg.Format("Can not find the Sony Master Database \"%s\".", sonyAoiSettings.getMasterLibraryLocation());
		msg += " Check the \".MASTER_DB_LOCATION\" setting in Sony_AOI.out.";
		ErrorMessage(msg, "Sony AOI Export Aborted");
		return;
	}


	CSonyBoard sonyBoard(Doc);
	if (sonyBoard.hasActiveFile())
	{
		MasterDatabase = new CSonyDatabase(sonyAoiSettings.getMasterLibraryLocation(),
													  sonyAoiSettings.getMasterLibraryPassword());
	
		SonyRsiXref = new CSonyRsiXref(sonyAoiSettings.getRsiXrefLibraryLocation());

      bool saveOptionEnabled = true;

		if (SonyRsiXref != NULL && !SonyRsiXref->isAvailable())
		{
			CString msg;
			msg.Format("Can not find the cross reference file \"%s\". New cross reference settings can not be saved.",
				SonyRsiXref->getLocation());
			ErrorMessage(msg, "Cross Reference \"Save\" Option Disabled");
         saveOptionEnabled = false;
		}
	
		sonyBoard.setExportFolder(pathname);
		sonyBoard.gatherUniqueParts();
		sonyBoard.applyPartNumberCrossReference();

		//sonyBoard.logUniqueParts("INITIAL CONDITIONS");
		//sonyBoard.getTopCatModMap().writeLogFile("Top");
		//sonyBoard.getBotCatModMap().writeLogFile("Bottom");

		CSonyAOIPropertySheet sonyGUI(sonyBoard);
		progress->ShowWindow(SW_HIDE); // get rid of old progress bar
      sonyGUI.SetSaveOptionEnabled(saveOptionEnabled);
		sonyGUI.DoModal();

		if (MasterDatabase != NULL) delete MasterDatabase;
		if (SonyRsiXref != NULL) delete SonyRsiXref;

	} // else messages already issued by CSonyBoard constructor

	if (logfp != NULL) fclose(logfp);

}

//====================================================================================

CSonyAoiSettings::CSonyAoiSettings()
{
	m_dummyType = SONYAOI_DUMMYTYPE_UNKNOWN;
	m_pixelConversion = 0;
	m_maxFrameSize = 0;
	m_masterLibraryLocation = "";
	m_masterLibraryPassword = "";
	m_rsiXrefLibraryLocation = "";
	m_exportLoaded = false;


   CString settingsFile( getApp().getExportSettingsFilePath("Sony_AOI.out") );

   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(settingsFile,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFile);
      MessageBox(NULL, tmp, "Sony AOI Settings", MB_OK | MB_ICONHAND);
	}
	else
	{
		while (fgets(line,255,fp))
		{
			if ((lp = strtok(line," \t\n")) == NULL)
				continue;

			if (lp[0] == '.')
			{
				if (STRCMPI(lp,".DUMMY_TYPE") == 0)
				{
					if ((lp = strtok(NULL," \t\n")) != NULL)
					{
						if (STRCMPI(lp, "MISSING") == 0)
							m_dummyType = SONYAOI_DUMMYTYPE_MISSING;
						else if (STRCMPI(lp, "OK_END") == 0)
							m_dummyType = SONYAOI_DUMMYTYPE_OK_END;
					}
				}
				else if (STRCMPI(lp,".PIXEL_CONVERSION") == 0)
				{
					if ((lp = strtok(NULL," \t\n")) != NULL)
						m_pixelConversion = atof(lp);
				}
				else if (STRCMPI(lp,".MAX_FRAME_SIZE") == 0)
				{
					if ((lp = strtok(NULL," \t\n")) != NULL)
						m_maxFrameSize = atoi(lp);
				}
				else if (STRCMPI(lp,".MASTER_DB_LOCATION") == 0)
				{
					// spaces allowed in value, so no spaces in token delimiter list
					if ((lp = strtok(NULL,"\t\n")) != NULL)
						m_masterLibraryLocation = lp;
				}
				else if (STRCMPI(lp,".MASTER_DB_PASSWORD") == 0)
				{
					// spaces allowed in value, so no spaces in token delimiter list
					if ((lp = strtok(NULL,"\t\n")) != NULL)
						m_masterLibraryPassword = lp;
				}
				else if (STRCMPI(lp,".XREF_LOCATION") == 0 || STRCMPI(lp,".RSI_XREF_LOCATION") == 0) // mentorization, dropped "rsi", but still backward compatible
				{
					// spaces allowed in value, so no spaces in token delimiter list
					if ((lp = strtok(NULL,"\t\n")) != NULL)
						m_rsiXrefLibraryLocation = lp;
				}
				else if (STRCMPI(lp,".EXPORT_LOADED") == 0)
				{
					if ((lp = strtok(NULL," \t\n")) != NULL)
					{
						if (lp[0] == 'Y' || lp[0] == 'y' || lp[0] == 'T' || lp[0] == 't')
							m_exportLoaded = true;
						else
							m_exportLoaded = false;
					}
				}
			}
		}

		fclose(fp);
	}
}

//====================================================================================

CSonyBoard::CSonyBoard(CCEtoODBDoc *doc) :
			m_uniqueParts(doc, 0),
			m_topCatModMap(0),
			m_botCatModMap(0)
{
	m_doc = doc;
	m_activeCCFile = determineActiveCCFile(m_doc);

	m_topPartCount = -1;  // value < 0 means not yet set
	m_botPartCount = -1;

	m_topFid1 = NULL;
	m_topFid2 = NULL;
	m_botFid1 = NULL;
	m_botFid2 = NULL;

	m_fidNumber = 0;
	m_suffixBoardName = "";
	m_pcbBasesVector = NULL;

	m_originX = 0.0;
	m_originY = 0.0;

	m_boardName = getDefaultBoardName();
}

CSonyBoard::~CSonyBoard()
{
	m_uniqueParts.empty();
	m_topCatModMap.empty();
	m_botCatModMap.empty();
}

FileStruct *CSonyBoard::determineActiveCCFile(CCEtoODBDoc *doc)
{
	FileStruct *filefound = NULL;

	if (doc != NULL)
	{
      // check if panel or single board,
      int panelfound = 0;
      int pcbfound   = 0;


      POSITION pos = doc->getFileList().GetHeadPosition();

      while (pos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);

         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (file->isShown())
				{
					filefound = file;
					panelfound++;
				}
         }

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            if (file->isShown())
				{
					filefound = file;
					pcbfound++;
				}
         }
      }

      if (panelfound > 1)
      {
         ErrorMessage("Multiple Visible Panel files found. Only one visible Panel allowed.","Too many Panels");
         return NULL;
      }

      if (pcbfound > 1)
      {
         ErrorMessage("Multiple Visible PCB files found. Only one visible PCB allowed.","Too many PCBs");
         return NULL;
      }

      if (!panelfound && !pcbfound)
      {
         ErrorMessage("No Visible Panel or PCB file found.", "No Visible File");
         return NULL;
      }

      if (panelfound && pcbfound)
      {
         ErrorMessage("Too many Visible files found.", "SJ: Too many visible Files");
         return NULL;
      }
	}

	return filefound;
}

int CSonyBoard::getPartCount(PcbSide side)
{
	if (side == sideTop)
	{
		if (m_topPartCount < 0)
			m_topPartCount = countParts(sideTop);

		return m_topPartCount;
	}
	else if (side == sideBottom)
	{
		if (m_botPartCount < 0)
			m_botPartCount = countParts(sideBottom);

		return m_botPartCount;
	}

	return 0;
}

void CSonyBoard::writePCBFiducial(CStdioFileWriteFormat *wfp, DataStruct *compData)
{
	if (wfp != NULL && compData != NULL)
	{
		CSonyPart *sonyPart = m_uniqueParts.get(compData);

		if (sonyPart != NULL)
		{
			CBasesVector compBV(compData->getInsert()->getBasesVector());
			if (m_pcbBasesVector != NULL)				
				compBV.transform(m_pcbBasesVector->getTransformationMatrix());

			double compX = compBV.getX();
			double compY = compBV.getY();

			double sonyX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (compX - m_originX));
			double sonyY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (compY - m_originY));

			double sonySizeX = 1.0;
			double sonySizeY = 1.0;

			// Case 1619 says just use 1.0 for fid size, so use of
			// extents has been disabled.
#ifdef USE_EXTENTS_FOR_FID_SIZE
			BlockStruct *insertedBlock = m_doc->getBlockAt(compData->getInsert()->getBlockNumber());
			if (insertedBlock != NULL)
			{
				if (!insertedBlock->getExtent().isValid())
					insertedBlock->calculateBlockExtents(*m_doc);
				CExtent ext = insertedBlock->getExtent();
				sonySizeX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, ext.getSize().cx);
				sonySizeY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, ext.getSize().cy);
			}

			// Default size back to 1 in case extent existed but was zero.
			if (sonySizeX == 0.0 || sonySizeY == 0.0)
				sonySizeX = sonySizeY = 1.0;
#endif

			m_fidNumber++;

			CString fidPname = compData->getInsert()->getRefname();
			if (fidPname.IsEmpty()) fidPname.Format("UnnamedFid%d", m_fidNumber);
			if (!m_suffixBoardName.IsEmpty()) fidPname += "_" + m_suffixBoardName;

			int category = 0;
			int model = 0;
			if (compData->getInsert()->getPlacedTop())
				m_topCatModMap.getMappedCategoryModel(sonyPart->getMasterCategory(), sonyPart->getMasterModel(), &category, &model);
			else
				m_botCatModMap.getMappedCategoryModel(sonyPart->getMasterCategory(), sonyPart->getMasterModel(), &category, &model);

			
			wfp->writef("%%ALMT%d=%.2f,%.2f\n", m_fidNumber, sonyX, sonyY);

			wfp->writef("%%ALMT%dPARTS", m_fidNumber);
			wfp->writef(",%%PNAME=%s", fidPname);
			wfp->writef(",%%PORG=%.2f,%.2f", sonyX, sonyY);
			wfp->writef(",%%PKIND=%s%d", "Alignment", m_fidNumber);
			wfp->writef(",%%PDIR=0");
			wfp->writef(",%%PSIZE=%.2f,%.2f", sonySizeX, sonySizeY);
			wfp->writef(",%%CATEGORY=%d", category);
			wfp->writef(",%%MODEL=%d", model);
			wfp->write("\n");
		}
	}
}

void CSonyBoard::writeSonyPartsRec(CStdioFileWriteFormat *wfp, CString title, CString refDes, CString partNumber, int category, int model, double sonyX, double sonyY, int sonyAngle, double frameSizeX, double frameSizeY)
{
	wfp->writef("%%%s", title);
	wfp->writef(",%%PNAME=%s", refDes);
	wfp->writef(",%%PORG=%.2f,%.2f", sonyX, sonyY);
	wfp->writef(",%%PKIND=%s", partNumber);
	wfp->writef(",%%PDIR=%d", sonyAngle);
	wfp->writef(",%%PSIZE=%.2f,%.2f", frameSizeX, frameSizeY);
	wfp->writef(",%%CATEGORY=%d", category);
	wfp->writef(",%%MODEL=%d", model);
	wfp->write("\n");
}

void CSonyBoard::writePCBComponent(CStdioFileWriteFormat *wfp, DataStruct *compData)
{
	// Output the part only if it is a member of the uniques parts list.
	// It is not an error to not be a member, it just means the part
	// missed some qualifying characteristic, so ignore it.
	
	if (wfp != NULL && compData != NULL)
	{
		CSonyPart *sonyPart = m_uniqueParts.get(compData);

		if (sonyPart != NULL)
		{
			double frameSizeX = 1.0;
			double frameSizeY = 1.0;
			sonyPart->determineFramesize(&frameSizeX, &frameSizeY);

			int category = 0;
			int model = 0;
			if (compData->getInsert()->getPlacedTop())
				m_topCatModMap.getMappedCategoryModel(sonyPart->getMasterCategory(), sonyPart->getMasterModel(), &category, &model);
			else
				m_botCatModMap.getMappedCategoryModel(sonyPart->getMasterCategory(), sonyPart->getMasterModel(), &category, &model);

			// Must have Package Outline to support Explode
			// Experimental, if we keep this it should be moved to a function.
         // <years later> Looks like we kept it, no motiviation to move/change anything in this exporter at the moment though.
			bool explodePins = sonyPart->getExplode();
			CExtent packageOutlineExtent;
			if (explodePins)
			{
				bool hasPkgOutline = GetGrClassExtent(m_doc, compData, GR_CLASS_PACKAGEOUTLINE, packageOutlineExtent);
				if (!hasPkgOutline || !packageOutlineExtent.isValid())
				{
					m_packageOutlineMissing = true;
#define AUTO_GEN_OUTLINE_EXTENT
#ifdef  AUTO_GEN_OUTLINE_EXTENT
					double xminx, yminy, xmaxx, ymaxy;
					if (get_insidepadsextents(m_doc, compData, &xminx, &yminy, &xmaxx, &ymaxy))
					{
						packageOutlineExtent.setXmin(xminx);
						packageOutlineExtent.setXmax(xmaxx);
						packageOutlineExtent.setYmin(yminy);
						packageOutlineExtent.setYmax(ymaxy);
						fprintf(logfp, "EXPLODE: No Package Outline for %s, extents determined by inside-pad dimensions.\n", sonyPart->getPartNumber());
					}
					else
					{
						explodePins = false;
						fprintf(logfp, "EXPLODE FAILED: No Package Outline for %s, Determination of inside-pad dimensions failed. Part not exploded.\n", sonyPart->getPartNumber());
					}
#else
					explodePins = false;
					fprintf(logfp, "EXPLODE FAILED: No Package Outline for %s, Part not exploded.\n", sonyPart->getPartNumber());

#endif
				}
			}
			
			//if (!sonyPart->getExplode())
			if (!explodePins)
			{
				// Use insert location for default, but centroid is preferred.
				// Actually, only centroid is truly correct according to Sony rep.
				CBasesVector compBV(compData->getInsert()->getBasesVector());
				if (m_pcbBasesVector != NULL)				
					compBV.transform(m_pcbBasesVector->getTransformationMatrix());

				double compX = compBV.getX();
				double compY = compBV.getY();
				double compT = compBV.getRotationDegrees();

				CPoint2d centroidPt;
            if (compData->getInsert()->getCentroidLocation(m_doc->getCamCadData(), centroidPt))
				{
					if (m_pcbBasesVector != NULL)
					{
						CTMatrix mat = m_pcbBasesVector->getTransformationMatrix();
						mat.transform(centroidPt);
					}

					compX = centroidPt.x;
					compY = centroidPt.y;
				}

				double sonyX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (compX - m_originX));
				double sonyY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (compY - m_originY));
				int sonyAngle = normalizeDegrees(DcaRound(compT + sonyPart->getAngleOffset())); // subtracting offset would be better, has physical world correlation (treats "offset" as angular origin) but Marks want it added, due to some precedents elsewhere.
				
				CString refdes = compData->getInsert()->getRefname();
				if (!m_suffixBoardName.IsEmpty()) refdes += "_" + m_suffixBoardName;
	
				writeSonyPartsRec(wfp, "PARTS", refdes, sonyPart->getPartNumber(), category, model, sonyX, sonyY, sonyAngle, frameSizeX, frameSizeY);
			
			}
			else
			{
				// Explode; Write each pin
				CBasesVector cmpBV(compData->getInsert()->getBasesVector());
				if (m_pcbBasesVector != NULL)				
					cmpBV.transform(m_pcbBasesVector->getTransformationMatrix());

				CTMatrix compMatrix = cmpBV.getTransformationMatrix();

				int insertedBlockNum = compData->getInsert()->getBlockNumber();
				BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);
				POSITION dataPos = insertedBlock->getDataList().GetHeadPosition();

				// Transform raw extent to lie where component is
				packageOutlineExtent.transform(compMatrix);

				while (dataPos)
				{
					DataStruct *data = insertedBlock->getDataList().GetNext(dataPos);
					if (data != NULL && data->isInsertType(insertTypePin))
					{
						CBasesVector pinBV(data->getInsert()->getBasesVector());
						pinBV.transform(compMatrix);

						double pinX = pinBV.getX();
						double pinY = pinBV.getY();
						double pinT = pinBV.getRotationDegrees();

						// Adjust pin location to package boundary edge
						if (pinX < packageOutlineExtent.getXmin())
							pinX = packageOutlineExtent.getXmin();
						if (pinX > packageOutlineExtent.getXmax())
							pinX = packageOutlineExtent.getXmax();

						if (pinY < packageOutlineExtent.getYmin())
							pinY = packageOutlineExtent.getYmin();
						if (pinY > packageOutlineExtent.getYmax())
							pinY = packageOutlineExtent.getYmax();


						double sonyX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (pinX - m_originX));
						double sonyY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (pinY - m_originY));

						int pinBlockNum = data->getInsert()->getBlockNumber();
						BlockStruct *pinBlock = m_doc->getBlockAt(pinBlockNum);
						if (pinBlock != NULL)
						{
							if (!pinBlock->getExtent().isValid())
                        pinBlock->calculateBlockExtents(m_doc->getCamCadData());

							CExtent *pinExtent = &pinBlock->getExtent();
							frameSizeX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, pinExtent->getXsize());
							frameSizeY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, pinExtent->getYsize());
						}
						if (frameSizeX == 0.0 || frameSizeY == 0.0)
							frameSizeX = frameSizeY = 1.0; // reset if either are zero

						int sonyAngle = normalizeDegrees(DcaRound(pinT + sonyPart->getAngleOffset())); // subtracting offset would be better, has physical world correlation (treats "offset" as angular origin) but Mark wants it added, due to some precedents elsewhere.
						
						CString refdes = compData->getInsert()->getRefname();
						refdes += "_" + data->getInsert()->getRefname();
						if (!m_suffixBoardName.IsEmpty()) refdes += "_" + m_suffixBoardName;

						writeSonyPartsRec(wfp, "PARTS", refdes, sonyPart->getPartNumber(), category, model, sonyX, sonyY, sonyAngle, frameSizeX, frameSizeY);
					}
				}

			}
		}
	}
}

void CSonyBoard::writePCBInserts(CStdioFileWriteFormat *wfp, BlockStruct *pcbblock, PcbSide side, int processingInsertType)
{
	if (wfp != NULL && pcbblock != NULL)
	{
		POSITION dataPos = pcbblock->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = pcbblock->getDataList().GetNext(dataPos);
			if (data != NULL && data->getDataType() == T_INSERT && data->getInsert() != NULL)
			{
				bool mirrored = data->getInsert()->getGraphicMirrored();
				bool top = data->getInsert()->getPlacedTop();
				bool bot = data->getInsert()->getPlacedBottom();

				if ((top && side == sideTop) || (bot && side == sideBottom))
				{
					int datainserttype = data->getInsert()->getInsertType();
					if (datainserttype == processingInsertType)
					{
						if (datainserttype == INSERTTYPE_PCBCOMPONENT)
						{
							writePCBComponent(wfp, data);
						}
						else if (datainserttype == INSERTTYPE_FIDUCIAL)
						{
							if ((int)data == (int)m_topFid1 ||
								(int)data == (int)m_topFid2 ||
								(int)data == (int)m_botFid1 ||
								(int)data == (int)m_botFid2)
								writePCBFiducial(wfp, data);
						}
					}
				}

			}
		}
	}
}

void CSonyBoard::writePCB(CStdioFileWriteFormat *wfp, BlockStruct *pcbblock, PcbSide side)
{
	if (wfp != NULL && pcbblock != NULL)
	{
		writePCBInserts(wfp, pcbblock, side, INSERTTYPE_FIDUCIAL);

		writePCBInserts(wfp, pcbblock, side, INSERTTYPE_PCBCOMPONENT);
	}
}

CString CSonyBoard::getDefaultBoardName()
{
	if (m_activeCCFile != NULL)
	{
		if (m_activeCCFile->getBlockType() == BLOCKTYPE_PANEL)
		{
			return m_activeCCFile->getBlock()->getName();
		}
		else if (m_activeCCFile->getBlockType() == BLOCKTYPE_PCB)
		{
			BlockStruct *pcbblock = m_activeCCFile->getBlock();
			return pcbblock->getName();
		}
	}

	return "NoActiveBoard";
}

void CSonyBoard::writePanelPCBs(CStdioFileWriteFormat *wfp, CDataList &datalist, PcbSide side)
{
	if (wfp != NULL)
	{
		POSITION dataPos = datalist.GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = datalist.GetNext(dataPos);
			if (data != NULL && data->isInsertType(insertTypePcb))
			{
				bool top = data->getInsert()->getPlacedTop();
				bool bot = data->getInsert()->getPlacedBottom();

				m_suffixBoardName = data->getInsert()->getRefname();

				m_pcbBasesVector = &data->getInsert()->getBasesVector();

				int blocknumber = data->getInsert()->getBlockNumber();
				FileStruct *file = m_doc->Find_File_by_BlockGeomNum(blocknumber);

				if (file !=NULL && file->getBlockType() == BLOCKTYPE_PCB)
					writePCB(wfp, file->getBlock(), side);
				
			}
		}
	}
}

void CSonyBoard::writePanelFids(CStdioFileWriteFormat *wfp, CDataList &datalist, PcbSide side)
{
	if (wfp != NULL)
	{
		POSITION dataPos = datalist.GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = datalist.GetNext(dataPos);
			if (data != NULL && data->isInsertType(insertTypeFiducial))
			{
				// If this is one of the selected fiducials and we're on the
				// right side, then output it.

				if (side == sideTop && data->getInsert()->getPlacedTop())
				{
					if ((long)data == (long)m_topFid1 || (long)data == (long)m_topFid2)
					{
						m_pcbBasesVector = NULL;  // is not in a pcb, don't apply a pcb transform
						writePCBFiducial(wfp, data);
					}
				}
				else if (side == sideBottom && data->getInsert()->getPlacedBottom())
				{
					if ((long)data == (long)m_botFid1 || (long)data == (long)m_botFid2)
					{
						m_pcbBasesVector = NULL;  // is not in a pcb, don't apply a pcb transform
						writePCBFiducial(wfp, data);
					}
				}
			}
		}
	}
}

void CSonyBoard::writeCCFile(CStdioFileWriteFormat *wfp, PcbSide side)
{
	if (m_activeCCFile != NULL && wfp != NULL)
	{
		m_fidNumber = 0;

		if (m_activeCCFile->getBlockType() == BLOCKTYPE_PANEL)
		{
			CExtent extent = this->GetOutlineExtent();

			wfp->writef("%%BOARD,%%BNAME=%s,%%BSIZE=%.02f,%.02f\n",
				this->getBoardName(), 
				m_doc->convertPageUnitsTo(pageUnitsMilliMeters, extent.getXsize()),
				m_doc->convertPageUnitsTo(pageUnitsMilliMeters, extent.getYsize()));
							
			writePanelFids(wfp, m_activeCCFile->getBlock()->getDataList(), side);
			writePanelPCBs(wfp, m_activeCCFile->getBlock()->getDataList(), side);
		}

		else if (m_activeCCFile->getBlockType() == BLOCKTYPE_PCB)
		{
			BlockStruct *pcbblock = m_activeCCFile->getBlock();
			CString boardname = this->getBoardName();

			CExtent extent = this->GetOutlineExtent();
	
			wfp->writef("%%BOARD,%%BNAME=%s,%%BSIZE=%.02f,%.02f\n",
				boardname, 
				m_doc->convertPageUnitsTo(pageUnitsMilliMeters, extent.getXsize()),
				m_doc->convertPageUnitsTo(pageUnitsMilliMeters, extent.getYsize()));

			m_suffixBoardName = ""; // Not used for single board view output
			m_pcbBasesVector = NULL;

			writePCB(wfp, pcbblock, side);
		}

		wfp->writef("%%END\n");
	}
}

CExtent CSonyBoard::GetOutlineExtent()
{
   Mat2x2 m;
   RotMat2(&m, 0);

	ExtentRect fileExtents;

   fileExtents.left = fileExtents.bottom = FLT_MAX;
   fileExtents.right = fileExtents.top = -FLT_MAX;

   bool OutlineFound = false;

	POSITION dataPos = m_activeCCFile->getBlock()->getDataList().GetHeadPosition();
	while (dataPos)
	{
		DataStruct *data = m_activeCCFile->getBlock()->getDataList().GetNext(dataPos);

		if ((data->getDataType() == T_POLY) &&
			((m_activeCCFile->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() == GR_CLASS_PANELOUTLINE) ||
			(m_activeCCFile->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)))
		{
			OutlineFound = true;

			ExtentRect polyExtents;
			if (PolyExtents(m_doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE))
			{
				if (polyExtents.left < fileExtents.left)
					fileExtents.left = polyExtents.left;
				if (polyExtents.right > fileExtents.right)
					fileExtents.right = polyExtents.right;
				if (polyExtents.bottom < fileExtents.bottom)
					fileExtents.bottom = polyExtents.bottom;
				if (polyExtents.top > fileExtents.top)
					fileExtents.top = polyExtents.top;
			}
		}
   }


	CExtent extent(fileExtents.left, fileExtents.bottom, fileExtents.right, fileExtents.top);

   if (!OutlineFound)
   {
      m_activeCCFile->getBlock()->calculateVisibleBlockExtents(m_doc->getCamCadData());		
		extent = m_activeCCFile->getBlock()->getExtent();
   }

   return extent;
}

int CSonyBoard::countInsertType(InsertTypeTag desiredInsertType, BlockStruct *pcbblock, PcbSide side)
{
	// For use with PCB

	int insertcount = 0;

	if (pcbblock != NULL)
	{
		POSITION dataPos = pcbblock->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = pcbblock->getDataList().GetNext(dataPos);
			if (data != NULL && data->getDataType() == T_INSERT && data->getInsert() != NULL)
			{
				bool top = data->getInsert()->getPlacedTop();
				bool bot = data->getInsert()->getPlacedBottom();

				if ((top && side == sideTop) || (bot && side == sideBottom))
				{
					if (data->getInsert()->getInsertType() == desiredInsertType)
					{
						// parts count only if they are in the unique parts list, 
						// i.e. count only if they have a part number
						if (desiredInsertType == insertTypePcbComponent)
						{
							CSonyPart *sonyPart = m_uniqueParts.get(data);
							if (sonyPart != NULL)
								insertcount++;
						}
						else
						{
							insertcount++;
						}
					}
				}

			}
		}
	}

	return insertcount;
}

int CSonyBoard::countInsertType(InsertTypeTag desiredInsertType, CDataList &datalist, PcbSide side)
{
	// For use with panel, process all PCBs in datalist

	int insertcount = 0;

	POSITION dataPos = datalist.GetHeadPosition();
	while (dataPos)
	{
		DataStruct *data = datalist.GetNext(dataPos);

		if (data != NULL && data->isInsertType(insertTypePcb))
		{
			int blocknumber = data->getInsert()->getBlockNumber();
			FileStruct *file = m_doc->Find_File_by_BlockGeomNum(blocknumber);

			if (file != NULL && file->getBlockType() == BLOCKTYPE_PCB)
				insertcount += countInsertType(desiredInsertType, file->getBlock(), side);
		}				
	}

	return insertcount;
}

int CSonyBoard::countParts(PcbSide side)
{
	int partcount = 0;

	if (m_activeCCFile)
	{
		if (m_activeCCFile->getBlockType() == BLOCKTYPE_PANEL)
		{
			partcount = countInsertType(insertTypePcbComponent, m_activeCCFile->getBlock()->getDataList(), side);
		}

		else if (m_activeCCFile->getBlockType() == BLOCKTYPE_PCB)
		{
			BlockStruct *pcbblock = m_activeCCFile->getBlock();
			partcount = countInsertType(insertTypePcbComponent, pcbblock, side);
		}
	}

	return partcount;
}

bool CSonyBoard::hasValidFidSelections()	
{ 
	// If a side has parts, then it must also have valid fids.
	// Valid fids for given side means two different fids are selected

	bool topFidsOk = false;
	bool botFidsOk = false;

	if (this->getPartCount(sideTop) > 0)
		topFidsOk = (m_topFid1 != NULL && m_topFid2 != NULL && m_topFid1 != m_topFid2);
	else
		topFidsOk = true; // no parts on top, don't need fids on top
	
	if (this->getPartCount(sideBottom) > 0)
		botFidsOk = (m_botFid1 != NULL && m_botFid2 != NULL && m_botFid1 != m_botFid2);
	else
		botFidsOk = true; // no parts on bottom, don't need fids on bottom
		
	return (topFidsOk && botFidsOk);
}


void CSonyBoard::writeTopTxt()
{
	if (m_activeCCFile == NULL) return;

#ifdef DEBUG_CAT_MOD_MAP
	m_topCatModMap.writeLogFile("Top");
#endif

	CFilePath outfilepath = getExportTxtFilePath(sideTop);

	CString path = outfilepath.getPath();
	   
	CStdioFileWriteFormat wf;

   if (wf.open(path))
   {
		writeCCFile(&wf, sideTop);
		wf.close();
	}
}

void CSonyBoard::writeBottomTxt()
{
	if (m_activeCCFile == NULL) return;

#ifdef DEBUG_CAT_MOD_MAP
	m_botCatModMap.writeLogFile("Bottom");
#endif

	CFilePath outfilepath = getExportTxtFilePath(sideBottom);

	CString path = outfilepath.getPath();
	   
	CStdioFileWriteFormat wf;

   if (wf.open(path))
   {
		writeCCFile(&wf, sideBottom);
		wf.close();
	}
}

void CSonyBoard::applyPartNumberCrossReference(CSonyPart *p)
{
	// This part only

	if (p != NULL)
	{
		CString partnumber = p->getPartNumber();
		int category = 0;
		int model = 0;
		bool explode = false;
		int angle = 0;

		SonyRsiXref->get(partnumber, &category, &model, &explode, &angle);
		p->setMasterCategory(category);
		p->setMasterModel(model);
		p->setExplode(explode);
		p->setAngleOffset(angle);
	}
}

void CSonyBoard::applyPartNumberCrossReference()
{
	// All parts

	if (SonyRsiXref != NULL)
	{
		for (int i = 0; i < m_uniqueParts.getSize(); i++)
		{
			CSonyPart *p = m_uniqueParts.getAt(i);
			applyPartNumberCrossReference(p);
		}
	}
}

void CSonyBoard::savePartNumberCrossReference()
{
   if (SonyRsiXref != NULL && SonyRsiXref->isAvailable())
	{
		for (int i = 0; i < m_uniqueParts.getSize(); i++)
		{
			CSonyPart *p = m_uniqueParts.getAt(i);
			if (p->getSave())
			{
				SonyRsiXref->put(p->getPartNumber(), p->getMasterCategory(), p->getMasterModel(), p->getExplode(), p->getAngleOffset());
			}
		}
	}
}

void CSonyBoard::gatherFiducials()
{
	CSonyPart *fid = NULL;

	fid = m_uniqueParts.getOrAdd(m_topFid1);
	if (fid != NULL)
		fid->setIsUsed(true);

	fid = m_uniqueParts.getOrAdd(m_topFid2);
	if (fid != NULL)
		fid->setIsUsed(true);

	fid = m_uniqueParts.getOrAdd(m_botFid1);
	if (fid != NULL)
		fid->setIsUsed(true);

	fid = m_uniqueParts.getOrAdd(m_botFid2);
	if (fid != NULL)
		fid->setIsUsed(true);
}

void CSonyBoard::gatherUniqueParts(BlockStruct *pcbblock)
{
	if (pcbblock != NULL)
	{
		POSITION dataPos = pcbblock->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct *data = pcbblock->getDataList().GetNext(dataPos);
			if (data != NULL && data->isInsertType(insertTypePcbComponent))
			{
				if (!SonyAoiSettings->getExportLoadedOnly() || partIsLoaded(data))
				{
					if (m_uniqueParts.getOrAdd(data) == NULL)
					{
						if (logfp != NULL)
						{
							fprintf(logfp, "No part number for %s\n",
								data->getInsert()->getRefname());
						}
					}
				}
			}
		}
	}
}

void CSonyBoard::gatherUniqueParts()
{
	m_uniqueParts.empty();

	if (m_activeCCFile != NULL)
	{
		if (m_activeCCFile->getBlockType() == BLOCKTYPE_PANEL)
		{
			POSITION dataPos = m_activeCCFile->getBlock()->getDataList().GetHeadPosition();
			while (dataPos)
			{
				DataStruct *data = m_activeCCFile->getBlock()->getDataList().GetNext(dataPos);

				if (data != NULL && data->isInsertType(insertTypePcb))
				{
					int blocknumber = data->getInsert()->getBlockNumber();
					BlockStruct *block = m_doc->getBlockAt(blocknumber);
					FileStruct *file = m_doc->Find_File_by_BlockGeomNum(blocknumber);
					
					if (file != NULL && file->getBlockType() == BLOCKTYPE_PCB)
						gatherUniqueParts(file->getBlock());
				}
			}
		}

		else if (m_activeCCFile->getBlockType() == BLOCKTYPE_PCB)
		{
			gatherUniqueParts(m_activeCCFile->getBlock());
		}
	}
}

void CSonyBoard::mapBoardSideCategoryModel()
{
	m_topCatModMap.empty();
	m_botCatModMap.empty();

	// Reset category to zero (Dummy) for any invalid cat/mod pairs.
	// Since each entry represents a unique part number, give it a unique model.
	// Also process all master cats that are already 0 in this loop, so we
	// end up with unique model for every part number in category 0.
	int model = 1;
	for (int i = 0; i < m_uniqueParts.getSize(); i++)
	{
		CSonyPart *part = m_uniqueParts.getAt(i);
		if (part->getMasterCategory() == 0 ||
			(MasterDatabase != NULL && !MasterDatabase->isValidCatModCombo(part->getMasterCategory(), part->getMasterModel())))
		{
			part->setMasterCategory(0);
			part->setMasterModel(model++);
		}
	}

	// Fill top and bottom maps with entries for each category/model used on given side
	for (int i = 0; i < m_uniqueParts.getSize(); i++)
	{
		CSonyPart *part = m_uniqueParts.getAt(i);

		if (part->getIsOnTop())    m_topCatModMap.add(part->getMasterCategory(), part->getMasterModel());
		if (part->getIsOnBottom()) m_botCatModMap.add(part->getMasterCategory(), part->getMasterModel());
	}

	// Create consecutive mapping for top and bottom individually
	m_topCatModMap.createConsecutiveMapping();
	m_botCatModMap.createConsecutiveMapping();
}

void CSonyBoard::logUniqueParts(CString title)
{
	if (logfp != NULL)
	{
		fprintf(logfp, "\n%s\n", title);
		for (int i = 0; i < m_uniqueParts.getSize(); i++)
		{
			CSonyPart *part = m_uniqueParts.getAt(i);
			fprintf(logfp, "   %3d.", i);
			fprintf(logfp, " PN(%s)", part->getPartNumber());
			fprintf(logfp, ", MCat(%d)", part->getMasterCategory());
			fprintf(logfp, ", MMod(%d)", part->getMasterModel());
			fprintf(logfp, ", Save(%s)", part->getSave() ? "Yes" : "No");
			fprintf(logfp, ", Explode(%s)", part->getExplode() ? "Yes" : "No");
			fprintf(logfp, ", RotAdj(%d)", part->getAngleOffset());

			fprintf(logfp, "\n");
		}
	}
}

CSonyDatabase* CSonyBoard::getMasterSonyDB()
{
	return MasterDatabase;
}

CSonyRsiXref* CSonyBoard::getMasterXref()
{
	return SonyRsiXref;
}

void CSonyBoard::transferImageData(PcbSide side)
{
	// Copy files named in ImageData fields from Master DB area to new
	// DB area, renaming to match new category and model.

	CSonyCatModArray *catModArray = (side == sideTop ? &m_topCatModMap : &m_botCatModMap);

	bool missingMasterBmp = false;
	bool copyFailed = false;

	if (logfp != NULL)
		fprintf(logfp, "\n\n");

	CFilePath outputDbFile = getExportDBFilePath(side);
	CString outputDbFolder = outputDbFile.getDirectoryPath();

	if (fileExists(outputDbFolder) && isFolder(outputDbFolder))
	{
		// Transfer data for legitimate parts
		for (int i = 0; i < catModArray->getSize(); i++)
		{
			CSonyCatMod *cm = catModArray->getAt(i);
			int mastercat = cm->getMasterCategory();
			int mastermod = cm->getMasterModel();

			if (MasterDatabase->isValidCatModCombo(mastercat, mastermod))
			{
				CString masterBmpFilename = MasterDatabase->getBitmapFilename(mastercat, mastermod);

				// Copy file and change name if master is not NoneImage.bmp
				if (masterBmpFilename.CompareNoCase("NoneImage.bmp") != 0)
				{
					CString masterDbLocation = MasterDatabase->getLocation();
					CFilePath masterBmpFilePath(masterDbLocation);
					masterBmpFilePath.setExtension("");
					masterBmpFilePath.setBaseFileName(masterBmpFilename);

					CString masterBmp = masterBmpFilePath.getPath();

					CString destBmpFilename;
					destBmpFilename.Format("Model(%d)(%d)", cm->getMappedCategory(), cm->getMappedModel());
						

					if (!fileExists(masterBmp))
					{
						missingMasterBmp = true;

						if (logfp != NULL)
						{
							fprintf(logfp, "Missing Master BMP file \"%s\", can not create output image data \"%s\".",
								masterBmpFilename, destBmpFilename);
						}

					}
					else
					{
						// Copy file

						CFilePath destBmpFilePath(outputDbFile); // start with clone

						destBmpFilePath.setBaseFileName(destBmpFilename);
						destBmpFilePath.setExtension("bmp");

						CString destBmp = destBmpFilePath.getPath();

						if (!File_Copy(masterBmp, destBmp))
						{
							copyFailed = true;

							if (logfp != NULL)
							{
								fprintf(logfp, "Copying of Master BMP file \"%s\" to output image data \"%s\" failed.",
									masterBmp, destBmp);
							}
						}

					}
				}
			}
		}

		// Transfer "NoneImage" 
		// This file may or may not exist. Copy it if it does.
		// Do not report any error if it does not.
		{
			CString masterDbLocation = MasterDatabase->getLocation();
			CFilePath masterBmpFilePath(masterDbLocation);
			masterBmpFilePath.setExtension("bmp");
			masterBmpFilePath.setBaseFileName("NoneImage");

			CString masterBmp = masterBmpFilePath.getPath();


			CFilePath destBmpFilePath(outputDbFile);
			destBmpFilePath.setBaseFileName("NoneImage");
			destBmpFilePath.setExtension("bmp");

			CString destBmp = destBmpFilePath.getPath();


			File_Copy(masterBmp, destBmp);
		}

		if (logfp != NULL)
			fprintf(logfp, "\n");
	}

	CString msg;

	if (missingMasterBmp)
		msg += "\nOne or more Master BMP Image Data files are missing in Master Database.";

	if (copyFailed)
		msg += "\nCopying of one or more existing Master BMP Image Data files failed.";

	if (!msg.IsEmpty())
	{
		CString caption = "Sony AOI Export - Side: ";
		caption += (side == sideTop ? "Top" : "Bottom");
		MessageBox(NULL, msg, caption, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CSonyBoard::createBoardSideSonyDB(PcbSide side)
{
	// Create board side specific Sony Database

	CSonyCatModArray *catModArray = (side == sideTop ? &m_topCatModMap : &m_botCatModMap);

	CFilePath outputDbFile = getExportDBFilePath(side);
	CString outputDbFolder = outputDbFile.getDirectoryPath();

	if (fileExists(outputDbFolder))
	{
		if (!isFolder(outputDbFolder))
		{
			CString msg;
			msg = "The name \"";
			msg += outputDbFolder;
			msg += "\" is already in use for a regular file. A directory of the same name can not be created for the board specific Database output.";
			ErrorMessage(msg, "Sony AOI Can Not Create Database Folder");
			return;
		}
	}
	else
	{
		// Create output dir. The Sony AOI DB utils will not create the directory
		if (MakeDirectory(outputDbFolder) != 0)
		{
			CString msg;
			msg = "Can not create the Database output folder \"";
			msg += outputDbFolder + "\".";
			ErrorMessage(msg, "Sony AOI Can Not Create Database Folder");
			return;		
		}
	}

	// Now write the DB file, replace it if is already exists, already have permission
	CString outputDB = outputDbFile.getPath();

	if (fileExists(outputDB)) DeleteFile(outputDB);

	if (logfp != NULL)
	{
		fprintf(logfp, "\nCreating board side database: %s\n", outputDB);
	}
	// 1. Create the DB
	// Must have a master to seed it.
	// 1.a. Copies table LibProfile from master database to created database.
	// 1.b Creates new (empty) LibCatList table in created database.
	if (MasterDatabase != NULL && !MasterDatabase->getLocation().IsEmpty())
		CreateDB(MasterDatabase->getLocation(), MasterDatabase->getPassword(), outputDB);
	else
		return /*fail*/;

	// Transfer data for legitimate parts
	for (int i = 0; i < catModArray->getSize(); i++)
	{
		CSonyCatMod *cm = catModArray->getAt(i);
		int mastercat = cm->getMasterCategory();
		int mastermod = cm->getMasterModel();

		if (MasterDatabase->isValidCatModCombo(mastercat, mastermod))
		{
			int mappedcat = cm->getMappedCategory();
			int mappedmod = cm->getMappedModel();

			CString strMastercat, strMastermod, strMappedcat, strMappedmod;
			strMastercat.Format("%d", mastercat);
			strMastermod.Format("%d", mastermod);
			strMappedcat.Format("%d", mappedcat);
			strMappedmod.Format("%d", mappedmod);
			
			if (logfp!=NULL)
			{
				fprintf(logfp, "   Master(%d)(%d)  -->  Mapped(%d)(%d)\n",
				mastercat, mastermod, mappedcat, mappedmod);
			}
			// 2. Make entry in new LibCatList.
			PutLibCatList(MasterDatabase->getLocation(), MasterDatabase->getPassword(), strMastercat, outputDB,  strMappedcat); 

			// 3. Create new LibMod(<mappedcat>) table in new database.
			CreateLibModTable(outputDB, MasterDatabase->getPassword(), strMappedcat);
			PutLibModData(MasterDatabase->getLocation(), MasterDatabase->getPassword(), strMastercat, strMastermod, outputDB,  strMappedcat, strMappedmod);

			// 4. Copy table LibPro(<mastercat>)(<mastermod>) from master database to
			// table LibPro(<mappedcat>)(<mappedmod>) in new database.
			// No need to change content of table.
			CString tablename;
			tablename.Format("LibPro(%s)(%s)", strMastercat, strMastermod);
			if (TableExists(MasterDatabase->getLocation(), MasterDatabase->getPassword(), tablename))
            CreateAndCopyTableLibPro(MasterDatabase->getLocation(), MasterDatabase->getPassword(), strMastercat, strMastermod, outputDB,  strMappedcat, strMappedmod);
			else
				if (logfp != NULL) fprintf(logfp, "   Error: Missing table %s in master database.\n", tablename);
		}
	}

	// Create DB entries for "Dummy" tests
	// Only one "dummy" category, one entry good for all dummies in catModArray loop 
	// that follows.
	if (catModArray->getDummyCat() > 0)
	{
		CString strDummyCat;
		strDummyCat.Format("%d", catModArray->getDummyCat());
		AppendToLibCatList(outputDB, strDummyCat, "2",  "DUMMY", MasterDatabase->getPassword()); 
	}
	int dummyOrderNum = 1;

	for (int i = 0; i < catModArray->getSize(); i++)
	{
		CSonyCatMod *cm = catModArray->getAt(i);
		int mastercat = cm->getMasterCategory();
		int mastermod = cm->getMasterModel();

		if (!MasterDatabase->isValidCatModCombo(mastercat, mastermod))
		{
			int mappedcat = cm->getMappedCategory();
			int mappedmod = cm->getMappedModel();

			CString strMastercat, strMastermod, strMappedcat, strMappedmod;
			strMastercat.Format("%d", mastercat);
			strMastermod.Format("%d", mastermod);
			strMappedcat.Format("%d", mappedcat);
			strMappedmod.Format("%d", mappedmod);

			if (logfp != NULL) 
			{	
				fprintf(logfp, "   Master(%d)(%d)  -->  Mapped(%d)(%d)  Dummy\n",
				mastercat, mastermod, mappedcat, mappedmod);
			}
			// LibMod table
			CreateLibModTable(outputDB, MasterDatabase->getPassword(), strMappedcat);
			CString strOrderNum;
			strOrderNum.Format("%d", dummyOrderNum++);
			AppendToLibMod(outputDB, MasterDatabase->getPassword(), strMappedcat, strMappedmod, strOrderNum, "DUMMY_" + strMappedmod, "0", "NoneImage.BMP");

			// LibPro tabe
			CString libproTableName;
			libproTableName.Format("LibPro(%d)(%d)", mappedcat, mappedmod);
			CreateLibProTable(outputDB, MasterDatabase->getPassword(), libproTableName);

			int lengthPixels = 255; // X  Dummy test, need a real value?
			int heightPixels = 127; // Y  Dummy test, need a real value?
			CString strProcNo = "1";
			CString strProcClass = "11";
			CString strProcDataFormat = "0,0,0,0,0,0,0,0,0,0,%d,0,0,0,%d,0,0,,1,,,,,,,,,,,";
			CString strProcData;
			strProcData.Format(strProcDataFormat, lengthPixels, heightPixels);
			AppendToLibPro(outputDB, MasterDatabase->getPassword(), strMappedcat, strMappedmod, strProcNo, strProcClass, strProcData);

			strProcNo = "2";
			strProcClass = "73";
			strProcDataFormat = "0,0,0,,0,0,,0,0,,0,,0,,0,%c,0,%c,0,,,,,,,,,,,";
			char mysteryvalue = ((SonyAoiSettings->getDummyType() == SONYAOI_DUMMYTYPE_OK_END) ? '0' : '1');
			strProcData.Format(strProcDataFormat, mysteryvalue, mysteryvalue);
			AppendToLibPro(outputDB, MasterDatabase->getPassword(), strMappedcat, strMappedmod, strProcNo, strProcClass, strProcData);
		}
	}
}

bool CSonyBoard::getOverwritePermission()
{
	// If any one of the various outputs exists, get overwrite permission
	// for the whole lot.

	CFilePath toptxtfile = getExportTxtFilePath(sideTop);
	bool toptxtexists = fileExists(toptxtfile.getPath());

	CFilePath bottxtfile = getExportTxtFilePath(sideBottom);
	bool bottxtexists = fileExists(bottxtfile.getPath());

	CFilePath topdbfile = this->getExportDBFilePath(sideTop);
	bool topdbexists = fileExists(topdbfile);

	CFilePath botdbfile = this->getExportDBFilePath(sideBottom);
	bool botdbexists = fileExists(botdbfile);


	if ((getPartCount(sideTop) > 0 && (toptxtexists || topdbexists)) ||
		(getPartCount(sideBottom) > 0 && (bottxtexists || botdbexists)))
	{
		CString boardname = this->getBoardName();
		CString msg;
		msg.Format("One or more output files for \"%s\" already exist. Continuing this export will overwrite these files.",
			boardname);
		
		int res = MessageBox(NULL, msg, "Sony AOI Export", MB_OKCANCEL | MB_ICONEXCLAMATION);
		if (res != IDOK)
		{
			return false;
		}
	}

	return true;
}

CFilePath CSonyBoard::getExportDBFilePath(PcbSide side)
{
	CString polishedBoardName = this->getBoardName();
	polishedBoardName.Replace('.','_');

	CString boardsideDbName = polishedBoardName;
	boardsideDbName += ((side == sideTop) ? "_top" : "_bot"); 


	CFilePath dbfile(getExportFolder());
	dbfile.pushLeaf("LIB");  // user wants a "LIB" level in the folder hierarchy

	dbfile.pushLeaf(boardsideDbName); // add a folder with same name as DB file

	dbfile.pushLeaf("workaround"); // works around an inconvenience, arguably a bug, in CFilePath
	// The problem is the previous pushed leaf is getting set as the BaseFileName upon the
	// next parse() call, which gets called a lot, and then the following setBaseFileName()
	// call will replace it. Hence we lose a level in the tree.
	// Essentially, CFilePath is too clever for its own (or at least my own) good.
	// It can't handle a file in a folder of the same name.

	dbfile.setBaseFileName(boardsideDbName); // the actual db file name
	dbfile.setExtension("lib");  // this is what spec calls for, not MDB as is natural

	return dbfile;
}

CFilePath CSonyBoard::getExportTxtFilePath(PcbSide side)
{
	CString folder = getExportFolder();
	CString filename = getBoardName();

	filename.Replace('.', '_');

	if (side == sideTop)
		filename += "_top";
	else
		filename += "_bot";

	CFilePath outfilepath(folder);
	outfilepath.setBaseFileName(filename);
   outfilepath.setExtension("txt");

	return outfilepath;
}

bool CSonyBoard::exportAll()
{
	if (!getOverwritePermission())
		return false;

	// Just to make sure they're all there
	generate_CENTROIDS(m_doc);

	// Experimental
	m_packageOutlineMissing = false;

	// If user didn't go to Xref tab, then fids never got added to "unique parts"
	// list, and will not end up being output. So make sure they are in the list now.
	gatherFiducials();

	// A finer grained progress bar might be nice. Maybe later. For now
	// just show major steps.
	CProgressDlg localProgressGauge;
	CProgressDlg *progressGauge = &localProgressGauge;
	progressGauge->caption = "Sony AOI Export";
   progressGauge->Create();
   progressGauge->SetStatus("Exporting text and database ...");
	progressGauge->ShowWindow(SW_SHOW);
	progressGauge->BringWindowToTop();
	progressGauge->SetRange(0, 6);
	progressGauge->SetPos(0);
	progressGauge->SetStep(1);
	
	logUniqueParts("EXPORT ALL");

	mapBoardSideCategoryModel();

#ifdef DEBUG_CAT_MOD_MAP
	getTopCatModMap().writeLogFile("Top");
	getBotCatModMap().writeLogFile("Bottom");
#endif

	if (getPartCount(sideTop) > 0)
	{
		writeTopTxt();								progressGauge->StepIt();
		createBoardSideSonyDB(sideTop);		progressGauge->StepIt();
		transferImageData(sideTop);			progressGauge->StepIt();
	}
	else
	{
		progressGauge->StepIt();
		progressGauge->StepIt();
		progressGauge->StepIt();
	}

	if (getPartCount(sideBottom) > 0)
	{
		writeBottomTxt();								progressGauge->StepIt();
		createBoardSideSonyDB(sideBottom);		progressGauge->StepIt();
		transferImageData(sideBottom);			progressGauge->StepIt();
	}
	else
	{
		progressGauge->StepIt();
		progressGauge->StepIt();
		progressGauge->StepIt();
	}

	savePartNumberCrossReference();

	if (logfp != NULL) 
	{
		fprintf(logfp, "\n-------------------------------------------------\n");
		fflush(logfp);
	}


	//progressGauge.ShowWindow(SW_HIDE);
	
	if (m_packageOutlineMissing)
	{
		MessageBox(NULL, "One or more parts set to be Exploded did not have Package Outlines for adjusting pin locations to package edge. These parts are not Exploded. See log file for list.",
			"Sony AOI Export", MB_ICONEXCLAMATION | MB_OK);
	}

	return true; // success
}

bool CSonyBoard::partIsLoaded(DataStruct *partData)
{
	// Return false if part has attribute LOADED==false.
	// Anything else is treated as LOADED==true, thus, default is true if attrib is not present.

	CString val;
	Attrib *a;

	if ((a = is_attvalue(m_doc, partData->getAttributesRef(),ATT_LOADED, 0)) != NULL)
	{
      val = get_attvalue_string(m_doc, a);
   }

	if (val.CompareNoCase("no") == 0 || val.CompareNoCase("false") == 0)
		return false;

	return true;
}
//====================================================================================

CSonyPart::CSonyPart(CCEtoODBDoc *doc, DataStruct *partdata)
{
	m_doc = doc;
	m_partData = partdata;

	m_masterCategory = 0;
	m_masterModel    = 0;

	m_isOnTop = false;
	m_isOnBot = false;

	m_isFid = (partdata != NULL && partdata->isInsertType(insertTypeFiducial));

	m_isUsed = true;

	m_save = false;
	m_explode = false;
	m_angleOffsetDegrees = 0;

}

CString CSonyPart::getPartNumber()
{
	// might consider caching partnumber in constructor for speed, right now 
	// we'll leave the late binding and continual lookup on the fly

	CString partnumber;

	
#ifdef USE_LOCAL_CODE
	if (m_isFid)
	{
		// Use inserted geom name for fid part number, with prefix "Fid" added
		int insertedBlockNum = m_partData->getInsert()->getBlockNumber();
		BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);
		CString fidBlockName = insertedBlock->getName();
		partnumber.Format("(Fid) %s", fidBlockName);
	}
	else
	{
		// PCB_COMPONENT
		Attrib *a;

		if ((a = is_attvalue(m_doc, m_partData->getAttributesRef(),ATT_PARTNUMBER, 0)) != NULL)
		{
			partnumber = get_attvalue_string(m_doc, a);
		}
	}
#else
	partnumber = sonyAoiGetDatastructPartnumber(m_doc, m_partData);
#endif
	return partnumber;
}

void CSonyPart::determineFramesize(double *frameSizeX, double *frameSizeY)
{
	*frameSizeX = 1.0;
	*frameSizeY = 1.0;

	DataStruct *compData = this->getData();

	if (compData != NULL)
	{
		if (MasterDatabase != NULL && 
			MasterDatabase->isValidCatModCombo(this->getMasterCategory(), this->getMasterModel()))
		{
			// Get frame size from master database
			int pixelsX, pixelsY;
			MasterDatabase->getFrameSize(this->getMasterCategory(), this->getMasterModel(), &pixelsX, &pixelsY);
			double micronsPerPixel = SonyAoiSettings->getPixelConversion();
			*frameSizeX = pixelsX * micronsPerPixel / 1000.0; // ends up with size in mm
			*frameSizeY = pixelsY * micronsPerPixel / 1000.0;
		}
		else
		{
			// Get frame size from pin extents, i.e. extent of all pads.
			// But only if not exploded. If exploded we get extent per pin when
			// pins are output. If not exploded, use extent of all pins.

			int insertedBlockNum = compData->getInsert()->getBlockNumber();
			BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);

			if (insertedBlock != NULL)
			{
				double xmin, ymin, xmax, ymax;
				int pinCount = GetPinOuterExtents(m_doc, &insertedBlock->getDataList(), &xmin, &ymin, &xmax, &ymax);
				if (pinCount > 0)
				{
					*frameSizeX = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (xmax - xmin));
					*frameSizeY = m_doc->convertPageUnitsTo(pageUnitsMilliMeters, (ymax - ymin));
				}
			}
		}

		// Default size back to 1 in case DB entry or extents were 0.
		if (*frameSizeX == 0.0 || *frameSizeY == 0.0)
			*frameSizeX = *frameSizeY = 1.0;
	}
}

CString CSonyPart::getBitmap()
{
	if (MasterDatabase != NULL)
	{
		CString filename = MasterDatabase->getBitmapFilename(this->m_masterCategory, this->m_masterModel);
		return filename;
	}

	return "";
}

CString CSonyPart::getMasterCategoryName()
{
	if (MasterDatabase != NULL)
	{
		CString name = MasterDatabase->getCategoryName(this->m_masterCategory);
		return name;
	}

	return "";
}

CString CSonyPart::getMasterModelName()
{
	if (MasterDatabase != NULL)
	{
		CString name = MasterDatabase->getModelName(m_masterCategory, m_masterModel);
		return name;
	}

	return "";
}

CString CSonyPart::getAngleOffsetString()
{
	CString str;
	str.Format("%d", this->getAngleOffset());

	return str;
}

SonyPartStatusTag CSonyPart::getStatus()
{ 
	if (getMasterCategory() == 0 && this->getMasterModel() == 0)
	{
		// Both zero, part is Unassigned
		return sonypartUnassigned;
	}
	else
	{
		// One or other, or both, are non-zero.
		// If combination of cat/mod is in database, then part is Assigned.
		// If not, then part is Unassigned.
		if (MasterDatabase != NULL)
		{
			if (MasterDatabase->isValidCatModCombo(m_masterCategory, m_masterModel))
				return sonypartAssigned;
			else
				return sonypartUnassigned;
		}
		else
		{
			// No active database, status is indeterminate, call it an error
			return sonypartError;
		}
	}

	return sonypartError;
}

CString CSonyPart::getStatusText()
{
	switch (getStatus())
	{
	case sonypartUnassigned:
		return "Unassigned";

	case sonypartAssigned:
		return "Assigned";

	case sonypartError:
		return "Error";
	}
	
	return "Unknown";
}

//------------------------------------------------------------------------------

CSonyPartArray::CSonyPartArray(CCEtoODBDoc *doc, int size) : CTypedPtrArrayWithMapContainer<CSonyPart>(size)
{
		m_doc = doc;
}


CSonyPart* CSonyPartArray::getOrAdd(DataStruct *partdata)
{
	CString partnumber;


	partnumber = sonyAoiGetDatastructPartnumber(m_doc, partdata);


	CSonyPart* part = NULL;

	if (!partnumber.IsEmpty() && !lookup(partnumber, part))
   {
      int index = getSize();
      part = new CSonyPart(m_doc, partdata);
      setAt(index, partnumber, part);

		if (logfp != NULL) fprintf(logfp, "Found part number %s\n", partnumber);
   }

	if (part != NULL)
	{
		// Set side. This is not an either/or, it is cummulative.
		// We want to know if there are instances on the top and if there are
		// instances on the bottom.
		if (partdata->getInsert()->getPlacedTop())
			part->setIsOnTop(true);
		else
			part->setIsOnBottom(true);
	}

   return part;
}


CSonyPart* CSonyPartArray::get(DataStruct *partdata)
{
	CString partnumber;


	partnumber = sonyAoiGetDatastructPartnumber(m_doc, partdata);


   CSonyPart* part = NULL;

	if (!partnumber.IsEmpty()) lookup(partnumber, part);

   return part;
}

//======================================================================================

CSonyCatModArray::CSonyCatModArray(int size) : CTypedPtrArrayWithMapContainer<CSonyCatMod>(size)
{
	m_dummyCat = -1;  // not assigned, i.e. no dummy category
}


CSonyCatMod* CSonyCatModArray::add(const int category, const int model)
{
	CString key;
	key.Format("(%d)(%d)", category, model);

	CSonyCatMod *catmod = NULL;

	if (!lookup(key, catmod))
   {
      int index = getSize();
      catmod = new CSonyCatMod(category, model);
      setAt(index, key, catmod);
   }

   return catmod;
}

CSonyCatMod* CSonyCatModArray::get(const int category, const int model)
{
	CString key;
	key.Format("(%d)(%d)", category, model);

	CSonyCatMod *catmod = NULL;

	lookup(key, catmod);

   return catmod;
}
	
void CSonyCatModArray::getMappedCategoryModel(const int mastercat, const int mastermod, int *mappedcat,int *mappedmod)
{
	*mappedcat = 0;
	*mappedmod = 0;

	CSonyCatMod *cm = get(mastercat, mastermod);
	if (cm != NULL)
	{
		*mappedcat = cm->getMappedCategory();
		*mappedmod = cm->getMappedModel();
	}
}

int CSonyCatModArray::getLowestMasterCategory()
{
	// Categories are 0 or greater, -1 means none found

	int lowest = (this->getSize() > 0) ? this->getAt(0)->getMasterCategory() : -1;

	for (int i = 0; i < this->getSize(); i++)
	{
		CSonyCatMod *cm = this->getAt(i);
		if (cm->getMasterCategory() < lowest) lowest = cm->getMasterCategory();
	}

	return lowest;
}

int CSonyCatModArray::getHighestMasterCategory()
{
	// Categories are 0 or greater, -1 means none found

	int highest = (this->getSize() > 0) ? this->getAt(0)->getMasterCategory() : -1;

	for (int i = 0; i < this->getSize(); i++)
	{
		CSonyCatMod *cm = this->getAt(i);
		if (cm->getMasterCategory() > highest) highest = cm->getMasterCategory();
	}

	return highest;
}

int CSonyCatModArray::getNextMasterCategory(int currentMasterCategory)
{
	// Categories are 0 or greater, -1 means none found
	// Legit categories are greater than 0, 0 is the "dummy" category.
	
	int nextcat = -1;
	
	int highlimit = getHighestMasterCategory();

	for (int i = 0; i < this->getSize(); i++)
	{
		CSonyCatMod *cm = this->getAt(i);
		if (cm->getMasterCategory() > currentMasterCategory && cm->getMasterCategory() <= highlimit)
		{
			nextcat = cm->getMasterCategory();
			highlimit = nextcat;
		}
	}

	return nextcat;
}

int CSonyCatModArray::getHighestMappedCategory()
{
	// Categories are 0 or greater, -1 means none found

	int highest = (this->getSize() > 0) ? this->getAt(0)->getMappedCategory() : -1;

	for (int i = 0; i < this->getSize(); i++)
	{
		CSonyCatMod *cm = this->getAt(i);
		if (cm->getMappedCategory() > highest) highest = cm->getMappedCategory();
	}

	return highest;
}

void CSonyCatModArray::createConsecutiveMapping()
{
	// Assign new consecutive categories
	// Master Category zero (0) is the special case for "dummy". It has to be
	// mapped last, to one greater than the max legit mapped category.


	// Do regular Assigned mapping
	int mappedcat = 1;
	for (int mastercat = getLowestMasterCategory(); mastercat > -1; mastercat = getNextMasterCategory(mastercat))
	{
		if (mastercat != 0)
		{
			for (int i = 0; i < this->getSize(); i++)
			{
				CSonyCatMod *cm = this->getAt(i);
				if (cm->getMasterCategory() == mastercat) cm->setMappedCategory(mappedcat);
			}
			mappedcat++;
		}
	}


	// Do special Unassigned "Dummy" category mapping
	m_dummyCat = mappedcat; // needed later to know which is dummy, if any
	for (int i = 0; i < this->getSize(); i++)
	{
		CSonyCatMod *cm = this->getAt(i);
		if (cm->getMasterCategory() == 0) cm->setMappedCategory(mappedcat);
	}

	// Within each category, assign new consecutive models
	for (int mappedcat = 1; mappedcat <= getHighestMappedCategory(); mappedcat++)
	{
		int mappedmodel = 1;
		for (int i = 0; i < this->getSize(); i++)
		{
			CSonyCatMod *cm = this->getAt(i);
			if (cm->getMappedCategory() == mappedcat) cm->setMappedModel(mappedmodel++);
		}
	}

}

void CSonyCatModArray::writeLogFile(CString side)
{
	if (logfp != NULL)
	{
		fprintf(logfp, "\n%s Side Category/Model Map (%d Entries)\n", side, this->getSize());
		for (int i = 0; i < this->getSize(); i++)
		{
			CSonyCatMod *cm = this->getAt(i);
			fprintf(logfp, "   Master (%d)(%d) mapped to (%d)(%d)\n",
				cm->getMasterCategory(), cm->getMasterModel(), cm->getMappedCategory(), cm->getMappedModel());
		}
		fprintf(logfp, "\n");
	}
}

//======================================================================================

CSonyDatabaseCatModNameArray::CSonyDatabaseCatModNameArray(int size) : CTypedPtrArrayWithMapContainer<CSonyDatabaseCatModName>(size)
{

}


void CSonyDatabaseCatModNameArray::add(const int catnum, const int modnum, CString catname, CString modname)
{
	// Put it in with an assortment of key arrangements to support various lookups

	CSonyDatabaseCatModName *catmod = NULL;
	CString key;

	// For integer cat/mod lookup
	key.Format("(%d)(%d)", catnum, modnum);
	catmod = NULL;
	if (!lookup(key, catmod))
   {
      int index = getSize();
      catmod = new CSonyDatabaseCatModName(catnum, modnum, catname, modname, catmodKeyNumber);
      setAt(index, key, catmod);
   }

	// For name string cat/mod lookup
	key.Format("(%s)(%s)", catname, modname);
	catmod = NULL;
	if (!lookup(key, catmod))
   {
      int index = getSize();
      catmod = new CSonyDatabaseCatModName(catnum, modnum, catname, modname, catmodKeyName);
      setAt(index, key, catmod);
   }

	// For lookup of just category by number, put in with special model
	key.Format("(%d)(%d)", catnum, ICategoryOnly);
	catmod = NULL;
	if (!lookup(key, catmod))
   {
      int index = getSize();
      catmod = new CSonyDatabaseCatModName(catnum, modnum, catname, QCategoryOnly, catmodKeyCategoryOnlyNum);
      setAt(index, key, catmod);
   }

	// For lookup of just category by name, put in with special model
	key.Format("(%s)(%s)", catname, QCategoryOnly);
	catmod = NULL;
	if (!lookup(key, catmod))
   {
      int index = getSize();
      catmod = new CSonyDatabaseCatModName(catnum, modnum, catname, QCategoryOnly, catmodKeyCategoryOnlyStr);
      setAt(index, key, catmod);
   }
}

CSonyDatabaseCatModName* CSonyDatabaseCatModNameArray::get(const int category, const int model)
{
	CString key;
	key.Format("(%d)(%d)", category, model);

	CSonyDatabaseCatModName *catmod = NULL;

	lookup(key, catmod);

   return catmod;
}

CSonyDatabaseCatModName* CSonyDatabaseCatModNameArray::get(CString category, CString model)
{
	CString key;
	key.Format("(%s)(%s)", category, model);

	CSonyDatabaseCatModName *catmod = NULL;

	lookup(key, catmod);

   return catmod;
}

//======================================================================================

CSonyDatabase::CSonyDatabase(CString location, CString password) 
				:	m_catModNames(0)
{
	m_db_location = location;
	m_db_password = password;

	loadCatModNames();

	// One pays a big price in GUI startup time by validating all cat/mod combos in advance.
	// In one benchmark, startup went from 5 secs to 12 secs.
	// So it is turned off.
	// Cat/Mod name combos will be validated on the fly as they are accessed.
	///validateCatModNames();
}

void CSonyDatabase::loadCatModNames(CString catnumname)
{
	int separatorPos = catnumname.Find(';');
	CString catnumstr = catnumname.Left(separatorPos);
	CString catname = catnumname.Right(catnumname.GetLength() - separatorPos - 1);
	int catnum = atoi(catnumstr);


	CString modnames;
	GetAllModNames(m_db_location, m_db_password, catnumstr, modnames);

	int start = 0;
	CString oneName = modnames;
	while (!oneName.IsEmpty())
	{
		int sep = oneName.Find((char)5);
		if (sep > 0) oneName.Truncate(sep);

		separatorPos = oneName.Find(';');
		CString modnumstr = oneName.Left(separatorPos);
		CString modname = oneName.Right(oneName.GetLength() - separatorPos - 1);
		int modnum = atoi(modnumstr);

		m_catModNames.add(catnum, modnum, catname, modname);

		start += oneName.GetLength() + 1;	
		oneName = modnames.Right(modnames.GetLength() - start);
	}
}

void CSonyDatabase::loadCatModNames()
{
	CString catnames;
	GetAllCatNames(m_db_location, m_db_password, catnames);

	int start = 0;
	CString oneName = catnames;
	while (!oneName.IsEmpty())
	{
		int sep = oneName.Find((char)5);
		if (sep > 0) oneName.Truncate(sep);
		loadCatModNames(oneName);
		start += oneName.GetLength() + 1;	
		oneName = catnames.Right(catnames.GetLength() - start);
	}
}

void CSonyDatabase::validateCatModNames()
{
	for (int i = 0; i < this->m_catModNames.getSize(); i++)
	{
		CSonyDatabaseCatModName *cmname = m_catModNames.getAt(i);
		if (cmname->getKeyType() == catmodKeyNumber || cmname->getKeyType() == catmodKeyName)
		{
			isValidCatModCombo(cmname); // this caches the determination, which requires a master DB lookup
		}
	}	

}

CString CSonyDatabase::getBitmapFilename(int category, int model)
{
	CString bitmapname;

	if (isValidCatModCombo(category, model))
	{
		CString strCatid, strModid;
		strCatid.Format("%d", category);
		strModid.Format("%d", model);
		GetBitMapName(m_db_location, m_db_password, strCatid, strModid, bitmapname);
	}

	return bitmapname;
}

void CSonyDatabase::getFrameSize(int category, int model, int *xsize, int *ysize)
{
	*xsize = 0;
	*ysize = 0;

	if (isValidCatModCombo(category, model))
	{
		CString strCatid, strModid;
		strCatid.Format("%d", category);
		strModid.Format("%d", model);
		CString strXSize;
		CString strYSize;
		GetFrameSize(m_db_location, m_db_password, strCatid, strModid, strXSize, strYSize);

		*xsize = atoi(strXSize);
		*ysize = atoi(strYSize);
	}
}

int CSonyDatabase::getCategoryNumber(CString catname)
{
	CSonyDatabaseCatModName *cmname = m_catModNames.get(catname, QCategoryOnly);
	if (cmname != NULL) return cmname->getCatNum();
	return 0;
}

CString CSonyDatabase::getCategoryName(int category)
{
	CSonyDatabaseCatModName *cmname = m_catModNames.get(category, ICategoryOnly);
	if (cmname != NULL) return cmname->getCatName();
	return "Dummy";
}
	
CString CSonyDatabase::getCategoryNameOptionList()
{
	CString optionDelimiter = "\n";
   CString optionList("Dummy" + optionDelimiter);

	for (int i = 0; i < this->m_catModNames.getSize(); i++)
	{
		CSonyDatabaseCatModName *cmname = m_catModNames.getAt(i);
		if (cmname->getKeyType() == catmodKeyCategoryOnlyStr)
		{
			optionList += cmname->getCatName() + optionDelimiter;  // Leave the last trailing delimiter, it is required by UG droplist interface.
		}
	}

	return optionList;
}

int CSonyDatabase::getModelNumber(int category, CString modelname)
{
	CString catname = getCategoryName(category);
	CSonyDatabaseCatModName *cmname = m_catModNames.get(catname, modelname);
	if (cmname != NULL) return cmname->getModNum();
	return 0;
}

CString CSonyDatabase::getModelName(int category,int model)
{
	CSonyDatabaseCatModName *cmname = m_catModNames.get(category, model);
	if (cmname != NULL) return cmname->getModName();
	return "Dummy";
}

CString CSonyDatabase::getModelNameOptionList(int category)
{
	CString optionDelimiter = "\n";
   CString optionList( "Dummy" + optionDelimiter );

	for (int i = 0; i < m_catModNames.getSize(); i++)
	{
		CSonyDatabaseCatModName *cmname = m_catModNames.getAt(i);
		if (cmname->getCatNum() == category && cmname->getKeyType() == catmodKeyNumber)
		{
			if (isValidCatModCombo(cmname))
			{
				optionList += cmname->getModName() + optionDelimiter;  // We want to keep the last trailing delimiter, it is required by UG droplist interface.
			}
		}
	}

	return optionList;
}

bool CSonyDatabase::isValidCatModCombo(int category, int model)
{
	// A category/model combo is valid if it exists in the database
	// The category would be listed in the LibCatList table.
	// The model would be listed in the LibMod(cat) table.
	// And the table LibPro(cat)(mod) must exist.

	CSonyDatabaseCatModName *cmname = m_catModNames.get(category, model);

	return isValidCatModCombo(cmname);
}


bool CSonyDatabase::isValidCatModCombo(CSonyDatabaseCatModName *cmname)
{
	// A category/model combo is valid if it exists in the database
	// The category would be listed in the LibCatList table.
	// The model would be listed in the LibMod(cat) table.
	// And the table LibPro(cat)(mod) must exist.

	if (cmname != NULL)
	{
		int tableOK = cmname->getLibProExists();
		if (tableOK < 0) // -1 means it is not set
		{
			CString tablename;
			tablename.Format("LibPro(%d)(%d)", cmname->getCatNum(), cmname->getModNum());
			tableOK = TableExists(m_db_location, m_db_password, tablename);
			cmname->setLibProExists(tableOK); // cache value
		}

		return (tableOK == 1);
	}

	return false;
}

//======================================================================================

CSonyRsiXref::CSonyRsiXref(CString databaseLocation)
{
	// Open cross-reference DB

	if (databaseLocation.IsEmpty()) {
		// Set to default location
		m_db_location = getApp().getUserPath() + "SonyAoiXref.MDB";
	}
	else
	{
		// User's location
		m_db_location = databaseLocation;
	}

	m_db_available = fileExists(m_db_location);
}

CSonyRsiXref::~CSonyRsiXref()
{
	// Close cross-reference DB
}

bool CSonyRsiXref::get(CString partnumber, int *category, int *model, bool *explode, int *angle)
{
	// Look up partnumber in cross-ref, 
	// if found, set category and model from DB, return true
	// if not found, set category and model to 0, return false.

	CString strCat = "0", strMod = "0", strExplode = "N";
	double dangle = 0.0;
	bool foundXref = GetXREFCatModel(m_db_location, partnumber, strCat, strMod, strExplode, dangle); 

	*category = atoi(strCat);
	*model = atoi(strMod);
	*explode = (strExplode.CompareNoCase("Y") == 0 || strExplode.CompareNoCase("YES") == 0);
	*angle = (int)dangle;

	return foundXref;
}

void CSonyRsiXref::put(CString partnumber, int category, int model, bool explode, int angle)
{
	// Add or update entry for given part number with given category and model
	// Permanent change, commit now.
	static int a = 0;
	if (!m_db_location.IsEmpty())
	{
		CString strCat, strMod, strExplode;
		strCat.Format("%d", category);
		strMod.Format("%d", model);
		strExplode.Format("%s", explode ? "YES" : "NO");
		double dangle = angle;

		SaveXREFCatModel(m_db_location, partnumber, strCat, strMod, strExplode, dangle); 
	}
}

