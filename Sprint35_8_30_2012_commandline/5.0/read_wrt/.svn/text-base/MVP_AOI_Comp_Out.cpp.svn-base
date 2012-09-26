// $Header: /CAMCAD/5.0/read_wrt/MVP_AOI_Comp_Out.cpp 59    6/17/07 8:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/


#include "stdafx.h"
#include "CCEtoODB.h"
#include "float.h"
#include "ccdoc.h"
#include "extents.h"
#include "centroid.h"
#include "MVP_AOI_Out.h"
#include "MVP_AOI_Comp_Out.h"
#include "DeviceType.h"
#include "Line2d.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/******************************************************************************
* MVP_AOI_Comp_WriteFiles
*/
void MVP_AOI_Comp_WriteFiles(const char *filename, CCEtoODBDoc* Doc, FormatStruct* Format)
{
   CMvpAoiCompWriter writer(*Doc,filename,Format->getMvpAoiSettings());
   writer.write();
}

//_____________________________________________________________________________
CMvpAoiCompWriter::CMvpAoiCompWriter(CCEtoODBDoc& camCadDoc,const CString fileName,CMvpAoiSettings& mvpAoiSettings) :
   doc(camCadDoc),
   m_fileName(fileName),
   m_mvpAoiSettings(mvpAoiSettings),
   m_topGeomMap(camCadDoc),
   m_botGeomMap(camCadDoc)
{
   m_file = NULL;
}

void CMvpAoiCompWriter::write()
{
	// Prep log file
	CString logFileName = GetLogfilePath("MVP_Comp.log");
	if (!m_logFile.Open(logFileName, CFile::modeCreate|CFile::modeWrite))   // rewrite file
   {
      ErrorMessage("Can not open Logfile !", logFileName, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
	CTime t;
   t = t.GetCurrentTime();
   m_logFile.WriteString("MVP Component AOI Writer Log File\n");
   m_logFile.WriteString("%s\n\n", t.Format("Created date: %A, %B %d, %Y at %H:%M:%S"));


	// Init Variables
	unitFactor = Units_Factor(doc.getSettings().getPageUnits(), UNIT_MILS);
	m_file = NULL;

	// 1 Visible File Only
	if (!getVisibleFile(&doc, m_file))
		return;

	// Must have centroids, Mark wants it this way (case 1501++)
	// Case 1693 overrided Mark's wishes, report missing centroids to
	// log file and keep going.
	checkComponentCentroids();


	// Open Files
	if (!openFiles(m_fileName, topFile, botFile))
		return;

	// Header
	fprintf(topFile, "[MVPCAD]\n");
	fprintf(botFile, "[MVPCAD]\n");

	// Inserts
	doInserts();

	// Geometry Definitions
	m_topGeomMap.WriteDefinitions(topFile,m_mvpAoiSettings);
	m_botGeomMap.WriteDefinitions(botFile,m_mvpAoiSettings);

	// Close Files
   fclose(topFile);
   fclose(botFile);
	m_logFile.Close();

	m_topGeomMap.empty();
	m_botGeomMap.empty();
}

//---------------------------------------------------------------------------------

void CMvpAoiCompWriter::checkComponentCentroids()
{
	// If any centroids are missing, pup up a message.
	// Write component info to log file.

	int numberMissing = 0;
	int numberPresent = 0;
   for (CDataListIterator insertIterator(m_file->getBlock()->getDataList(),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct *data = insertIterator.getNext();

		if (data->getInsert()->getInsertType() == insertTypePcbComponent)
		{
			CPoint2d point;
			if (data->getInsert()->getCentroidLocation(doc.getCamCadData(), point))
			{
				numberPresent++;
			}
			else
			{
				numberMissing++;
				BlockStruct *block = doc.getBlockAt(data->getInsert()->getBlockNumber());
				m_logFile.WriteString("Component %s, insert of geometry %s found without a centroid.\n",
					data->getInsert()->getRefname(), block->getName());
			}
		}
	}

	if (numberMissing > 0)
	{
		CString msg;
		msg.Format("Missing centroid on %d part%s.\n", numberMissing, numberMissing > 1 ? "s" : "");
		msg += "Use 'Tools | PCB | Generate Component Centroid' to prepare centroids for MVP Writer.\n";
		if (numberPresent > 0)
		{
			CString msg2;
			msg2.Format("Centroid is present on %d parts, output will proceed for these parts.", numberPresent);
			msg += msg2;
		}
		else
		{
			msg += "No parts have centroid, output will be empty.";
		}
		ErrorMessage(msg, "MVP Component AOI Writer");
	}
	// else if (numberPresent == 0) then no parts will be output because there are
	// no parts at all. Could issue a message here but the situation should be
	// obvious just by looking a the drawing in camcad.
}

//-----------------------------------------------------------------------

void CMvpAoiCompWriter::doInserts()
{
	int refNum = 1, fiducialNum = 1;

	for (CDataListIterator insertIterator(m_file->getBlock()->getDataList(),dataTypeInsert);insertIterator.hasNext();)
	{
		DataStruct *data = insertIterator.getNext();

		// Case 1693, output parts only if they have centroid
		// But fiducials don't need a centroid - 12/08/05 - rcf via Mark
		bool validLocation = false;
		CPoint2d point;
		if (data->getInsert()->getCentroidLocation(doc.getCamCadData(), point))
		{
			validLocation = true;
		}
		else if (data->getInsert()->getInsertType() == insertTypeFiducial)
		{
			point = data->getInsert()->getOrigin2d();
			validLocation = true;
		}

		if (validLocation)
		{
			int x, y;
			if (data->getInsert()->getPlacedTop())
			{
				x = -convertUnit(point.x, unitFactor);
				y = convertUnit(point.y, unitFactor);
			}
			else
			{
				x = convertUnit(point.x, unitFactor);
				y = convertUnit(point.y, unitFactor);
			}

			InsertTypeTag insertType = data->getInsert()->getInsertType();

			switch (insertType)
			{
			case insertTypeFiducial:
				{
					// Ref Des
					// This change was made based on a request by ML.  2/15/2005 --MAN
					CString refDes;
					refDes.Format("Fid%d", fiducialNum++);

					CUniqueFiducialStruct* uniqueFiducialGeom = NULL;
					if (data->getInsert()->getPlacedTop())
					{
						uniqueFiducialGeom = m_topGeomMap.AddFiducailStruct(doc, data);
					}
					else
					{
						uniqueFiducialGeom = m_botGeomMap.AddFiducailStruct(doc, data);
					}

					CString output;
					output.Format("%s\t%s\t%d\t%d\t%d\t%s\n", refDes, uniqueFiducialGeom->GetName(), x, y,
						convertAngle(data->getInsert()->getAngle()), uniqueFiducialGeom->GetName());

					if (data->getInsert()->getPlacedTop())
						fprintf(topFile, output);
					else
						fprintf(botFile, output);
				}

				break;

			case insertTypePcbComponent:
				{
					Attrib* attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_LOADED, 0);

					if (attrib != NULL)
					{
						CString value = get_attvalue_string(&doc, attrib);

						if (!value.CompareNoCase("FALSE"))
							continue;
					}

					CString commentChar = "";

#if CamCadMajorVersion > 4 || (CamCadMajorVersion == 4 && CamCadMinorVersion > 4)
					attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TEST_STRATEGY, 0);
					if (attrib != NULL)
					{
						if (attrib->getStringValue().Find(ATT_VALUE_TEST_AOI) < 0)
							commentChar = "* ";
					}
#endif

					// Technology
					CString technology;
					bool isSmdFlag = false;
					bool isThFlag  = false;

					attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_TECHNOLOGY, 0);
					if (attrib != NULL)
					{
						technology = get_attvalue_string(&doc, attrib);

						if (technology.CompareNoCase("THRU") == 0)
							isThFlag   = true;
						else
							isSmdFlag  = true;
					}

					// Ref Des
					CString refDes;
					if (data->getInsert()->getRefname().GetLength() == 0)
						refDes.Format("%d", refNum++);
					else
						refDes = data->getInsert()->getRefname();

					// Geom
					CUniqueGeomStruct* uniqueGeom = NULL;
					if (data->getInsert()->getPlacedTop())
						uniqueGeom = m_topGeomMap.AddGeomStruct(doc, data);
					else
						uniqueGeom = m_botGeomMap.AddGeomStruct(doc, data);

					CString geomName = uniqueGeom->GetPackageName();

					// Partnumber
					CString partNumber;
					if (m_mvpAoiSettings.getCompFieldSixUsage() == FIELD_USE_PARTNUMBER)
					{
						partNumber = "0";
						attrib = is_attvalue(&doc, data->getAttributesRef(), ATT_PARTNUMBER, 0);
						if (attrib != NULL)
							partNumber = get_attvalue_string(&doc, attrib);
					}
					else
					{
						partNumber = geomName;
					}

					// SOICs and QFPs get special rotation handling, case 1501
					int angleDegrees = 0;
					DataStruct* centroid = uniqueGeom->GetGeom()->GetCentroidData();
					if (uniqueGeom->GetPackageType() == packageTypeSOIC ||
						uniqueGeom->GetPackageType() == packageTypeQFP)
					{
						// Get angle for non-rotated part (i.e part at 0 degrees)
						// Adjust for inserted angle
						angleDegrees = DcaRound(uniqueGeom->getPinOneQuadrantDegree() + data->getInsert()->getAngleDegrees());
					}
					else
					{
						angleDegrees = DcaRound(data->getInsert()->getAngleDegrees());
					}

					if (data->getInsert()->getPlacedTop())
					{
						angleDegrees = angleDegrees + 270;
					}
					else
					{
						// For bottom component need to do
						//   1) angleDegrees = 360 - angleDegree
						//   2) angleDegrees = angleDegrees + 270
						// which is equivalence to
						//      angleDegrees = 270 - angleDegrees
						angleDegrees = 270 - angleDegrees; 
					}
					angleDegrees = normalizeDegrees(angleDegrees);

					if ((isSmdFlag && m_mvpAoiSettings.getExportSmdComponentsFlag()) ||
						(isThFlag  && m_mvpAoiSettings.getExportThComponentsFlag())     )
					{
						CString output;
						output.Format("%s%s\t%s\t%d\t%d\t%d\t%s\n",
							commentChar, refDes, geomName, x, y, angleDegrees, partNumber);

						if (data->getInsert()->getPlacedTop())
							fprintf(topFile, output);
						else
							fprintf(botFile, output);
					}
				}

				break;
			}
		}
	}
}

/******************************************************************************/
/*  CPinStruct Class
/******************************************************************************/
CPinStruct::CPinStruct(DataStruct *data)
{
	m_pData = data;
	m_cOrientation = '\0';
}

CPinStruct::~CPinStruct()
{
	m_pData = NULL;
}

CPoint2d CPinStruct::getOrigin()
{
   CPoint2d origin;

   if (m_pData != NULL)
   {
      origin = m_pData->getInsert()->getOrigin2d();
   }

   return origin;
}

void CPinStruct::WritePinStruct(FILE* outFile)
{
}

//_____________________________________________________________________________
CPinStructList::CPinStructList(bool isContainer) :
    CTypedPtrListContainer<CPinStruct*>(isContainer)
{
	m_sPinOrientation = "";
}

CPinStructList::~CPinStructList()
{
}

int CPinStructList::GetOrientationCount()
{	
	return m_sPinOrientation.GetLength();
}

void CPinStructList::DetermineOrientation()
{
	CString orientation = "";

	POSITION pos = this->GetHeadPosition();
	while (pos != NULL)
	{
		CPinStruct* pinStruct = this->GetNext(pos);

		if (pinStruct == NULL)
			continue;

		if (orientation.Find(pinStruct->GetOrientation()) < 0)
      {
			orientation += pinStruct->GetOrientation();
      }
	}

	m_sPinOrientation = orientation;
}

double CPinStructList::getClosestNeighborDistance(CPinStruct& pinStruct)
{
   double closestDistance = DBL_MAX;

	for (POSITION pinPos = GetHeadPosition();pinPos != NULL;)
	{
		CPinStruct* neighborPinStruct = GetNext(pinPos);

      if (neighborPinStruct != & pinStruct)
      {
         CLine2d distanceLine(pinStruct.getOrigin(),neighborPinStruct->getOrigin());
         double distance = distanceLine.getLength();

         if (distance < closestDistance)
         {
            closestDistance = distance;
         }
      }
	}

   return closestDistance;
}

/*
[SOIC]
Field 1 – Package Type
Field 2 – Number of pins on each side of SOIC
Field 3 – Pad separation of pads, centroid of pads
Field 4 – Length of Pad
Field 5 – Width of Pad
Field 6 – Length of component body
Field 7 – Width of component body
Field 8 – Pitch
Field 9 – Not used, default to zero

[QFP]
Field  1 – Package Type
Field  2 – Number of leads on side 1
Field  3 – Number of leads on side 2
Field  4 – Pad separation of leads in X
Field  5 – Pad separation of leads in Y
Field  6 – Length of Pads
Field  7 – Width of Pads
Field  8 – Pitch
Field  9 – Component size in X
Field 10 – Component size in Y

*/

void CPinStructList::WriteCompEntry(FILE* outFile,CUniqueGeomStruct& uniqueGeom,CCEtoODBDoc& doc)
{
   double unitFactor = Units_Factor(doc.getSettings().getPageUnits(), UNIT_MILS);
   EPackageType packageType = uniqueGeom.GetPackageType();
   CString packageName      = uniqueGeom.GetPackageName();

   if (GetCount() > 0)
   {
		CPinStruct* pinStruct = GetHead();

		if (pinStruct != NULL)
      {
		   BlockStruct *pinGeom = doc.getBlockAt(pinStruct->GetData()->getInsert()->getBlockNumber());

		   if (pinGeom != NULL)
         {
		      CExtent pinExtent = getPinExtent(pinGeom);

		      CString output = "";

				// Get angle for non-rotated part (i.e part at 0 degrees)
            double angleDegrees = uniqueGeom.getPinOneQuadrantDegree();
            double padLength = max(pinExtent.getXsize(),pinExtent.getYsize());
            double padWidth = min(pinExtent.getXsize(),pinExtent.getYsize());

            if (packageType == packageTypeSOIC)
            {
               int sidePinCount = uniqueGeom.getSidePinCount();
               int pinSeparation = uniqueGeom.getPinSeparation();

               double compBodyLength = uniqueGeom.getXSize();
               double compBodyWidth = uniqueGeom.getYSize();
               if (angleDegrees == 90 || angleDegrees == 270)
               {
                  compBodyLength = uniqueGeom.getYSize();
                  compBodyWidth = uniqueGeom.getXSize();
               }

		         output.Format("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", 
                     packageName,
                     sidePinCount,
                     pinSeparation,
                     convertUnit(padLength, unitFactor), 
                     convertUnit(padWidth, unitFactor), 
				         convertUnit(compBodyLength, unitFactor), 
				         convertUnit(compBodyWidth, unitFactor), 
                     uniqueGeom.getPinPitch(),
                     0);
            }
            else if (packageType == packageTypeQFP)
            {
               int horizontalPinCount = uniqueGeom.getVerticalPinCount();     // we count horizontal pin as pins with the same X coordinate, where as MVP count pins with the same Y coordinate that line the X-axis
               int verticalPinCount = uniqueGeom.getHorizontalPinCount();		// we count vertical pin as pins with the same Y coordinate, where as MVP count pins with the same X coordinate that line the Y-axis
               int horizontalPinSeparation = uniqueGeom.getHorizontalPinSeparation();
               int verticalPinSeparation = uniqueGeom.getVerticalPinSeparation();
               double compBodyLength = uniqueGeom.getXSize();
               double compBodyWidth = uniqueGeom.getYSize();

               if (angleDegrees == 90 || angleDegrees == 270)
               {
                  horizontalPinCount = uniqueGeom.getHorizontalPinCount(); 
                  verticalPinCount = uniqueGeom.getVerticalPinCount();
                  horizontalPinSeparation = uniqueGeom.getVerticalPinSeparation();
                  verticalPinSeparation = uniqueGeom.getHorizontalPinSeparation();
                  compBodyLength = uniqueGeom.getYSize();
                  compBodyWidth = uniqueGeom.getXSize();
               }

		         output.Format("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t0\n", 
                     packageName,
                     horizontalPinCount,			
                     verticalPinCount,			
                     horizontalPinSeparation,
                     verticalPinSeparation,
                     convertUnit(padLength, unitFactor), 
                     convertUnit(padWidth, unitFactor), 
                     uniqueGeom.getPinPitch(),
				         convertUnit(compBodyLength, unitFactor), 
				         convertUnit(compBodyWidth, unitFactor));
            }

		      fprintf(outFile, output);
         }
      }
	}
}

void CPinStructList::WritePinList(FILE* outFile,CUniqueGeomStruct& uniqueGeom,CCEtoODBDoc& doc)
{
   double unitFactor = Units_Factor(doc.getSettings().getPageUnits(), UNIT_MILS);
   EPackageType packageType = uniqueGeom.GetPackageType();
   CString packageName      = uniqueGeom.GetPackageName();


	DbUnit centroidX = 0, centroidY = 0;
	DataStruct* centroiddata = centroid_exist_in_block(uniqueGeom.GetGeom());
	if (centroiddata != NULL)
	{
		centroidX = centroiddata->getInsert()->getOriginX();
		centroidY = centroiddata->getInsert()->getOriginY();
	}


 	int pinNum = 0;
	
	for (POSITION pos = GetHeadPosition();pos != NULL;)
	{
		CPinStruct* pinStruct = GetNext(pos);

		if (pinStruct == NULL)
			continue;
	
		BlockStruct *pinGeom = doc.getBlockAt(pinStruct->GetData()->getInsert()->getBlockNumber());

		if (pinGeom == NULL)
			continue;

		pinNum++;
		CString refName = pinStruct->GetData()->getInsert()->getRefname();
		int refNum = atoi(refName);
		if (refNum < 1)
			refNum = pinNum;

		double pinInsertX = pinStruct->GetData()->getInsert()->getOriginX();
		double pinInsertY = pinStruct->GetData()->getInsert()->getOriginY();
		double pinRelativeToCentroidX = pinInsertX - centroidX;
		double pinRelativeToCentroidY = pinInsertY - centroidY;		

		// Rotate the X & Y size of pin dependent on the rotation
		CTMatrix matrix;
		CPoint2d minPoint = getPinExtent(pinGeom).getMin();
		CPoint2d maxPoint = getPinExtent(pinGeom).getMax();	
		matrix.rotateDegrees(pinStruct->GetData()->getInsert()->getAngleDegrees());
		matrix.transform(minPoint);
		matrix.transform(maxPoint);
		double xSize = fabs(minPoint.x - maxPoint.x);
		double ySize = fabs(minPoint.y - maxPoint.y);

		CString output = "";
		output.Format("%d\t%d\t%d\t%d\t%d\t%c\n", 
				convertUnit(pinRelativeToCentroidX, unitFactor), 
				convertUnit(pinRelativeToCentroidY, unitFactor), 
				convertUnit(xSize, unitFactor), 
				convertUnit(ySize, unitFactor), 
				refNum, 
				pinStruct->GetOrientation());

		fprintf(outFile, output);
	}
}

// getPinExtent
CExtent CPinStructList::getPinExtent(BlockStruct* pinGeom)
{
	// TODO: better size calculation using the appropriate pad in the padstack
	CExtent extent = pinGeom->getExtent();

   return extent;
}

//_____________________________________________________________________________
CPinListBucket::CPinListBucket()
{
   m_pinPitch       = 0.;
   m_pinCount       = 0;
}

CPinListBucket::~CPinListBucket()
{
}

bool CPinListBucket::lookup(int key, CPinStructList **pinList)
{
	CPinStructList* pinStructList = NULL;

	bool result = (m_bucket.Lookup(key, pinStructList) != 0);

	*pinList = pinStructList;

	return result;
}

void CPinListBucket::add(int ordinate,CPinStruct* pin)
{
   CPinStructList* pinList = NULL;

   if (!lookup(ordinate,&pinList))
   {
      pinList = NULL;
   }

   if (pinList == NULL)
   {
      pinList = new CPinStructList(false);
      setAt(ordinate,pinList);
   }

   pinList->AddTail(pin);
}

void CPinListBucket::analyze(DbUnit centroidOrdinate,const char leOrientation,const char gtOrientation)
{
   m_pinPitch			= DBL_MAX;
	m_pinSeparation	= DBL_MAX;
   m_pinCount			= 0;

	double gtOrdinate = DBL_MAX;
	double leOrdinate = DBL_MAX;

	for (POSITION bucketPos = getStartPosition();bucketPos != NULL;)
	{
		int ordinate;
		CPinStructList* pinStructList = NULL;
		getNextAssoc(bucketPos, ordinate, pinStructList);

		if (pinStructList != NULL)
      {
         if (pinStructList->GetCount() > m_pinCount)
         {
            m_pinCount = pinStructList->GetCount();
         }

         int pinCount = pinStructList->GetCount();
         bool setOrientationFlag = (pinCount > 2);

		   if (pinCount >= 2)
		   {
			   char orientation;

			   if (ordinate > centroidOrdinate)
            {
				   orientation = gtOrientation;

					if (setOrientationFlag)
					{
						CPinStruct* pinStruct = pinStructList->GetHead();

						if (orientation == PIN_ORIENTATION_EAST )
							gtOrdinate = pinStruct->getOrigin().x;
						else if (orientation == PIN_ORIENTATION_NORTH)
							gtOrdinate = pinStruct->getOrigin().y;
					}
            }
			   else
            {
				   orientation = leOrientation;

					if (setOrientationFlag)
					{
						CPinStruct* pinStruct = pinStructList->GetHead();

						if (orientation == PIN_ORIENTATION_WEST)
							leOrdinate = pinStruct->getOrigin().x;
						else if (orientation == PIN_ORIENTATION_SOUTH)
							leOrdinate = pinStruct->getOrigin().y;
					}
            }
   			
			   for (POSITION pinPos = pinStructList->GetHeadPosition();pinPos != NULL;)
			   {
				   CPinStruct* pinStruct = pinStructList->GetNext(pinPos);

               if (setOrientationFlag)
               {
				      pinStruct->SetOrientation(orientation);
               }

               double closestNeighborDistance = pinStructList->getClosestNeighborDistance(*pinStruct);

               if (closestNeighborDistance < m_pinPitch)
               {
                  m_pinPitch = closestNeighborDistance;
               }
			   }
		   }
      }
	}

	if (gtOrdinate != DBL_MAX && leOrdinate != DBL_MAX)
		m_pinSeparation = fabs(gtOrdinate - leOrdinate);
}

int CPinListBucket::getPinPitch(CCEtoODBDoc& camCadDoc)
{
   int pinPitch = -1;

   if (m_pinPitch < INT_MAX)
   {
      double unitFactor = Units_Factor(camCadDoc.getSettings().getPageUnits(), UNIT_MILS);
      pinPitch = convertUnit(m_pinPitch, unitFactor); 
   }

   return pinPitch;
}

int CPinListBucket::getPinSeparation(CCEtoODBDoc& camCadDoc)
{
	int pinSeparation = -1;

   if (m_pinSeparation < INT_MAX)
   {
      double unitFactor = Units_Factor(camCadDoc.getSettings().getPageUnits(), UNIT_MILS);
      pinSeparation = convertUnit(m_pinSeparation, unitFactor); 
   }

   return pinSeparation;
}

//_____________________________________________________________________________
void CPinListBuckets::putPinsInBuckets(CPinStructList* pins,CCEtoODBDoc& camCadDoc)
{
   double unitFactor = Units_Factor(camCadDoc.getSettings().getPageUnits(), UNIT_MILS);
	
	for (POSITION pinPos = pins->GetHeadPosition();pinPos != NULL;)
	{
		CPinStruct* pin = pins->GetNext(pinPos);

		int x = convertUnit(pin->GetData()->getInsert()->getOriginX(), unitFactor); 
		int y = convertUnit(pin->GetData()->getInsert()->getOriginY(), unitFactor); 

      m_xBucket.add(x,pin);
      m_yBucket.add(y,pin);
	}
}

void CPinListBuckets::analyzeBuckets(BlockStruct* geom,CCEtoODBDoc& camCadDoc)
{
   DbUnit centroidX = 0, centroidY = 0;
	DataStruct* data = centroid_exist_in_block(geom);

   if (data != NULL)
   {
      centroidX = data->getInsert()->getOriginX();
      centroidY = data->getInsert()->getOriginY();
   }

   m_xBucket.analyze(centroidX,PIN_ORIENTATION_WEST ,PIN_ORIENTATION_EAST );
   m_yBucket.analyze(centroidY,PIN_ORIENTATION_SOUTH,PIN_ORIENTATION_NORTH);
}

int CPinListBuckets::getHorizontalPinPitch(CCEtoODBDoc& doc)
{
   return m_xBucket.getPinPitch(doc);
}

int CPinListBuckets::getVerticalPinPitch(CCEtoODBDoc& doc)
{
   return m_yBucket.getPinPitch(doc);
}

int CPinListBuckets::getHorizontalPinSeparation(CCEtoODBDoc& doc)
{
   return m_xBucket.getPinSeparation(doc);
}

int CPinListBuckets::getVerticalPinSeparation(CCEtoODBDoc& doc)
{
   return m_yBucket.getPinSeparation(doc);
}

int CPinListBuckets::getHorizontalPinCount()
{
   return m_xBucket.getPinCount();
}

int CPinListBuckets::getVerticalPinCount()
{
   return m_yBucket.getPinCount();
}

EPinPatternType CPinListBuckets::determinePinPattern()
{
	// Determine if pins lie on perimeter of body or form a grid.

	// If there are only two or less columns of pins with more than two pins,
	// and there are only two or less rows of pins with more than two pins,
	// then the pins likely form a rectange, and are on the perimiter.
	// If there are three (or more) rows or columns with more than two pins,
	// then there is a pin under the comp body, so it must be a grid.

	// Determine if there are more than two columns of pins with more
	// than two pins
	int foundWithMoreThanTwo = 0;
	for (POSITION bucketPos = m_xBucket.getStartPosition();bucketPos != NULL;)
	{
		int ordinate;
		CPinStructList* pinStructList = NULL;
		m_xBucket.getNextAssoc(bucketPos, ordinate, pinStructList);

		if (pinStructList != NULL)
      {
         if (pinStructList->GetCount() > 2)
         {
            foundWithMoreThanTwo++;

				// No need to process all rows/columns.
				// Can make GRID determination as soon as 3rd one is found.
				if (foundWithMoreThanTwo > 2) return pinPatternGrid;
         }
		}
	}


	// Determine if there are more than than two rows of pins with more
	// than two pins
	foundWithMoreThanTwo = 0;
	for (POSITION bucketPos = m_yBucket.getStartPosition();bucketPos != NULL;)
	{
		int ordinate;
		CPinStructList* pinStructList = NULL;
		m_yBucket.getNextAssoc(bucketPos, ordinate, pinStructList);

		if (pinStructList != NULL)
      {
         if (pinStructList->GetCount() > 2)
         {
            foundWithMoreThanTwo++;

				// No need to process all rows/columns.
				// Can made GRID determination as soon as 3rd one is found.
				if (foundWithMoreThanTwo > 2) return pinPatternGrid;
         }
		}
	}


	// Default, did not find more than two rows or columns with more than
	// two pins, presume all pins are on perimeter.
	return pinPatternPerimeter;
}

EQuadrantType CPinListBuckets::determinePinOneQuadrant(CPoint2d partCenter)
{
	for (POSITION bucketPos = m_xBucket.getStartPosition();bucketPos != NULL;)
	{
		int ordinate;
		CPinStructList* pinStructList = NULL;
		m_xBucket.getNextAssoc(bucketPos, ordinate, pinStructList);

		if (pinStructList != NULL)
		{
			for (POSITION pinPos = pinStructList->GetHeadPosition(); pinPos != NULL;)
			{
				CPinStruct *pin = pinStructList->GetNext(pinPos);

				CString pinName = pin->GetData()->getInsert()->getRefname();
				if (pinName.CompareNoCase("1") == 0)
				{
					CPoint2d pinorg = pin->getOrigin();

					// If either pin 1 X or Y is on the axis, then the quadrant is 
					// ambiguous. Select a quadrant to cause a counter-clockwise bias.
					// Eg is x = 0 (on Y axis) and y > 0, select quad 2, which is the
					// zero rotation position. If y < 0, select quad 4 (i.e. opposite
					// quadrant, 180 degrees out).
					// Special special case: if both at center give it Quad 2.

					if (pinorg.x == partCenter.x)
					{
						if (pinorg.y >= partCenter.y)
							return quadrantTwo;
						else
							return quadrantFour;
					}
					else if (pinorg.y == partCenter.y)
					{
						if (pinorg.x >= partCenter.x)
							return quadrantOne;
						else
							return quadrantThree;
					}


					// Still here? Then neither of pin 1 XY is on axis.
					// Determine quadrant.

					if (pinorg.x > partCenter.x)
					{
						if (pinorg.y > partCenter.y)
							return quadrantOne;
						else
							return quadrantFour;
					}
					else
					{
						if (pinorg.y > partCenter.y)
							return quadrantTwo;
						else
							return quadrantThree;
					}
				}
			}
		}
	}

	return quadrantTwo;
}

/******************************************************************************/
/*  CUniqueGeomStruct Class
/******************************************************************************/
CUniqueGeomStruct::CUniqueGeomStruct(CCEtoODBDoc& camCadDoc, CString uniqueName, DataStruct* compData, bool mirrorLayers, bool mirrorFlip, int insertLayerIndex) :
   m_pinList(true)
{
	m_sName = uniqueName;
	m_pGeomBlock = camCadDoc.getBlockAt(compData->getInsert()->getBlockNumber());
	m_ePackageType = packageTypeUSER;
	m_bMirrorLayers = mirrorLayers;
	m_bMirrorFlip = mirrorFlip;
	m_iInsertLayerIndex = insertLayerIndex;

   m_horizontalPinPitch = 
   m_verticalPinPitch   = 
   m_horizontalPinCount = 
   m_verticalPinCount   = 0;

	m_pinPattern         = pinPatternPerimeter;
	m_pin1quadrant			= quadrantTwo;  // aka Northwest, SOIC/QFP rotation 0

	gatherPinData(camCadDoc, compData);
}

CUniqueGeomStruct::~CUniqueGeomStruct()
{
	m_pGeomBlock = NULL;
}

void CUniqueGeomStruct::gatherPinData(CCEtoODBDoc& camCadDoc, DataStruct* compData)
{
	POSITION dataPos = m_pGeomBlock->getHeadDataInsertPosition();
	while (dataPos != NULL)
	{
		DataStruct *pinData = m_pGeomBlock->getNextDataInsert(dataPos);

		if (pinData == NULL || pinData->getInsert() == NULL)
			continue;
 
		if (pinData->getInsert()->getInsertType() == insertTypePin)
			m_pinList.AddTail(new CPinStruct(pinData));
	}

	calculateOrientations(camCadDoc);
	m_pinList.DetermineOrientation();

	m_ePackageType = packageTypeUSER;									// Package type USER

	if (compData->getAttributes() == NULL)
		return;

	WORD deviceKey = camCadDoc.RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD technologyKey = camCadDoc.RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);
	Attrib* attrib = NULL;
	
	if (compData->getAttributes()->Lookup(technologyKey, attrib) && attrib != NULL)
	{
		CString technology = "";
		technology = attrib->getStringValue();

		if (technology.CompareNoCase("THRU") == 0)
		{			
			m_mountType = thruHoleMountComponent;
		}
		else if (technology.CompareNoCase("SMD") == 0)
		{			
			m_mountType = surfaceMountComponent;
		}
   }

	if (compData->getAttributes()->Lookup(deviceKey, attrib) && attrib != NULL)
	{
		CString deviceType = "";
		deviceType = attrib->getStringValue();

		if (deviceType.CompareNoCase(deviceTypeTagToFriendlyString(deviceTypeConnector)) == 0)
		{			
			m_ePackageType = packageTypeCONN;							// Package type CONN
			return;
		}
	}
	
	if (m_mountType == thruHoleMountComponent)
	{			
		m_ePackageType = packageTypeTHT;									// Package type THT
	}
	else if (m_mountType == surfaceMountComponent)
	{
		if (m_pGeomBlock->getPinCount() == 2)
      {
			m_ePackageType = packageTypeCHIP;							// Package type CHIP
      }
		else
		{
			// Can only be SOIC or QFP if pin count is even and greater than 5, case 1501
			if ((m_pinList.GetCount() & 1) || m_pinList.GetCount() < 6 ) //|| getPinSeparation() < 0)
			{
				m_ePackageType = packageTypeUSER;
			}
         else if (getPinSeparation() < 0)
			{
				m_ePackageType = packageTypeUSER;
			}


			// Can not be a SOIC or QFP is pins are in a grid (is likely BGA/PGA), case 1501
			else if (m_pinPattern == pinPatternGrid)
			{
				m_ePackageType = packageTypeUSER;
			}

			// Is likely SOIC or QFP
			else
			{
				// OrientCount is number of sides that have pins.
				int orientCount = m_pinList.GetOrientationCount();
				if (orientCount == 2 )
				{
					m_ePackageType = packageTypeSOIC;						// Package type SOIC
				}
				else if (orientCount == 4)
				{
					m_ePackageType = packageTypeQFP;						   // Package type QFP
				}
			}
		}
	}
}

void CUniqueGeomStruct::calculateOrientations(CCEtoODBDoc& camCadDoc)
{
	double unitFactor = Units_Factor(camCadDoc.getSettings().getPageUnits(), UNIT_MILS);

	calculateSimpleOrientations(camCadDoc);
   doBucketAlgorithm(camCadDoc);

	switch (m_pinList.GetCount())
	{
	case 0:
		return;

	case 1:
		{
			CPinStruct* pin = m_pinList.GetHead();
			pin->SetOrientation(PIN_ORIENTATION_NORTH);
		}

		return;

	case 2:
		{
			CPinStruct* pin1 = m_pinList.GetHead();
			CPinStruct* pin2 = m_pinList.GetTail();

			int pin1x = convertUnit(pin1->GetData()->getInsert()->getOriginX(), unitFactor); 
			int pin1y = convertUnit(pin1->GetData()->getInsert()->getOriginY(), unitFactor); 
			int pin2x = convertUnit(pin2->GetData()->getInsert()->getOriginX(), unitFactor); 
			int pin2y = convertUnit(pin2->GetData()->getInsert()->getOriginY(), unitFactor); 

			if (pin1x == pin2x)
			{
				if (pin1y > pin2y)
				{
					pin1->SetOrientation(PIN_ORIENTATION_NORTH);
					pin2->SetOrientation(PIN_ORIENTATION_SOUTH);
				}
				else
				{
					pin1->SetOrientation(PIN_ORIENTATION_SOUTH);
					pin2->SetOrientation(PIN_ORIENTATION_NORTH);
				}
			}
			else
			{
				if (pin1x > pin2x)
				{
					pin1->SetOrientation(PIN_ORIENTATION_EAST);
					pin2->SetOrientation(PIN_ORIENTATION_WEST);
				}
				else
				{
					pin1->SetOrientation(PIN_ORIENTATION_WEST);
					pin2->SetOrientation(PIN_ORIENTATION_EAST);
				}
			}
		}

		return;

	case 3: // use simple orientations
	case 4:
	case 5:
	   calculateSimpleOrientations(camCadDoc);
		break;

	default:
		break;
	}
}

void CUniqueGeomStruct::doBucketAlgorithm(CCEtoODBDoc& camCadDoc)
{
   CPinListBuckets buckets;


	buckets.putPinsInBuckets(&m_pinList,camCadDoc);
	buckets.analyzeBuckets(m_pGeomBlock,camCadDoc);

	CPoint2d centroidPt;
	DataStruct *centroidData = this->GetGeom()->GetCentroidData();
	if (centroidData != NULL)
   {
      centroidPt.x = centroidData->getInsert()->getOriginX();
      centroidPt.y = centroidData->getInsert()->getOriginY();
	}

   m_horizontalPinPitch       = buckets.getHorizontalPinPitch(camCadDoc);
   m_verticalPinPitch         = buckets.getVerticalPinPitch(camCadDoc);
   m_horizontalPinCount       = buckets.getHorizontalPinCount();
   m_verticalPinCount         = buckets.getVerticalPinCount();
	m_horizontalPinSeparation	= buckets.getHorizontalPinSeparation(camCadDoc);
	m_verticalPinSeparation		= buckets.getVerticalPinSeparation(camCadDoc);
	m_pinPattern               = buckets.determinePinPattern();
	m_pin1quadrant					= buckets.determinePinOneQuadrant(centroidPt);
}

void CUniqueGeomStruct::calculateSimpleOrientations(CCEtoODBDoc& camCadDoc)
{
   DbUnit centroidX = 0, centroidY = 0;
	DataStruct* data = centroid_exist_in_block(m_pGeomBlock);

   if (data != NULL)
   {
      centroidX = data->getInsert()->getOriginX();
      centroidY = data->getInsert()->getOriginY();
   }

	POSITION pinPos = m_pinList.GetHeadPosition();
	while (pinPos != NULL)
	{
		CPinStruct* pin = m_pinList.GetNext(pinPos);

		DbUnit x = pin->GetData()->getInsert()->getOriginX();
		DbUnit y = pin->GetData()->getInsert()->getOriginY();

		if (fabs(x) > fabs(y))
		{
			if (x > centroidX)
			{
				pin->SetOrientation(PIN_ORIENTATION_EAST);
			}
			else
			{
				pin->SetOrientation(PIN_ORIENTATION_WEST);
			}
		}
		else
		{
			if (y > centroidY)
			{
				pin->SetOrientation(PIN_ORIENTATION_NORTH);
			}
			else
			{
				pin->SetOrientation(PIN_ORIENTATION_SOUTH);
			}
		}
	}
}

CExtent CUniqueGeomStruct::getBodyExtent()
{
   CExtent bodyExtent;

   if (m_pGeomBlock != NULL)
   {
      bodyExtent = m_pGeomBlock->getPinExtent();
   }
   else
   {
      bodyExtent.update(0.,0.);
   }

   return bodyExtent;
}

double CUniqueGeomStruct::getBodyLength()
{
   double bodyLength = max(getBodyExtent().getXsize(),getBodyExtent().getYsize());

   return bodyLength;
}

double CUniqueGeomStruct::getBodyWidth()
{
   double bodyWidth = min(getBodyExtent().getXsize(),getBodyExtent().getYsize());

   return bodyWidth;
}

double CUniqueGeomStruct::getXSize()
{
   double xSize = getBodyExtent().getXsize();

   return xSize;
}

double CUniqueGeomStruct::getYSize()
{
   double ySize = getBodyExtent().getYsize();

   return ySize;
}

int CUniqueGeomStruct::getPinPitch()
{
   int pinPitch;

   if (m_horizontalPinCount > m_verticalPinCount)
   {
      pinPitch =  m_horizontalPinPitch;
   }
   else if  (m_verticalPinCount > m_horizontalPinCount)
   {
      pinPitch =  m_verticalPinPitch;
   }
   else
   {
      pinPitch = min(m_horizontalPinPitch,m_verticalPinPitch);
   }

   return pinPitch;
}

int CUniqueGeomStruct::getPinSeparation()
{
	int pinSeparation;

	if (m_horizontalPinSeparation > m_verticalPinSeparation)
	{
		pinSeparation = m_horizontalPinSeparation;
	}
	else if (m_verticalPinSeparation > m_horizontalPinSeparation)
	{
		pinSeparation = m_verticalPinSeparation;
	}
	else
	{
		pinSeparation = max(m_horizontalPinSeparation, m_verticalPinSeparation);
	}

	return pinSeparation;
}

int CUniqueGeomStruct::getSidePinCount()
{
   int pinCount = max (m_horizontalPinCount,m_verticalPinCount);

   return pinCount;
}

double CUniqueGeomStruct::getPinOneQuadrantDegree()
{
	double angleDegrees = 0.;

	switch (m_pin1quadrant)
	{
	case quadrantOne:		angleDegrees = 270.;		break;
	case quadrantTwo:		angleDegrees = 0.;		break;
	case quadrantThree:	angleDegrees = 90.;		break;
	case quadrantFour:	angleDegrees = 180.;		break;
	}

	return angleDegrees;
}

void CUniqueGeomStruct::writeCONN(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::writeTHT(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::writeCHIP(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::writeSOIC(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::writeQFP(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::writeUSER(FILE* outFile)
{
//	m_pinList.WritePinList(outFile);
}

void CUniqueGeomStruct::WriteGeom(FILE* outFile,CCEtoODBDoc& doc)
{
   switch (m_ePackageType)
   {
   case packageTypeSOIC:
   case packageTypeQFP: 
	   m_pinList.WriteCompEntry(outFile,*this,doc);

      break;
   case packageTypeCONN:
   case packageTypeTHT: 
   case packageTypeCHIP:
   case packageTypeUSER:
   default:
	   fprintf(outFile, ".%s\n", m_sName);

	   m_pinList.WritePinList(outFile,*this,doc);
   }
}

//_____________________________________________________________________________
CUniqueFiducialStruct::CUniqueFiducialStruct(CCEtoODBDoc& camCadDoc, CString uniqueName, DataStruct* compData)
{
	m_sName = uniqueName;
	m_xSize = 0.;
	m_ySize = 0.;
	m_pGeomBlock = camCadDoc.getBlockAt(compData->getInsert()->getBlockNumber());
	calculateXYSizeOfFiducialGeom(camCadDoc);
}

CUniqueFiducialStruct::~CUniqueFiducialStruct()
{
	m_pGeomBlock = NULL;
}

void CUniqueFiducialStruct::calculateXYSizeOfFiducialGeom(CCEtoODBDoc& camCadDoc)
{
	double electricXSize = 0.;
	double electricYSize = 0.;
	double maskXSize = 0.;
	double maskYSize = 0.;
	for (CDataListIterator dataList(*m_pGeomBlock, dataTypeInsert); dataList.hasNext();)
	{
		DataStruct* data = dataList.getNext();
		InsertStruct* insert = data->getInsert();		
		BlockStruct* insertBlock = camCadDoc.getBlockAt(insert->getBlockNumber());
		if (insertBlock->isAperture() && insertBlock->isComplexAperture())
			continue;

		camCadDoc.CalcBlockExtents(insertBlock);
		CTMatrix matrix;
		CPoint2d minPoint = insertBlock->getExtent().getMin();
		CPoint2d maxPoint = insertBlock->getExtent().getMax();	
		matrix.rotateDegrees(insert->getAngleDegrees());
		matrix.transform(minPoint);
		matrix.transform(maxPoint);
		double tmpXSize = fabs(minPoint.x - maxPoint.x);
		double tmpYSize = fabs(minPoint.y - maxPoint.y);
		int layerIndex = data->getLayerIndex();   // case 1692 was causing a crash when layerIndex = -1.  MAN
		if (layerIndex > -1)
		{
			LayerStruct* layer = camCadDoc.getLayerAt(layerIndex);
		
			if (layer->isElectricalBottom() || layer->isElectricalInner() || layer->isElectricalTop())
			{
				if (tmpXSize > electricXSize && tmpYSize > electricYSize)
				{
					electricXSize = tmpXSize;
					electricYSize = tmpYSize;
				}
			}
			else
			{
				if (tmpXSize > maskXSize && tmpYSize > electricYSize)
				{
					maskXSize = tmpXSize;
					maskYSize = tmpYSize;
				}
			}		
		}
	}

	if (electricXSize > 0. && electricYSize > 0.)
	{
		m_xSize = electricXSize;
		m_ySize = electricYSize;
	}
	else if (maskXSize > 0. && maskYSize > 0.)
	{
		m_xSize = maskXSize;
		m_ySize = maskYSize;
	}
	else
	{
		camCadDoc.CalcBlockExtents(m_pGeomBlock);
		m_xSize = m_pGeomBlock->getExtent().getXsize();
		m_ySize = m_pGeomBlock->getExtent().getYsize();
	}
}

void CUniqueFiducialStruct::WriteGeom(FILE* outFile,CCEtoODBDoc& camCadDoc)
{
	double unitFactor = Units_Factor(camCadDoc.getSettings().getPageUnits(), UNIT_MILS);
	fprintf(outFile, ".%s\n", m_sName);
	fprintf(outFile,"0\t0\t%d\t%d\t1\tN\n", convertUnit(m_xSize, unitFactor), convertUnit(m_ySize, unitFactor));
}


//_____________________________________________________________________________
CUniqueGeomStructMap::CUniqueGeomStructMap(CCEtoODBDoc& Doc) :
   doc(Doc)
{
}

CUniqueGeomStructMap::~CUniqueGeomStructMap()
{
}

CUniqueGeomStruct* CUniqueGeomStructMap::AddGeomStruct(CCEtoODBDoc& camCadDoc, DataStruct* data)
{
	int insertLayerIndex = getActualLayer(&doc, data, -1, false);
	bool mirrorLayers = data->getInsert()->getLayerMirrored();
	bool mirrorFlip   = data->getInsert()->getGraphicMirrored();

	BlockStruct *geom = doc.getBlockAt(data->getInsert()->getBlockNumber());
	CString cleanGeomName = geom->getName();

	// Clean Illegal Chars
	for (int i=0; i<cleanGeomName.GetLength(); i++)
	{
		if (!__iscsym(cleanGeomName[i]))
			cleanGeomName.SetAt(i, '_');
	}

	if (cleanGeomName.GetLength() > 31)
	{
		// Generate Unique Name less than 31 Chars
		cleanGeomName.Truncate(31);
	
		int count = 0;
		CString tmpStr;
		CUniqueGeomStruct* uniqueGeom = NULL;
		CString trucatedGeomName = cleanGeomName;

		while (this->Lookup(trucatedGeomName, uniqueGeom))
		{
			// Same truncated name found and geometries are the same then break
			if (uniqueGeom->GetGeom() == geom)
				return uniqueGeom;

			// Same truncated name found but geometries are different then create unique truncated name
			tmpStr.Format("%d", ++count);
			trucatedGeomName = cleanGeomName;
			trucatedGeomName.Truncate(31 - tmpStr.GetLength());
			trucatedGeomName.AppendFormat("%d", tmpStr);
		}

		// Created uniquely truncated name
		uniqueGeom = new CUniqueGeomStruct(camCadDoc, trucatedGeomName, data, mirrorLayers, mirrorFlip, insertLayerIndex);
		this->SetAt(trucatedGeomName, uniqueGeom);
		return uniqueGeom;
	}
	else
	{
		CUniqueGeomStruct* uniqueGeom = NULL;
		if (this->Lookup(cleanGeomName, uniqueGeom))
		{
			return uniqueGeom;
		}
		else
		{
			uniqueGeom = new CUniqueGeomStruct(camCadDoc, cleanGeomName, data, mirrorLayers, mirrorFlip, insertLayerIndex);
			this->SetAt(cleanGeomName, uniqueGeom);
			return uniqueGeom;
		}
	}	
}

CUniqueFiducialStruct* CUniqueGeomStructMap::AddFiducailStruct(CCEtoODBDoc& camCadDoc, DataStruct* data)
{
	BlockStruct* geom = doc.getBlockAt(data->getInsert()->getBlockNumber());
	CString cleanGeomName = geom->getName();

	// Clean Illegal Chars
	for (int i=0; i<cleanGeomName.GetLength(); i++)
	{
		if (!__iscsym(cleanGeomName[i]))
			cleanGeomName.SetAt(i, '_');
	}

	if (cleanGeomName.GetLength() > 31)
	{
		// Generate Unique Name less than 31 Chars
		cleanGeomName.Truncate(31);

		int count = 0;
		CString tmpStr;
		CUniqueFiducialStruct* uniqueFiducialStruct = NULL;
		CString trucatedGeomName = cleanGeomName;

		while (m_uniqueFiducialStructMap.Lookup(trucatedGeomName, uniqueFiducialStruct))
		{
			// Same truncated name found and geometries are the same then break
			if (uniqueFiducialStruct->GetGeom() == geom)
				return uniqueFiducialStruct;

			// Same truncated name found but geometries are different then create unique truncated name
			tmpStr.Format("%d", ++count);
			trucatedGeomName = cleanGeomName;
			trucatedGeomName.Truncate(31 - tmpStr.GetLength());
			trucatedGeomName.AppendFormat("%d", tmpStr);
		}
	
		// Created uniquely truncated name
		uniqueFiducialStruct = new CUniqueFiducialStruct(camCadDoc, trucatedGeomName, data);
		m_uniqueFiducialStructMap.SetAt(trucatedGeomName, uniqueFiducialStruct);
		return uniqueFiducialStruct;
	}
	else
	{
		CUniqueFiducialStruct* uniqueFiducialStruct = NULL;
		if (m_uniqueFiducialStructMap.Lookup(cleanGeomName, uniqueFiducialStruct))
		{
			return uniqueFiducialStruct;
		}
		else
		{
			uniqueFiducialStruct = new CUniqueFiducialStruct(camCadDoc, cleanGeomName, data);
			m_uniqueFiducialStructMap.SetAt(cleanGeomName, uniqueFiducialStruct);
			return uniqueFiducialStruct;
		}
	}
}

void CUniqueGeomStructMap::writeDefinitionByPackageType(FILE* outFile, EPackageType packageType,const CMvpAoiSettings& mvpAoiSettings)
{
	CString packageTypeName = "";

   switch (packageType)
   {
   case packageTypeCONN:  packageTypeName = "CONN";  break;
   case packageTypeTHT:   packageTypeName = "THT";   break;
   case packageTypeCHIP:  packageTypeName = "CHIP";  break;
   case packageTypeSOIC:  packageTypeName = "SOIC";  break;
   case packageTypeQFP:   packageTypeName = "QFP";   break;
   case packageTypeUSER:  packageTypeName = "USER";  break;
   }

   if (packageType != packageTypeCONN && packageType != packageTypeTHT)
   {
	   fprintf(outFile, "\n[%s]\n", packageTypeName);
   }
	
	for (POSITION pos = GetStartPosition();pos != NULL;)
	{
		CString geomName = "";
		CUniqueGeomStruct* uniqueGeom = NULL;
		GetNextAssoc(pos, geomName, uniqueGeom);

		if (uniqueGeom == NULL || uniqueGeom->GetPackageType() != packageType)
      {
			continue;
      }

      if ((uniqueGeom->getMountType() == thruHoleMountComponent && mvpAoiSettings.getExportThComponentsFlag() ) ||
          (uniqueGeom->getMountType() == surfaceMountComponent  && mvpAoiSettings.getExportSmdComponentsFlag())    )
      {
		   uniqueGeom->WriteGeom(outFile,doc);
      }
	}

	if (packageType == packageTypeUSER)
	{
		for (POSITION pos = m_uniqueFiducialStructMap.GetStartPosition(); pos != NULL;)
		{
			CString key;
			CUniqueFiducialStruct* uniqueFiducialStruct = NULL;
			m_uniqueFiducialStructMap.GetNextAssoc(pos, key, uniqueFiducialStruct);
			if (uniqueFiducialStruct != NULL)
				uniqueFiducialStruct->WriteGeom(outFile, doc);
		}
	}
}

void CUniqueGeomStructMap::WriteDefinitions(FILE* outFile,const CMvpAoiSettings& mvpAoiSettings)
{
	writeDefinitionByPackageType(outFile, packageTypeCHIP,mvpAoiSettings);
	writeDefinitionByPackageType(outFile, packageTypeSOIC,mvpAoiSettings);
	writeDefinitionByPackageType(outFile, packageTypeQFP ,mvpAoiSettings);
	writeDefinitionByPackageType(outFile, packageTypeUSER,mvpAoiSettings);
	writeDefinitionByPackageType(outFile, packageTypeTHT ,mvpAoiSettings);
	writeDefinitionByPackageType(outFile, packageTypeCONN,mvpAoiSettings);
}
