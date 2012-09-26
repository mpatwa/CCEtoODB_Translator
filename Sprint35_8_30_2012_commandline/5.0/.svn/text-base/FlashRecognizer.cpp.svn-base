
#include "StdAfx.h"
#include "FlashRecognizer.h"
#include "ConvertDrawsToAperturesDialog.h"
#include "Edit.h"
#include "CamCadDatabase.h"
#include "Region.h"
#include "Apertur2.h"
#include "LedaPolygon.h"
#include "CcDoc.h"
#include "CCEtoODB.h"
#include "QfeTypes.h"
#include "GerberEducator.h"
#include "RwUiLib.h"

#include "PolyLib.h" // For PolyIsCircle used in Scanner interface


#define Min(a,b)            (((a) < (b)) ? (a) : (b))
#define Max(a,b)            (((a) > (b)) ? (a) : (b))

//#define EnablePerformanceReporter

//_____________________________________________________________________________
CString flashRecognitionMethodToFriendlyString(FlashRecognitionMethodTag recognitionMethod)
{
   CString retval;

   switch (recognitionMethod)
   {
   case flashRecognitionMethodSingleFlash:                 retval = "Single Flash";          break;     
   case flashRecognitionMethodSingleSegment:               retval = "Single Segment";        break;     
   case flashRecognitionMethodOrthogonalRectangle:         retval = "Orthogonal Rectangle";  break;           
   case flashRecognitionMethodOrthogonalRectangleVariant:  retval = "Rectangular Variant";   break;              
   case flashRecognitionMethodThreeEntityOblong:           retval = "Three Entity Oblong";   break;         
   case flashRecognitionMethodCircularBoundary:            retval = "Circular Boundary";     break;        
   case flashRecognitionMethodPolygonBoundary:             retval = "Polygon Boundary";      break;       
   case flashRecognitionMethodPolygonMerged:               retval = "Polygon Merged";        break;     
   case flashRecognitionMethodRegionMerged:                retval = "Region Merged";         break;    
   case flashRecognitionMethodComplexCluster:              retval = "Complex Cluster";       break;      
   case flashRecognitionMethodUndefined:                   retval = "Undefined";             break;   
   default:                                                retval = "Unknown";               break;   
   }

   return retval;
}

FlashRecognitionMethodTag stringToFlashRecognitionMethod(const CString& methodString)
{
   FlashRecognitionMethodTag retval = flashRecognitionMethodUndefined;

   if      (methodString.CompareNoCase("Single Flash")         == 0) retval = flashRecognitionMethodSingleFlash;
   else if (methodString.CompareNoCase("Single Segment")       == 0) retval = flashRecognitionMethodSingleSegment;
   else if (methodString.CompareNoCase("Orthogonal Rectangle") == 0) retval = flashRecognitionMethodOrthogonalRectangle;
   else if (methodString.CompareNoCase("Rectangular Variant")  == 0) retval = flashRecognitionMethodOrthogonalRectangleVariant;
   else if (methodString.CompareNoCase("Three Entity Oblong")  == 0) retval = flashRecognitionMethodThreeEntityOblong;
   else if (methodString.CompareNoCase("Circular Boundary")    == 0) retval = flashRecognitionMethodCircularBoundary;
   else if (methodString.CompareNoCase("Polygon Boundary")     == 0) retval = flashRecognitionMethodPolygonBoundary;
   else if (methodString.CompareNoCase("Polygon Merged")       == 0) retval = flashRecognitionMethodPolygonMerged;
   else if (methodString.CompareNoCase("Region Merged")        == 0) retval = flashRecognitionMethodRegionMerged;
   else if (methodString.CompareNoCase("Complex Cluster")      == 0) retval = flashRecognitionMethodComplexCluster;

   return retval;
}

//_____________________________________________________________________________
void CCEtoODBDoc::OnConvertDrawsToApertures()
{
//#ifdef _DEBUG
//   CMemoryState oldMemState, newMemState, diffMemState;
//   oldMemState.Checkpoint();
//#endif


   CCamCadDatabase camCadDatabase(*this);
   CFlashRecognizerParameters flashRecognizerParameters(camCadDatabase);
   flashRecognizerParameters.loadFromRegistry();

   CConvertDrawsToAperturesDialog dialog(flashRecognizerParameters);

   if (dialog.DoModal() == IDOK)
   {

#if defined(UseAlgoCOMsPolygonLibrary)
      ::CoInitialize(NULL);
#endif

      CDebugWriteFormat::setFilePath("c:\\FlashRecognizer.txt");

      flashRecognizerParameters.storeInRegistry();

      CArray<int,int> layerIndexes; 

      LayerStruct* singleVisibleLayer = camCadDatabase.getSingleVisibleLayer();
      FileStruct*  singleVisibleFile  = camCadDatabase.getSingleVisibleFile();

      if (singleVisibleLayer == NULL)
      {
         formatMessageBox("One and only one layer must be visible when converting Draws to Apertures");
      }
      else if (singleVisibleFile == NULL)
      {
         formatMessageBox("One and only one file must be visible when converting Draws to Apertures");
      }
      else
      {
         CFlashRecognizer flashRecognizer(camCadDatabase,flashRecognizerParameters,singleVisibleLayer->getLayerIndex());

         flashRecognizer.scanData();
         flashRecognizer.printTreeMetricsReport(CDebugWriteFormat::getWriteFormat());

         flashRecognizer.clusterSegments();

         if (flashRecognizerParameters.getGenerateClusterDisplayFlag())
         {
            flashRecognizer.generateClusterDisplay();
         }

         LayerStruct* destinationLayer = flashRecognizer.convertClustersToApertures();
         //verifyAndRepairData();

         // Copy attribute from source layer to destination layer
         if (singleVisibleLayer != NULL && destinationLayer != NULL)
         {
            int dataSourceKw = camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);
            int dataTypeKw   = camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);

            CString dataSource, dataType;
            camCadDatabase.getAttributeStringValue(dataType  ,singleVisibleLayer->attributes(),dataTypeKw);
            dataSource = gerberEducatorDataSourceToString(gerberEducatorDataSourceGerber);

            // Add the Data Source and Data Type to destination layer
            destinationLayer->setAttrib(camCadDatabase.getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);
            destinationLayer->setAttrib(camCadDatabase.getCamCadData(), dataTypeKw, valueTypeString, dataType.GetBuffer(0), attributeUpdateOverwrite, NULL);
            destinationLayer->setLayerType(singleVisibleLayer->getLayerType());

            // Remove the Data Source and Data Type from destination layer
            RemoveAttrib((WORD)dataSourceKw, &singleVisibleLayer->getAttributesRef());
            RemoveAttrib((WORD)dataTypeKw, &singleVisibleLayer->getAttributesRef());
         }

         OnRedraw();
      }

#if defined(UseAlgoCOMsPolygonLibrary)
      ::CoUninitialize();
#endif

   }

   CDebugWriteFormat::close();

//#ifdef _DEBUG
//   newMemState.Checkpoint();
//   if( diffMemState.Difference( oldMemState, newMemState ) )
//   {
//      TRACE( "Memory leaked!\n" );
//      diffMemState.DumpStatistics();
//      oldMemState.DumpAllObjectsSince();
//   }
//#endif
}

void CCEtoODBDoc::OnUpdateConvertDrawsToApertures(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(true);
}

//_____________________________________________________________________________
CSegmentPolygon::CSegmentPolygon(int id,PageUnitsTag pageUnits)
: CPolygon(pageUnits)
, m_id(id)
{
}

//_____________________________________________________________________________
CSegmentPoly::CSegmentPoly(int id,CPoly& poly)
: CPoly(poly)
, m_id(id)
{
}

//_____________________________________________________________________________
int CSegment::m_nextId = 0;

CSegment::CSegment(const CSegment& other)
: m_camCadDatabase(other.m_camCadDatabase)
, m_line(other.m_line)
, m_width(other.m_width)
, m_roundedFlag(other.m_roundedFlag)
{
   m_id            = m_nextId++;

   m_extent        = NULL;
   m_pointExtent   = NULL;
   m_parent        = NULL;
   m_parentPolygon = NULL;
   m_parentPoly    = NULL;
}

CSegment::CSegment(CCamCadDatabase& camCadDatabase,const CPoint2d& p0,const CPoint2d& p1,double width,bool roundedFlag)
: m_camCadDatabase(camCadDatabase)
, m_line(p0,p1)
, m_width(width)
, m_roundedFlag(roundedFlag)
{
   m_id            = m_nextId++;

   m_extent        = NULL;
   m_pointExtent   = NULL;
   m_parent        = NULL;
   m_parentPolygon = NULL;
   m_parentPoly    = NULL;
}

CSegment::CSegment(CCamCadDatabase& camCadDatabase,const CLine2d& line,double width,DataStruct* parent,bool roundedFlag,
   CSegmentPolygon* parentPolygon,CSegmentPoly* parentPoly)
: m_camCadDatabase(camCadDatabase)
, m_line(line)
, m_parent(parent)
, m_width(width)
, m_roundedFlag(roundedFlag)
, m_parentPolygon(parentPolygon)
, m_parentPoly(parentPoly)
{
   m_id            = m_nextId++;

   m_extent        = NULL;
   m_pointExtent   = NULL;
}

CSegment::~CSegment()
{
   delete m_extent;
   delete m_pointExtent;
}

PageUnitsTag CSegment::getPageUnits() const
{
   return m_camCadDatabase.getPageUnits();
}

void CSegment::clearExtent()
{
   delete m_extent;
   m_extent = NULL;
}

void CSegment::clearPointExtent()
{
   delete m_pointExtent;
   m_pointExtent = NULL;
}

void CSegment::clearState()
{
   delete m_extent;
   m_extent = NULL;

   delete m_pointExtent;
   m_pointExtent = NULL;
}

void CSegment::set0(const CPoint2d& point)
{
   clearState();

   m_line.set0(point);
}

void CSegment::set1(const CPoint2d& point)
{
   clearState();

   m_line.set1(point);
}

void CSegment::setEndPoint(int endIndex,const CPoint2d& point)
{
   clearState();

   m_line.setEndPoint(endIndex,point);
}

void CSegment::setWidth(double width)
{
   clearExtent();

   m_width = width;
}

void CSegment::setRounded(bool flag)
{
   clearExtent();

   m_roundedFlag = flag;
}

CPoint2d CSegment::getOrigin() const
{
   CPoint2d origin((get0().x + get1().x)/2.,(get0().y + get1().y)/2.);

   return origin;
}

double CSegment::getExteriorLength() const
{
   double length = m_line.getLength() + m_width;

   return length;
}

double CSegment::getSquareMajorOffset() const
{
   double dx = get1().x - get0().x;
   double dy = get1().y - get0().y;
   double cosThetaSquared = dx*dx / (dx*dx + dy*dy);
   double sinTheta = sqrt(1. - cosThetaSquared);
   double cosTheta = sqrt(cosThetaSquared);
   double majorOffset = (m_width/2.) * (cosTheta + sinTheta);

   return majorOffset;
}

const CExtent& CSegment::getExtent() const
{
   if (m_extent == NULL)
   {
      m_extent = new CExtent(get0(),get1());

      if (m_roundedFlag)
      {
         m_extent->expand(m_width);
      }
      else
      {
         double d = 2. * getSquareMajorOffset();

         m_extent->expand(d,d);
      }
   }

   return *m_extent;
}

const CExtent& CSegment::getPointExtent() const
{
   if (m_pointExtent == NULL)
   {
      m_pointExtent = new CExtent(get0(),get1());
   }

   return *m_pointExtent;
}

void CSegment::transform(const CTMatrix& matrix)
{
   clearState();

   m_line.transform(matrix);
}

void CSegment::getSquaredPolygon(CPolygon& polygon) const
{
   polygon.empty();

   double dx  = m_line.get1().x - m_line.get0().x;
   double dy  = m_line.get1().y - m_line.get0().y;
   double len = m_line.getLength();

   if (len != 0.)
   {
      double halfWidth  = m_width / 2.;
      double xComponent = halfWidth * (dx/len);
      double yComponent = halfWidth * (dy/len);

      CPoint2d point0(get0());
      point0.x -= xComponent; 
      point0.y -= yComponent; 

      CPoint2d point1(get1());
      point1.x += xComponent; 
      point1.y += yComponent; 

      CPoint2d point;

      point = point0;
      point.x -= yComponent;
      point.y += xComponent;
      polygon.addVertex(point);

      point = point0;
      point.x += yComponent;
      point.y -= xComponent;
      polygon.addVertex(point);

      point = point1;
      point.x += yComponent;
      point.y -= xComponent;
      polygon.addVertex(point);

      point = point1;
      point.x -= yComponent;
      point.y += xComponent;
      polygon.addVertex(point);
   }   
}

void CSegment::getLinePolygon(CPolygon& polygon) const
{
   polygon.empty();

   double dx  = m_line.get1().x - m_line.get0().x;
   double dy  = m_line.get1().y - m_line.get0().y;
   double len = m_line.getLength();

   if (len != 0.)
   {
      double halfWidth  = m_width / 2.;
      double xComponent = halfWidth * (dx/len);
      double yComponent = halfWidth * (dy/len);

      CPoint2d point0(get0());
      CPoint2d point1(get1());
      CPoint2d point;

      point = point0;
      point.x -= yComponent;
      point.y += xComponent;
      polygon.addVertex(point);

      point = point0;
      point.x += yComponent;
      point.y -= xComponent;
      polygon.addVertex(point);

      point = point1;
      point.x += yComponent;
      point.y -= xComponent;
      polygon.addVertex(point);

      point = point1;
      point.x -= yComponent;
      point.y += xComponent;
      polygon.addVertex(point);
   }   
}

void CSegment::getRoundedPolygon(CPolygon& polygon) const
{
   polygon.empty();

   double dx  = m_line.get1().x - m_line.get0().x;
   double dy  = m_line.get1().y - m_line.get0().y;
   double len = m_line.getLength();
   double halfWidth  = m_width / 2.;
   double xComponent;
   double yComponent;
   double lineAngleRadians = m_line.getTheta();
   double angleRadians;
   const int semicircleSegmentCount = 18;
   const double semicircleSegmentDeltaAngleRadians = Pi / semicircleSegmentCount;

   if (len != 0.)
   {
      xComponent = halfWidth * (dx/len);
      yComponent = halfWidth * (dy/len);
   }
   else
   {
      xComponent = halfWidth;
      yComponent = 0;
   }

   CPoint2d point0(get0());
   CPoint2d point1(get1());
   CPoint2d point;

   for (int semicircleSegmentIndex = 0;semicircleSegmentIndex <= semicircleSegmentCount;semicircleSegmentIndex++)
   {
      angleRadians = semicircleSegmentIndex*semicircleSegmentDeltaAngleRadians + lineAngleRadians + PiOver2;
      point.x = point0.x + halfWidth * cos(angleRadians);
      point.y = point0.y + halfWidth * sin(angleRadians);
      polygon.addVertex(point);
   }

   for (int semicircleSegmentIndex = 0;semicircleSegmentIndex <= semicircleSegmentCount;semicircleSegmentIndex++)
   {
      angleRadians = semicircleSegmentIndex*semicircleSegmentDeltaAngleRadians + lineAngleRadians - PiOver2;
      point.x = point1.x + halfWidth * cos(angleRadians);
      point.y = point1.y + halfWidth * sin(angleRadians);
      polygon.addVertex(point);
   }
}

void CSegment::getPolygon(CPolygon& polygon) const
{
   if (m_roundedFlag)
   {
      getRoundedPolygon(polygon);
   }
   else
   {
      getSquaredPolygon(polygon);
   }
}

void CSegment::getPoly(CPoly& poly) const
{
   poly.setClosed(true);
   poly.setWidthIndex(0);
   poly.empty();

   double dx  = m_line.get1().x - m_line.get0().x;
   double dy  = m_line.get1().y - m_line.get0().y;
   double len = m_line.getLength();

   if (len != 0.)
   {
      double halfWidth  = m_width / 2.;
      double xComponent = halfWidth * (dx/len);
      double yComponent = halfWidth * (dy/len);

      CPoint2d point0(get0());
      CPoint2d point1(get1());
      CPoint2d point,firstPoint;

      if (m_roundedFlag)
      {
         firstPoint = point0;
         firstPoint.x -= yComponent;
         firstPoint.y += xComponent;
         poly.addVertex(firstPoint.x,firstPoint.y,1.0);

         point = point0;
         point.x += yComponent;
         point.y -= xComponent;
         poly.addVertex(point.x,point.y);

         point = point1;
         point.x += yComponent;
         point.y -= xComponent;
         poly.addVertex(point.x,point.y,1.0);

         point = point1;
         point.x -= yComponent;
         point.y += xComponent;
         poly.addVertex(point.x,point.y);
         poly.addVertex(firstPoint.x,firstPoint.y);
      }
      else
      {
         point0.x -= xComponent; 
         point0.y -= yComponent; 

         point1.x += xComponent; 
         point1.y += yComponent; 

         firstPoint = point0;
         firstPoint.x -= yComponent;
         firstPoint.y += xComponent;
         poly.addVertex(firstPoint.x,firstPoint.y);

         point = point0;
         point.x += yComponent;
         point.y -= xComponent;
         poly.addVertex(point.x,point.y);

         point = point1;
         point.x += yComponent;
         point.y -= xComponent;
         poly.addVertex(point.x,point.y);

         point = point1;
         point.x -= yComponent;
         point.y += xComponent;
         poly.addVertex(point.x,point.y);
         poly.addVertex(firstPoint.x,firstPoint.y);
      }
   } 
}

void CSegment::getPoly(CCamCadDatabase& camCadDatabase,CPoly& poly) const
{
   getPoly(poly);

   int widthIndex = camCadDatabase.getDefinedWidthIndex(m_width);
   poly.setWidthIndex(widthIndex);
}

CPoly* CSegment::makeNewPoly(int widthIndex) const
{
   CPoly* poly = new CPoly();
   getPoly(*poly);
   poly->setWidthIndex(widthIndex);

   return poly;
}

CPoly* CSegment::addPoly(CCamCadDatabase& camCadDatabase,DataStruct& polyData,int widthIndex) const
{
   CPoly* poly = camCadDatabase.addClosedPoly(&polyData,widthIndex);

   double dx  = m_line.get1().x - m_line.get0().x;
   double dy  = m_line.get1().y - m_line.get0().y;
   double len = m_line.getLength();

   if (len != 0.)
   {
      double halfWidth  = m_width / 2.;
      double xComponent = halfWidth * (dx/len);
      double yComponent = halfWidth * (dy/len);

      CPoint2d point0(get0());
      CPoint2d point1(get1());
      CPoint2d point,firstPoint;

      if (m_roundedFlag)
      {
         CPolygon polygon(camCadDatabase.getPageUnits());
         getPolygon(polygon);

         if (polygon.isValid())
         {
            polygon.makePoly(*poly);
         }
         else
         {
            firstPoint = point0;
            firstPoint.x -= yComponent;
            firstPoint.y += xComponent;
            camCadDatabase.addVertex(poly,firstPoint.x,firstPoint.y,1.0);

            point = point0;
            point.x += yComponent;
            point.y -= xComponent;
            camCadDatabase.addVertex(poly,point.x,point.y);

            point = point1;
            point.x += yComponent;
            point.y -= xComponent;
            camCadDatabase.addVertex(poly,point.x,point.y,1.0);

            point = point1;
            point.x -= yComponent;
            point.y += xComponent;
            camCadDatabase.addVertex(poly,point.x,point.y);
            camCadDatabase.addVertex(poly,firstPoint.x,firstPoint.y);
         }
      }
      else
      {
         point0.x -= xComponent; 
         point0.y -= yComponent; 

         point1.x += xComponent; 
         point1.y += yComponent; 

         firstPoint = point0;
         firstPoint.x -= yComponent;
         firstPoint.y += xComponent;
         camCadDatabase.addVertex(poly,firstPoint.x,firstPoint.y);

         point = point0;
         point.x += yComponent;
         point.y -= xComponent;
         camCadDatabase.addVertex(poly,point.x,point.y);

         point = point1;
         point.x += yComponent;
         point.y -= xComponent;
         camCadDatabase.addVertex(poly,point.x,point.y);

         point = point1;
         point.x -= yComponent;
         point.y += xComponent;
         camCadDatabase.addVertex(poly,point.x,point.y);
         camCadDatabase.addVertex(poly,firstPoint.x,firstPoint.y);
      }
   } 

   return poly;
}

double CSegment::getEndpointDistanceSquared(const CSegment& other) const
{
   double minEndpointDistanceSquared = m_line.distanceToLineSquared(other.m_line.get0());
   double endpointDistanceSquared    = m_line.distanceToLineSquared(other.m_line.get1());

   if (endpointDistanceSquared < minEndpointDistanceSquared)
   {
      minEndpointDistanceSquared = endpointDistanceSquared;
   }

   endpointDistanceSquared    = other.m_line.distanceToLineSquared(m_line.get0());

   if (endpointDistanceSquared < minEndpointDistanceSquared)
   {
      minEndpointDistanceSquared = endpointDistanceSquared;
   }

   endpointDistanceSquared    = other.m_line.distanceToLineSquared(m_line.get1());

   if (endpointDistanceSquared < minEndpointDistanceSquared)
   {
      minEndpointDistanceSquared = endpointDistanceSquared;
   }

   return minEndpointDistanceSquared;
}

bool CSegment::isInside(const CPolygon& polygon) const
{
   bool retval = getExtent().intersects(polygon.getExtent());

   if (retval)
   {
      retval = false;

      double distance = polygon.distanceTo(m_line.get0());

      if (-distance >= m_width/2.)
      {
         distance = polygon.distanceTo(m_line.get1());

         if (-distance >= m_width/2.)
         {
            CPolygon segmentPolygon(polygon.getPageUnits());

            if (m_roundedFlag)
            {
               getLinePolygon(segmentPolygon);
            }
            else
            {
               getSquaredPolygon(segmentPolygon);
            }

            if (segmentPolygon.areAllPointsInside(polygon))
            {
               if (!segmentPolygon.intersects(polygon))
               {
                  retval = true;
               }
               else
               {
                  retval = false;
               }
            }
            else
            {
               retval = false;
            }
         }
      }
   }

   return retval;
}

bool CSegment::intersects(const CSegment& other)
{
   bool retval = getExtent().intersects(other.getExtent());

   if (retval)
   {
      double minEndPointDistance = 0.;
      retval = m_line.intersects(other.m_line);

      if (!retval)
      {
         double touchDistance = (m_width + other.m_width)/2.;
         touchDistance *= touchDistance;

         double distance = m_line.distanceToLineSquared(other.m_line.get0());
         minEndPointDistance = distance;

         retval = (distance < touchDistance);

         if (!retval)
         {
            distance = m_line.distanceToLineSquared(other.m_line.get1());

            retval = (distance < touchDistance);

            if (!retval)
            {
               if (distance < minEndPointDistance) minEndPointDistance = distance;

               distance = other.m_line.distanceToLineSquared(m_line.get0());

               retval = (distance < touchDistance);

               if (!retval)
               {
                  if (distance < minEndPointDistance) minEndPointDistance = distance;

                  distance = other.m_line.distanceToLineSquared(m_line.get1());

                  retval = (distance < touchDistance);

                  if (!retval && (distance < minEndPointDistance)) minEndPointDistance = distance;
               }
            }
         }

         if (!retval && (!m_roundedFlag || !other.m_roundedFlag))
         {
            double touchDistance = m_width*(m_roundedFlag ? SqrtOf2 : 1.) + 
                                   other.m_width*(other.m_roundedFlag ? SqrtOf2 : 1.);

            if (minEndPointDistance < touchDistance)
            {
               if (!m_roundedFlag && !other.m_roundedFlag)
               {
                  CPolygon polygon(getPageUnits()),otherPolygon(getPageUnits());

                  getSquaredPolygon(polygon);
                  other.getSquaredPolygon(otherPolygon);

                  for (POSITION pos = polygon.getHeadPointPosition();!retval && pos != NULL;)
                  {
                     CPoint2d point = polygon.getNextPoint(pos);

                     retval = otherPolygon.isInside(point);
                  }

                  if (!retval)
                  {
                     for (POSITION pos = otherPolygon.getHeadPointPosition();!retval && pos != NULL;)
                     {
                        CPoint2d point = otherPolygon.getNextPoint(pos);

                        retval = polygon.isInside(point);
                     }
                  }
               }
               else
               {
                  const CSegment& roundedSegment = (m_roundedFlag ? *this : other);
                  const CSegment& squaredSegment = (m_roundedFlag ? other : *this);

                  CPolygon squaredPolygon(getPageUnits());
                  squaredSegment.getSquaredPolygon(squaredPolygon);

                  double touchingDistance = roundedSegment.m_width;
                  touchingDistance *= touchingDistance;

                  retval = (squaredPolygon.isOutsideAndCloserThanDistanceSquared(roundedSegment.get0(),touchingDistance) || 
                            squaredPolygon.isOutsideAndCloserThanDistanceSquared(roundedSegment.get1(),touchingDistance)    );
               }
            }
         }
      }
   }

   return retval;
}

void CSegment::dump(CWriteFormat& writeFormat,int depth)
{
   writeFormat.pushHeader(".  ");

   writeFormat.writef("CSegment - %p (%f,%f),(%f,%f) width=%f\n",this,
      m_line.get0().x,m_line.get0().y,m_line.get1().x,m_line.get1().y,m_width);

   writeFormat.popHeader();
}

//_____________________________________________________________________________
//CSegmentQfe::CSegmentQfe(const CPoint2d& origin,CSegment* segment) :
//   CTypedQfe<CSegment>(origin,segment)
//{
//   m_cluster = NULL;
//}

CSegmentQfe::CSegmentQfe(const CSegmentQfe& other) :
   CTypedQfe<CSegment>(other.getSegment().getOrigin(),
                       new CSegment(other.getSegment()))
{
   m_cluster = NULL;
}

CSegmentQfe::CSegmentQfe(const CSegmentQfe& other,const CTMatrix& matrix) :
   CTypedQfe<CSegment>(matrix.transform(other.getSegment().getOrigin()),
                       new CSegment(other.getSegment()))
{
   m_cluster = NULL;

   getSegment().transform(matrix);
}

CSegmentQfe::CSegmentQfe(CSegment& segment) :
   CTypedQfe<CSegment>(segment.getOrigin(),&segment)
{
   m_cluster = NULL;
}

CSegmentQfe::~CSegmentQfe()
{
   delete getObject();
}

void CSegmentQfe::setCluster(CSegmentCluster& cluster) 
{ 
   m_cluster = &cluster; 
}

void CSegmentQfe::transform(const CTMatrix& matrix)
{
   getObject()->transform(matrix);
}

int CSegmentQfe::sortByAscendingLength(const void* elem0,const void* elem1)
{
   CSegmentQfe* segmentQfe0 = *((CSegmentQfe**)elem0);
   CSegmentQfe* segmentQfe1 = *((CSegmentQfe**)elem1);

   double difference = segmentQfe0->getSegment().getLineLengthSquared() -
                       segmentQfe1->getSegment().getLineLengthSquared();

   int retval = 0;

   if (difference < 0.)
   {
      retval = 1;
   }
   else if (difference > 0.)
   {
      retval = -1;
   }

   return retval;
}

//_____________________________________________________________________________
CSegmentQfeTree::CSegmentQfeTree() :
   CDerivedQfeTree<CSegmentQfe>()
{
}

//_____________________________________________________________________________
CSegmentGraduatedQfeTree::CSegmentGraduatedQfeTree(double granularity,double maxFeatureSize) :
   CDerivedGraduatedQfeTree<CSegmentQfe>(granularity,maxFeatureSize)
{
}

//_____________________________________________________________________________
CSegmentQfeList::CSegmentQfeList() :
   CDerivedQfeList<CSegmentQfe>(false)
{
}

CSegmentQfeList::CSegmentQfeList(const CSegmentQfeList& other,bool isContainer) :
   CDerivedQfeList<CSegmentQfe>(isContainer)
{
   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = other.GetNext(pos);

      if (isContainer)
      {
         AddTail(new CSegmentQfe(*segmentQfe));
      }
      else
      {
         AddTail(segmentQfe);
      }
   }
}

CSegmentQfeList::CSegmentQfeList(const CSegmentQfeList& other,const CTMatrix& matrix) :
   CDerivedQfeList<CSegmentQfe>(true)
{
   for (POSITION pos = other.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = other.GetNext(pos);
      AddTail(new CSegmentQfe(*segmentQfe,matrix));
   }
}

void CSegmentQfeList::transform(const CTMatrix& matrix)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = GetNext(pos);
      segmentQfe->transform(matrix);
   }
}

CPoint2d CSegmentQfeList::getCentroid() const
{
   CPoint2d centroid(0.,0.);
   int numPoints = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = GetNext(pos);
      const CLine2d& line = segmentQfe->getSegment().getLine();

      centroid.x += line.get0().x;
      centroid.y += line.get0().y;
      centroid.x += line.get1().x;
      centroid.y += line.get1().y;

      numPoints += 2;
   }

   if (numPoints > 0)
   {
      centroid.x /= numPoints;
      centroid.y /= numPoints;
   }

   return centroid;
}

void CSegmentQfeList::sortByAscendingLength()
{
   int count = GetCount();

   if (count > 1)
   {
      CTypedPtrArrayContainer<CSegmentQfe*> sortArray(1,false);
      sortArray.SetSize(count);
      int index = 0;

      for (POSITION pos = GetHeadPosition();pos != NULL;index++)
      {
         CSegmentQfe* segmentQfe = GetNext(pos);
         sortArray.setAt(index,segmentQfe);
      }

      sortArray.setSortFunction(CSegmentQfe::sortByAscendingLength);
      sortArray.sort();

      index = 0;
      RemoveAll();

      for (index = 0;index < count;index++)
      {
         CSegmentQfe* segmentQfe = sortArray.GetAt(index);
         AddTail(segmentQfe);
      }
   }
}

//_____________________________________________________________________________
CGraphSegment::CGraphSegment(CSegment& segment)
: m_segment(segment)
{
   m_visited   = false;
   m_evaluated = false;
   m_headEndIndex = 0;
}

//_____________________________________________________________________________
CPolygon* CGraphSegmentList::makePolygon(POSITION startPos,double& width,CGraphSegmentMap& segmentMap) 
{ 
   CPolygon* polygon = NULL;
   segmentMap.RemoveAll();

   if (GetCount() > 0)
   {
      segmentMap.InitHashTable(nextPrime2n(Min(100,2*GetCount())));

      for (POSITION pos = startPos;pos != NULL;)
      {
         CGraphSegment* graphSegment = GetNext(pos);
         width = graphSegment->getSegment().getWidth();

         if (polygon == NULL)
         {
            polygon = new CPolygon(graphSegment->getSegment().getPageUnits());
         }

         polygon->addVertex(graphSegment->getTailEndPoint());
         segmentMap.SetAt(graphSegment,graphSegment);
      }
   }

   return polygon; 
}

CString CGraphSegmentList::getDiagram(POSITION pos)
{
   CString diagramString,idString;

   if (pos == NULL)
   {
      pos = GetHeadPosition();
   }

   while (pos != NULL)
   {
      CGraphSegment* graphSegment = GetNext(pos);

      idString.Format("%d ",graphSegment->getSegment().getId());
      diagramString += idString;
   }

   return diagramString;
}

//_____________________________________________________________________________
CString CGraphSegmentQfeList::getDiagram(POSITION pos)
{
   CString diagramString,idString;

   if (pos == NULL)
   {
      pos = GetHeadPosition();
   }

   while (pos != NULL)
   {
      CGraphSegmentQfe* graphSegmentQfe = GetNext(pos);
      CGraphSegment* graphSegment = graphSegmentQfe->getObject();

      idString.Format("%d ",graphSegment->getSegment().getId());
      diagramString += idString;
   }

   return diagramString;
}

//_____________________________________________________________________________
CGraphSegmentMap& CGraphSegmentMap::operator=(const CGraphSegmentMap& other)
{
   if (&other != this)
   {
      RemoveAll();
      InitHashTable(other.GetHashTableSize());
      CGraphSegment *key,*value;

      for (POSITION pos = other.GetStartPosition();pos != NULL;)
      {
         other.GetNextAssoc(pos,key,value);
         SetAt(key,value);
      }
   }

   return *this;
}

//_____________________________________________________________________________
CGraphSegmentQfe::CGraphSegmentQfe(CGraphSegment& graphSegment,int endIndex) :
   m_endIndex(endIndex),
   m_origin(graphSegment.getSegment().getEndPoint(endIndex)),
   CTypedQfe<CGraphSegment>(m_origin,&graphSegment)
{
}

//_____________________________________________________________________________
int CSegmentCluster::m_nextClusterId = 0;
int CSegmentCluster::m_flashRecognitionMethodKeywordIndex = -1;
int CSegmentCluster::m_flashRecognitionTimeKeywordIndex   = -1;

int CSegmentCluster::getFlashRecognitionMethodKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_flashRecognitionMethodKeywordIndex < 0)
   {
      m_flashRecognitionMethodKeywordIndex = camCadDatabase.registerKeyword("FlashRecognitionMethod",valueTypeString);
   }

   return m_flashRecognitionMethodKeywordIndex;
}

int CSegmentCluster::getFlashRecognitionTimeKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_flashRecognitionTimeKeywordIndex < 0)
   {
      m_flashRecognitionTimeKeywordIndex = camCadDatabase.registerKeyword("FlashRecognitionTimeMilliseconds",valueTypeDouble);
   }

   return m_flashRecognitionTimeKeywordIndex;
}

void CSegmentCluster::resetKeywordIndices()
{
   m_flashRecognitionMethodKeywordIndex = -1;
   m_flashRecognitionTimeKeywordIndex   = -1;
}

CSegmentCluster::CSegmentCluster()
{
   m_clusterId = m_nextClusterId++;

   m_isCleanAperture = false;
   m_apertureInsert  = NULL;
}

CSegmentCluster::CSegmentCluster(const CSegmentCluster& other) :
   m_list(other.m_list)
{
   m_clusterId = m_nextClusterId++;

   m_isCleanAperture = other.m_isCleanAperture;
   m_apertureInsert  = other.m_apertureInsert;
}

CSegmentCluster::CSegmentCluster(const CSegmentCluster& other,const CTMatrix& matrix) :
   m_list(other.m_list,matrix)
{
   m_clusterId = m_nextClusterId++;

   m_isCleanAperture = other.m_isCleanAperture;
   m_apertureInsert  = other.m_apertureInsert;
}

CSegmentCluster::CSegmentCluster(CFlashRecognizer& flashRecognizer,DataStruct& apertureInsert,int layerIndex)
{
   CCamCadDatabase& camCadDatabase = flashRecognizer.getCamCadDatabase();
   m_clusterId = m_nextClusterId++;

   m_isCleanAperture = false;
   m_apertureInsert  = NULL;

   if (apertureInsert.getDataType() == dataTypeInsert)
   {
      BlockStruct* apertureBlock = camCadDatabase.getBlock(apertureInsert.getInsert()->getBlockNumber());
      InsertStruct* insert = apertureInsert.getInsert();
      DataStruct* polyStruct = NULL;
      int zeroWidthIndex = camCadDatabase.getDefinedWidthIndex(0.);

      if (apertureBlock->isAperture())
      {
         double halfSizeA = apertureBlock->getSizeA()/2.;
         double halfSizeB = apertureBlock->getSizeB()/2.;
         ApertureShapeTag shape = apertureBlock->getShape();
         double angleRadians    = apertureBlock->getRotationRadians();

         // determine if a single segment can represent the aperture
         if (shape == apertureOblong    || shape == apertureRound || 
             shape == apertureRectangle || shape == apertureSquare   )
         {
            if (halfSizeA > 0.)
            {
               const double minRectAspectRatio = 1.001;

               if (shape == apertureSquare || shape == apertureRound)
               {
                  halfSizeB = halfSizeA / minRectAspectRatio;
               }
               else
               {
                  if (halfSizeB > halfSizeA) 
                  {
                     double temp = halfSizeB;
                     halfSizeB = halfSizeA;
                     halfSizeA = temp;
                     angleRadians += Pi/2.;
                  }

                  if ((halfSizeA / halfSizeB) < minRectAspectRatio)
                  {
                     halfSizeB = halfSizeA / minRectAspectRatio;
                  }
               }

               double halfHeight = halfSizeA - halfSizeB;
               int widthIndex = (((shape == apertureOblong) || (shape == apertureRound)) ? 
                                      camCadDatabase.getDefinedWidthIndex(halfSizeB*2.) :
                                      camCadDatabase.getDefinedSquareWidthIndex(halfSizeB*2.));

               polyStruct = camCadDatabase.newPolyStruct(layerIndex,0,false,graphicClassNormal);
               CPoly* poly = camCadDatabase.addOpenPoly(polyStruct,widthIndex);
               camCadDatabase.addVertex(poly,-halfHeight,0.);
               camCadDatabase.addVertex(poly, halfHeight,0.);
            }
         }

         if (polyStruct == NULL)
         {
            switch (apertureBlock->getShape())
            {
            case apertureRound:
            case apertureSquare:
            case apertureRectangle:
            case apertureOctagon:
               {
                  CDataList dataList(true);
                  polyStruct = camCadDatabase.newPolyStruct(layerIndex,0,false,graphicClassNormal);

                  CStandardAperture standardAperture(apertureBlock,camCadDatabase.getPageUnits());
                  standardAperture.getAperturePoly(camCadDatabase.getCamCadData(),dataList,polyStruct);
               }

               break;
            }
         }

         if (polyStruct != NULL)
         {
            CTMatrix matrix = insert->getTMatrix();
            matrix.translateCtm(apertureBlock->getXoffset(),apertureBlock->getXoffset());
            matrix.rotateRadiansCtm(angleRadians);

            polyStruct->transform(matrix);

            CPolyList& polyList = *(polyStruct->getPolyList());

            for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
            {
               CPoly* poly = polyList.GetNext(polyPos);
               BlockStruct* widthBlock = camCadDatabase.getCamCadDoc().getWidthBlock(poly->getWidthIndex());
               bool roundedFlag = (widthBlock->getShape() == apertureRound);
               double width = widthBlock->getSizeA();

               //if (width < minimumWidth)
               //{
               //   width = minimumWidth;
               //}

               if (poly != NULL)
               {
                  CPoly vectorPoly(*poly);
                  vectorPoly.vectorize(camCadDatabase.getPageUnits());
                  CSegmentPolygon* parentPolygon = NULL;
                  CSegmentPoly* parentPoly = NULL;

                  if (vectorPoly.isClosed())
                  {
                     parentPolygon = flashRecognizer.addSegmentPolygon();
                     parentPoly    = flashRecognizer.addSegmentPoly(*poly);
                  }

                  const CPntList& pointList = vectorPoly.getPntList();
                  CPnt* previousPnt = NULL;

                  for (POSITION pointPos = pointList.GetHeadPosition();pointPos != NULL;)
                  {
                     CPnt* pnt = pointList.GetNext(pointPos);
                     CPoint2d point(pnt->x,pnt->y);

                     if (previousPnt != NULL)
                     {
                        if (parentPolygon != NULL)
                        {
                           parentPolygon->addVertex(point);
                        }

                        CLine2d line(CPoint2d(previousPnt->x,previousPnt->y),point);

                        double length = line.getLength();

                        CSegment* segment = new CSegment(camCadDatabase,line,width,&apertureInsert,roundedFlag,parentPolygon,parentPoly);
                        
                        //CPoint2d origin = segment->getOrigin();
                        CSegmentQfe* segmentQfe = new CSegmentQfe(*segment);
                        merge(*segmentQfe);
                     }

                     previousPnt = pnt;
                  }

                  //CPolygon* newPolygon = new CPolygon(*poly,width);
                  //CExtent polygonxtent = newPolygon->getExtent();
                  //CPoint2d origin = polygonxtent.getCenter();
                  //CPolygonQfe* polygonQfe = new CPolygonQfe(origin,newPolygon);
                  //m_layerTree.setAt(polygonQfe);
               }
            }

            m_isCleanAperture = true;
            m_apertureInsert  = &apertureInsert;
            delete polyStruct;
         }
      }
   }
}

CSegmentCluster::~CSegmentCluster()
{
}

bool CSegmentCluster::merge(CSegmentQfe& segmentQfe)
{
   bool retval = (segmentQfe.getCluster() != this);

   //CDebugWriteFormat::getWriteFormat().pushHeader(".  ");
   //CDebugWriteFormat::getWriteFormat().writef("CSegmentCluster::merge() - retval = %d\n",retval);
   //CDebugWriteFormat::getWriteFormat().writef("this cluster %p\n",this);
   //dump(CDebugWriteFormat::getWriteFormat());
   m_isCleanAperture = false;

   if (retval)
   {
      CSegmentCluster* otherCluster = segmentQfe.getCluster();

      if (otherCluster != NULL)
      {
         //CDebugWriteFormat::getWriteFormat().writef("other cluster %p\n",otherCluster);
         //otherCluster->dump(CDebugWriteFormat::getWriteFormat());

         for (POSITION pos = otherCluster->getList().GetHeadPosition();pos != NULL;)
         {
            CSegmentQfe* segmentQfe = otherCluster->getList().GetNext(pos);
            segmentQfe->setCluster(*this);
            getList().AddTail(segmentQfe);
         }

         otherCluster->getList().RemoveAll();
      }
      else
      {
         segmentQfe.setCluster(*this);
         getList().AddTail(&segmentQfe);
      }
   }

   //CDebugWriteFormat::getWriteFormat().popHeader();

   return retval;
}

CExtent CSegmentCluster::getExtent() const
{
   CExtent extent;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      extent.update(segmentQfe->getSegment().getExtent());
   }

   return extent;
}

CExtent CSegmentCluster::getPointExtent() const
{
   CExtent extent;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      extent.update(segmentQfe->getSegment().getPointExtent());
   }

   return extent;
}

bool CSegmentCluster::allSegmentsInsidePolygon(const CPolygon& polygon) const
{
   bool retval = true;

   for (POSITION pos = m_list.GetHeadPosition();retval && pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      CSegment& segment = segmentQfe->getSegment();

      retval = segment.isInside(polygon);
   }

   return retval;
}

CPolygon* CSegmentCluster::getBoundaryPolygon(CGraphSegmentQfeTree& tree,CGraphSegmentList& stack,
      CGraphSegmentMap& segmentMap,
      CGraphSegment& tailGraphSegment,int endIndex,double searchTolerance,double& width,
      int& iterationCount,const int maxIterationCount) const
{
   static int depth = 0;
   //CString debugStackDiagram = stack.getDiagram();

   depth++;
   iterationCount++;

   CPolygon* polygon = NULL;
   double maxArea = 0.;
   CPolygon* newPolygon = NULL;
   CGraphSegmentMap* newSegmentMap = NULL;

   CSegment& tailSegment = tailGraphSegment.getSegment();
   int tailSegmentId = tailSegment.getId();
   CPoint2d endPoint = tailSegment.getEndPoint(endIndex);
   CExtent searchExtent(endPoint.x - searchTolerance,endPoint.y - searchTolerance,
                        endPoint.x + searchTolerance,endPoint.y + searchTolerance);

   // find segments coincident with endPoint of tailSegment
   CGraphSegmentQfeList foundList;
   tree.search(searchExtent,foundList);
   //CString foundListDiagram = foundList.getDiagram();

   if (foundList.GetCount() > 0)
   {
      POSITION stackTailPos = stack.AddTail(&tailGraphSegment);
      tailGraphSegment.setVisited(true);
      tailGraphSegment.setHeadEndIndex(endIndex);

      for (POSITION pos = foundList.GetHeadPosition();pos != NULL;)
      {
         CGraphSegmentQfe* graphSegmentQfe = foundList.GetNext(pos);
         CGraphSegment* foundGraphSegment = graphSegmentQfe->getObject();
         CSegment& foundSegment = foundGraphSegment->getSegment();
         int foundSegmentId = foundSegment.getId();

         // tailSegment found self
         if (&tailGraphSegment == foundGraphSegment)
         {
            continue;
         }

         double twoSearchToleranceSquared = 2.*searchTolerance*searchTolerance;
         bool matchedEnd1 = (foundGraphSegment->getSegment().get1().distanceSqr(endPoint) <= twoSearchToleranceSquared);
         CPoint2d headPoint = foundGraphSegment->getSegment().getEndPoint(matchedEnd1);
         delete newPolygon;
         newPolygon = NULL;

         if (foundGraphSegment->getVisited())
         {  // hit a previous segment
            POSITION stackPos = stackTailPos;
            CGraphSegment* stackGraphSegment = NULL;

            // work backward from tail segment until the head coordinate is encountered
            for (int index = 0;stackPos != NULL;index++)
            {
               POSITION polygonHeadSegmentPos = stackPos;
               stackGraphSegment = stack.GetPrev(stackPos);

               if (index > 1 && 
                  (stackGraphSegment->getTailEndPoint().distanceSqr(headPoint) <= twoSearchToleranceSquared))
               {  // found a closed polygon
                  delete newSegmentMap;
                  newSegmentMap = new CGraphSegmentMap();

                  //CString debugPolygonDiagram = stack.getDiagram(polygonHeadSegmentPos);
                  newPolygon = stack.makePolygon(polygonHeadSegmentPos,width,*newSegmentMap);
                  break;
               }
            }
         }
         else
         {
            delete newSegmentMap;
            newSegmentMap = new CGraphSegmentMap();

            newPolygon = getBoundaryPolygon(tree,stack,*newSegmentMap,*foundGraphSegment,
                            !matchedEnd1,searchTolerance,width,iterationCount,maxIterationCount);

            if (iterationCount >= maxIterationCount)
            {
               break;
            }
         }

         if (newPolygon != NULL)
         {
            double newPolygonArea = newPolygon->getArea();

            if (polygon != NULL)
            {
               if (newPolygonArea > maxArea)
               {
                  delete polygon;
                  polygon = newPolygon;
                  newPolygon = NULL;

                  maxArea = newPolygonArea;

                  if (newSegmentMap != NULL)
                  {
                     segmentMap = *newSegmentMap;
                     delete newSegmentMap;
                     newSegmentMap = NULL;
                  }
               }
            }
            else
            {
               polygon = newPolygon;
               newPolygon = NULL;

               maxArea = newPolygonArea;

               if (newSegmentMap != NULL)
               {
                  segmentMap = *newSegmentMap;
                  delete newSegmentMap;
                  newSegmentMap = NULL;
               }
            }
         }
      }

      stack.RemoveTail();
      tailGraphSegment.setVisited(false);
   }

   delete newPolygon;
   delete newSegmentMap;

   --depth;

   return polygon;
}

int removeDuplicateSegments(CGraphSegmentQfeList& list,const CSegment& segment,double tolerance)
{
   int removedCount = 0;
   int segmentId = segment.getId();

   for (POSITION pos = list.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CGraphSegmentQfe* graphSegmentQfe = list.GetNext(pos);
      CGraphSegment* graphSegment = graphSegmentQfe->getObject();
      CSegment& listSegment = graphSegment->getSegment();
      int listSegmentId = listSegment.getId();

      if (&listSegment == &segment)
      {
         list.RemoveAt(oldPos);
         removedCount++;
      }
      else if (listSegment.get0().fpeq(segment.get0(),tolerance) && 
               listSegment.get1().fpeq(segment.get1(),tolerance)    )
      {
         list.RemoveAt(oldPos);
         removedCount++;
      }
      else if (listSegment.get1().fpeq(segment.get0(),tolerance) && 
               listSegment.get0().fpeq(segment.get1(),tolerance)    )
      {
         list.RemoveAt(oldPos);
         removedCount++;
      }
   }

   return removedCount;
}

bool CGraphSegmentQfeTree::containsDuplicateSegment(const CSegment& segment,double searchTolerance)
{
   bool retval = false;

   int segmentId = segment.getId();

   CGraphSegmentQfeList foundList;
   CExtent searchExtent(segment.get0().x - searchTolerance,segment.get0().y - searchTolerance,
                        segment.get0().x + searchTolerance,segment.get0().y + searchTolerance);
   search(searchExtent,foundList);

   for (POSITION pos = foundList.GetHeadPosition();pos != NULL;)
   {
      CGraphSegmentQfe* graphSegmentQfe = foundList.GetNext(pos);
      CGraphSegment* graphSegment = graphSegmentQfe->getObject();
      CSegment& foundSegment = graphSegment->getSegment();
      int foundSegmentId = foundSegment.getId();

      if (foundSegment.get0().fpeq(segment.get0(),searchTolerance) && 
          foundSegment.get1().fpeq(segment.get1(),searchTolerance)    )
      {
         retval = true;
         break;
      }
      else if (foundSegment.get1().fpeq(segment.get0(),searchTolerance) && 
               foundSegment.get0().fpeq(segment.get1(),searchTolerance)    )
      {
         retval = true;
         break;
      }
   }

   return retval;
}

bool CSegmentCluster::getBoundaryPolygon(CCamCadDatabase& camCadDatabase,CPolygon& polygon,
   double searchTolerance,double& width,int maxIterationCount) const
{
   bool retval = false;
   CGraphSegmentQfeTree graphSegmentQfeTree;
   CGraphSegmentList graphSegmentList;
   CGraphSegmentMap polygonSegmentMap;

   // contruct tree for non duplicate segments
   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      CSegment& segment = segmentQfe->getSegment();

      if (! graphSegmentQfeTree.containsDuplicateSegment(segment,searchTolerance))
      {
         CGraphSegment* graphSegment = new CGraphSegment(segment);
         graphSegmentList.AddTail(graphSegment);

         CGraphSegmentQfe* graphSegmentQfe0 = new CGraphSegmentQfe(*graphSegment,0);
         CGraphSegmentQfe* graphSegmentQfe1 = new CGraphSegmentQfe(*graphSegment,1);
         graphSegmentQfeTree.setAt(graphSegmentQfe0);
         graphSegmentQfeTree.setAt(graphSegmentQfe1);
      }
   }

   // filter out segments that don't share both endpoints
   CGraphSegmentList connectedList(false);
   CGraphSegmentQfeList foundList;
   CExtent searchExtent;
   //CClusterIndicator clusterIndicator(*this);  //debug

   for (POSITION pos = graphSegmentList.GetHeadPosition();pos != NULL;)
   {
      CGraphSegment* graphSegment = graphSegmentList.GetNext(pos);
      CSegment& segment = graphSegment->getSegment();

      searchExtent.set(segment.get0().x - searchTolerance,segment.get0().y - searchTolerance,
                       segment.get0().x + searchTolerance,segment.get0().y + searchTolerance);

      foundList.empty();
      graphSegmentQfeTree.search(searchExtent,foundList);
      //removeDuplicateSegments(foundList,segment,searchTolerance);

      if (foundList.GetCount() > 1)
      {
         searchExtent.set(segment.get1().x - searchTolerance,segment.get1().y - searchTolerance,
                          segment.get1().x + searchTolerance,segment.get1().y + searchTolerance);

         foundList.empty();
         graphSegmentQfeTree.search(searchExtent,foundList);
         //removeDuplicateSegments(foundList,segment,searchTolerance);

         if (foundList.GetCount() > 1)
         {
            connectedList.AddTail(graphSegment);
            //clusterIndicator.addSegment(segment); //debug
         }
      }
   }

   //if (true)
   //{
   //   FileStruct* file = camCadDatabase.getFirstVisibleFile();

   //   if (file != NULL)
   //   {
   //      BlockStruct* pcbBlock = file->getBlock();
   //      LayerStruct* clusterDisplayLayer   = camCadDatabase.getDefinedLayer("dbDisplay");
   //      LayerStruct* clusterIndicatorLayer = camCadDatabase.getDefinedLayer("dbIndicator");
   //      LayerStruct* clusterExtentLayer    = camCadDatabase.getDefinedLayer("dbExtent");

   //      clusterIndicator.instantiate(camCadDatabase,pcbBlock,*clusterDisplayLayer,*clusterIndicatorLayer,*clusterExtentLayer); //debug
   //   }
   //}

   if (connectedList.GetCount() > 2)
   {
      // contruct tree for connected segments
      graphSegmentQfeTree.deleteAll();
      //graphSegmentList.empty();

      for (POSITION pos = connectedList.GetHeadPosition();pos != NULL;)
      {
         CGraphSegment* graphSegment = connectedList.GetNext(pos);

         CGraphSegmentQfe* graphSegmentQfe0 = new CGraphSegmentQfe(*graphSegment,0);
         CGraphSegmentQfe* graphSegmentQfe1 = new CGraphSegmentQfe(*graphSegment,1);
         graphSegmentQfeTree.setAt(graphSegmentQfe0);
         graphSegmentQfeTree.setAt(graphSegmentQfe1);
      }

      int iterationCount = 0;
      //CString debugConnectedDiagram = graphSegmentList.getDiagram();

      // try to determine a boundary polygon starting with each segment in the connected list
      for (POSITION graphSegmentPos = graphSegmentList.GetHeadPosition();graphSegmentPos != NULL;)
      {
         CGraphSegment* graphSegment = graphSegmentList.GetNext(graphSegmentPos);
         //graphSegment->setVisited(true);

         CGraphSegmentList graphSegmentStack(false);
         CGraphSegmentMap segmentMap;
         CPolygon* foundPolygon = getBoundaryPolygon(graphSegmentQfeTree,graphSegmentStack,segmentMap,
                                     *graphSegment,1,searchTolerance,width,iterationCount,maxIterationCount);

         if (iterationCount >= maxIterationCount)
         {
            delete foundPolygon;
            foundPolygon = NULL;
            retval = false;

            break;
         }

         graphSegment->setEvaluated(true);

         if (foundPolygon != NULL)
         {
            if (foundPolygon->getArea() > polygon.getArea())
            {
               polygon           = *foundPolygon;
               polygonSegmentMap = segmentMap;
               retval            = true;
            }

            delete foundPolygon;
            foundPolygon = NULL;

            if (polygon.getVertexCount() == graphSegmentList.GetCount())
            {
               // all segments used in boundary
               break;
            }
         }
      }
   }

   if (retval && false)
   {
      // check if all segments are in the polygon
      CGraphSegment* graphSegment;
      //CGraphSegment* graphSegmentValue;
      const double lineFillTolerance = .0001;

      for (POSITION graphSegmentPos = graphSegmentList.GetHeadPosition();graphSegmentPos != NULL;)
      {
         graphSegment = graphSegmentList.GetNext(graphSegmentPos);

         //if (! polygonSegmentMap.Lookup(graphSegment,graphSegmentValue))
         //{
            CLine2d line = graphSegment->getSegment().getLine();

            double d0 = polygon.distanceTo(line.get0());
            double d1 = polygon.distanceTo(line.get1());

            retval = ((d0 < lineFillTolerance) && (d1 < lineFillTolerance));
            //line.expand(-.0001);

            //retval = polygon.isInsideAndCloserThanDistanceSquared(line,0.);
         //}
      }
   }

   return retval;
}

DataStruct* CSegmentCluster::getComplexApertureInsert(CCamCadDatabase& camCadDatabase,
   const CFlashRecognizerParameters& parameters,
   int layerIndex,double width,double tolerance,const CPolygon& inputPolygon)
{
   DataStruct* apertureInsert = NULL;

   CPolygon polygon(inputPolygon);

   CTMatrix matrix;
   BlockStruct* apertureBlock = camCadDatabase.getDefinedPolygonAperture(polygon,width,tolerance,matrix);

   if (apertureBlock != NULL)
   {
      CBasesVector basesVector;
      basesVector.transform(matrix);

      apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
         basesVector.getX(),basesVector.getY(),basesVector.getRotationRadians(),basesVector.getMirror());
   }

   return apertureInsert;
}

DataStruct* CSegmentCluster::getComplexApertureInsert(CCamCadDatabase& camCadDatabase,
   const CFlashRecognizerParameters& parameters,
   int layerIndex,double width,double tolerance,const CPoly& inputPoly)
{
   DataStruct* apertureInsert = NULL;

   CTMatrix matrix;
   BlockStruct* apertureBlock = camCadDatabase.getDefinedPolyAperture(inputPoly,width,tolerance,matrix);

   if (apertureBlock != NULL)
   {
      CBasesVector basesVector;
      basesVector.transform(matrix);

      apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
         basesVector.getX(),basesVector.getY(),basesVector.getRotationRadians(),basesVector.getMirror());
   }

   return apertureInsert;
}

DataStruct* CSegmentCluster::getComplexClusterApertureInsert(CCamCadDatabase& camCadDatabase,
   const CFlashRecognizerParameters& parameters,
   int layerIndex,double width,double tolerance)
{
   DataStruct* apertureInsert = NULL;
   CPolyList polyList;
   const float scaleFactor = 2000.;

   CTypedPtrMap<CMapPtrToPtr,DataStruct*,DataStruct*> dataMap;
   dataMap.InitHashTable(nextPrime2n(m_list.GetCount()));

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      DataStruct* segmentParent = segmentQfe->getSegment().getParent();

      if (segmentParent != NULL)
      {
         dataMap.SetAt(segmentParent,segmentParent);
      }
   }

   for (POSITION pos = dataMap.GetStartPosition();pos != NULL;)
   {
      DataStruct *data,*dataKey;
      dataMap.GetNextAssoc(pos,dataKey,data);

      if (data->getDataType() == dataTypePoly)
      {
         CPolyList* dataPolyList = new CPolyList(*(data->getPolyList()));
         polyList.takeData(*dataPolyList);
         delete dataPolyList;
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();
         BlockStruct* block = camCadDatabase.getBlock(insert->getBlockNumber());

         if (block->isAperture())
         {
            CPolyList* aperturePoly = ApertureToPoly(block,insert,false);
            polyList.takeData(*aperturePoly);

            delete aperturePoly;
         }         
      }
   }

   //----------------
   CSegmentCluster inputSegmentCluster(*this);

   CTMatrix matrix;
   BlockStruct* apertureBlock = camCadDatabase.getDefinedClusterAperture(inputSegmentCluster,
                                                  polyList,width,tolerance,matrix);

   if (apertureBlock != NULL)
   {
      CBasesVector basesVector;
      basesVector.transform(matrix);

      apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
         basesVector.getX(),basesVector.getY(),basesVector.getRotationRadians(),basesVector.getMirror());
   }

   return apertureInsert;
}

//#define WritePolygonsForDebugging

#ifdef UseLedaLibrary  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

//DataStruct* CSegmentCluster::getMergedClusterPolygonApertureInsert(CCamCadDatabase& camCadDatabase,
   //const CFlashRecognizerParameters& parameters,
   //int layerIndex,double width,double tolerance)
{
   TimeIndex startTime = getTimeIndex();
   DataStruct* apertureInsert = NULL;
   int zeroWidthIndex = camCadDatabase.getZeroWidthIndex();

   CTypedPtrListContainer<CPoly*> allocatedSegmentPolys;
   CTypedPtrListContainer<CPoly*> segmentPolys(false);

   CLedaSegmentCluster ledaSegmentCluster(*this);

   //CTypedPtrMap<CMapPtrToPtr,CSegmentPoly*,CSegmentPoly*> parentPolyMap;
   //parentPolyMap.InitHashTable(nextPrime2n(m_list.GetCount()));
   //CSegmentPoly* junk;

   //for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   //{
   //   CSegmentQfe* segmentQfe        = m_list.GetNext(pos);
   //   CSegment& segment              = segmentQfe->getSegment();

   //   CSegmentPoly* parentPoly       = segment.getParentPoly();

   //   if (parentPoly != NULL && parentPoly->getWidthIndex() == zeroWidthIndex)
   //   {
   //      if (!parentPolyMap.Lookup(parentPoly,junk))
   //      {
   //         parentPolyMap.SetAt(parentPoly,parentPoly);
   //         segmentPolys.AddTail(parentPoly);
   //         ledaSegmentCluster.addPoly(parentPoly);
   //      }
   //   }
   //   else
   //   {
   //      ledaSegmentCluster.addSegment(segment);
   //   }
      //else
      //{
      //   CPoly* segmentPoly = new CPoly();
      //   //bool oldRoundedFlag = segment.getRounded();
      //   //segment.setRounded(false);
      //   segment.getPoly(*segmentPoly);
      //   //segment.setRounded(oldRoundedFlag);
      //   segmentPolys.AddTail(segmentPoly);
      //   allocatedSegmentPolys.AddTail(segmentPoly);
      //}
   //}

   //CLedaPolygon2 mergedLedaPolygon;

   //for (POSITION pos = segmentPolys.GetHeadPosition();pos != NULL;)
   //{
   //   CPoly* segmentPoly = segmentPolys.GetNext(pos);

   //   mergedLedaPolygon.addPoly(*segmentPoly);
   //}

   CPolyList polyList;
   CPoly* poly = NULL;

   //mergedLedaPolygon.getPolys(polyList);
   ledaSegmentCluster.getPolys(polyList);

   if (polyList.GetCount() == 1)
   {
      poly = polyList.GetHead();
   }
   else
   {
      double maxArea = 0.;

      for (POSITION pos = polyList.GetHeadPosition();pos != NULL;)
      {
         CPoly* polyItem = polyList.GetNext(pos);
         polyItem->setWidthIndex(zeroWidthIndex);
         CPolygon polygon(*polyItem);
         double area = polygon.getArea();

         if (area > maxArea)
         {
            maxArea = area;
            poly    = polyItem;
         }
      }
   }

   if (poly != NULL)
   {
      apertureInsert = getComplexApertureInsert(camCadDatabase,parameters,layerIndex,width,tolerance,*poly);
   }

   leda::std_memory_mgr.clear();

   TimeIndex stopTime = getTimeIndex();
   //TimeSpan duration = stopTime - startTime;
   //double durationInMilliseconds = timeToMilliseconds(duration);

   if (apertureInsert != NULL)
   {
      camCadDatabase.addAttribute(&(apertureInsert->getAttributesRef()),
         getFlashRecognitionTimeKeywordIndex(camCadDatabase),
         fpfmt(timeToMilliseconds(stopTime - startTime))));
   }

#ifdef EnablePerformanceReporter
   CPerformanceReporter::getPerformanceReporter().addEntry(apertureInsert,startTime,stopTime);
#endif

   return apertureInsert;
}

#else   // defined(UseLedaLibrary)  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DataStruct* CSegmentCluster::getMergedClusterPolygonApertureInsert(CCamCadDatabase& camCadDatabase,
   const CFlashRecognizerParameters& parameters,
   int layerIndex,double width,double tolerance)
{
   TimeIndex startTime = getTimeIndex();
   DataStruct* apertureInsert = NULL;

   CTypedPtrListContainer<CPolygon*> allocatedSegmentPolygons;
   CTypedPtrListContainer<CPolygon*> segmentPolygons(false);

   CTypedPtrMap<CMapPtrToPtr,CSegmentPolygon*,CSegmentPolygon*> parentPolygonMap;
   parentPolygonMap.InitHashTable(nextPrime2n(m_list.GetCount()));

#ifdef WritePolygonsForDebugging
   CStdioFileWriteFormat debugWriteFormat("c:\\PolygonDebug.txt");
#endif

   //bool debugMergeFlag = false;

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe        = m_list.GetNext(pos);
      CSegment& segment              = segmentQfe->getSegment();
      CSegmentPolygon* parentPolygon = segment.getParentPolygon();
      CPoly* parentPoly              = segment.getParentPoly();
      CSegmentPolygon* junk;

      //if (segment.getId() == 24090)
      //{
      //   debugMergeFlag = true;
      //}

      if (parentPolygon != NULL && parentPoly != NULL && 
          camCadDatabase.getCamCadDoc().getWidth(parentPoly->getWidthIndex()) == 0.)
      {
         if (!parentPolygonMap.Lookup(parentPolygon,junk))
         {
            parentPolygonMap.SetAt(parentPolygon,parentPolygon);
            segmentPolygons.AddTail(parentPolygon);

#ifdef WritePolygonsForDebugging
            debugWriteFormat.writef("segmentId=%d, parent polygonId=%d)\n",segment.getId(),parentPolygon->getId());
            parentPolygon->write(debugWriteFormat);
#endif
         }
      }
      else
      {
         CPolygon* segmentPolygon = new CPolygon(camCadDatabase.getPageUnits());
         segment.getPolygon(*segmentPolygon);
         segmentPolygons.AddTail(segmentPolygon);
         allocatedSegmentPolygons.AddTail(segmentPolygon);

#ifdef WritePolygonsForDebugging
         debugWriteFormat.writef("segmentId=%d\n",segment.getId());
         segmentPolygon->write(debugWriteFormat);
#endif
      }
   }

   //CPolygonSubdivision::setDebugMergeFlag(debugMergeFlag);

#ifdef WritePolygonsForDebugging
   debugWriteFormat.flush();
   debugWriteFormat.close();
#endif

#if defined(UseAlgoCOMsPolygonLibrary)

   CAlgoComPolygon mergedAlgoComPolygon;

   for (POSITION pos = segmentPolygons.GetHeadPosition();pos != NULL;)
   {
      CPolygon* segmentPolygon = segmentPolygons.GetNext(pos);

      mergedAlgoComPolygon.addPolygon(*segmentPolygon);
   }

   int polygonCount = mergedAlgoComPolygon.getPolygonCount();

   if (polygonCount > 0)
   {
      CPolygon mergedPolygon;
      mergedAlgoComPolygon.getPolygonAt(mergedPolygon,0);

      apertureInsert = getComplexApertureInsert(camCadDatabase,parameters,layerIndex,width,tolerance,mergedPolygon);
   }

#else

   CSegmentQfeList segmentList(m_list,false);
   CPolygon mergedPolygon(camCadDatabase.getPageUnits());
   CPolygon segmentPolygon(camCadDatabase.getPageUnits());
   int processedSegmentCount = 0;
   int mergedSegmentCount = 0;

   for (bool progressFlag = true;progressFlag;)
   {
      progressFlag = false;

      for (POSITION pos = segmentPolygons.GetHeadPosition();pos != NULL;processedSegmentCount++)
      {
         //if (processedSegmentCount >= 2)
         //{
         //   segmentPolygons.empty();
         //   break;
         //}

         POSITION oldPos = pos;
         CPolygon* segmentPolygon = segmentPolygons.GetNext(pos);

         if (mergedPolygon.getVertexCount() == 0)
         {
            mergedPolygon = *segmentPolygon;

            segmentPolygons.RemoveAt(oldPos);
            progressFlag = true;
            mergedSegmentCount++;
         }
         else 
         {
            bool mergedFlag = mergedPolygon.merge(*segmentPolygon);

            if (mergedFlag)
            {
               segmentPolygons.RemoveAt(oldPos);
               progressFlag = true;
               mergedSegmentCount++;
            }
         }
      }

      if (segmentPolygons.IsEmpty())
      {
         apertureInsert = getComplexApertureInsert(camCadDatabase,parameters,layerIndex,width,tolerance,mergedPolygon);
         break;
      }
   }

#endif  // defined(UseAlgoCOMsPolygonLibrary)

   TimeIndex stopTime = getTimeIndex();
   TimeSpan duration = stopTime - startTime;
   double durationInMilliseconds = timeToMilliseconds(duration);

   if (apertureInsert != NULL)
   {
      camCadDatabase.addAttribute(&(apertureInsert->getAttributesRef()),
         getFlashRecognitionTimeKeywordIndex(camCadDatabase),
         fpfmt(timeToMilliseconds(stopTime - startTime)));
   }

#ifdef EnablePerformanceReporter
   CPerformanceReporter::getPerformanceReporter().addEntry(apertureInsert,flashRecognitionMethodPolygonMerged,startTime,stopTime);
#endif

   //CPolygonSubdivision::setDebugMergeFlag(false);

   return apertureInsert;
}

#endif   // defined(UseLedaLibrary)  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

DataStruct* CSegmentCluster::getDefinedRegionedComplexApertureInsert(CCamCadDatabase& camCadDatabase,
   CFlashRecognizer& flashRecognizer,
   int layerIndex,double width,double tolerance)
{
   TimeIndex startTime = getTimeIndex();
   DataStruct* apertureInsert = NULL;
   CPolyList polyList;
   CFlashRecognizerParameters& parameters = flashRecognizer.getParameters();

   CExtent extent = getExtent();
   double maxExtent = Max(extent.getXsize(),extent.getYsize());

   if (maxExtent <= 0.) maxExtent = 1.;

   double pixelsPerPageUnit = 400./maxExtent;

   CTypedPtrMap<CMapPtrToPtr,DataStruct*,DataStruct*> dataMap;
   dataMap.InitHashTable(nextPrime2n(m_list.GetCount()));

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      CSegment& segment = segmentQfe->getSegment();
      DataStruct* segmentParent = segment.getParent();

      if (segmentParent != NULL)
      {
         dataMap.SetAt(segmentParent,segmentParent);
      }

      CPoly* poly = new CPoly();
      segment.getPoly(*poly);
      polyList.AddTail(poly);
   }

   for (POSITION pos = dataMap.GetStartPosition();pos != NULL;)
   {
      DataStruct *data,*dataKey;
      dataMap.GetNextAssoc(pos,dataKey,data);

      if (data->getDataType() == dataTypePoly)
      {
         CPolyList* dataPolyList = new CPolyList(*(data->getPolyList()));
         polyList.takeData(*dataPolyList);
         delete dataPolyList;
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         InsertStruct* insert = data->getInsert();
         BlockStruct* block = camCadDatabase.getBlock(insert->getBlockNumber());

         if (block->isAperture())
         {
            CPolyList* aperturePoly = ApertureToPoly(block,insert,false);
            polyList.takeData(*aperturePoly);

            delete aperturePoly;
         }         
      }
   }

   //Region* region = RegionFromPolylist(&(camCadDatabase.getCamCadDoc()),&polyList,pixelsPerPageUnit);
   //region->setFilledFlag(true);
   CRegionPolygon regionPolygon(camCadDatabase.getCamCadDoc(),polyList,pixelsPerPageUnit);

   if (parameters.getGenerateDiagnosticLayersFlag())
   {
      FileStruct* pcbFile   = camCadDatabase.getSingleVisibleFile();

      if (pcbFile != NULL)
      {
         BlockStruct* pcbBlock = pcbFile->getBlock();

         CPolyList* rectPolyList = regionPolygon.getRectangles();

         DataStruct* polyData = camCadDatabase.addPolyStruct(pcbBlock,
            flashRecognizer.getDiagnosticLayer("Region Rectangles")->getLayerIndex(),
            0,false,graphicClassNormal);

         polyData->getPolyList()->takeData(*rectPolyList);
         delete rectPolyList;

         rectPolyList = regionPolygon.getPolys();

         if (rectPolyList != NULL)
         {
            polyData = camCadDatabase.addPolyStruct(pcbBlock,
               flashRecognizer.getDiagnosticLayer("Region Outlines")->getLayerIndex(),
               0,false,graphicClassNormal);

            polyData->getPolyList()->takeData(*rectPolyList);
            delete rectPolyList;
         }
      }
   }

   //CPolyList* mergedPolyList = region->GetPolys(scaleFactor);
   //CPolyList* mergedPolyList = region->BuildRectPolys(scaleFactor);
   //region->fillVerticalConcavities();
   //CPolyList* mergedPolyList = region->GetSmoothedPolys(pixelsPerPageUnit);
   CPolyList* mergedPolyList = regionPolygon.getSmoothedPolys();

   if (mergedPolyList != NULL)
   {

      //if (true)
      //{
      //   CPolyList* debugPolyList = region->GetPolys(scaleFactor);
      //   mergedPolyList->takeData(*debugPolyList);

      //   delete debugPolyList;
      //}

      int polyCount = mergedPolyList->GetCount();

      if (polyCount == 1)
      {
         CPoly* poly = mergedPolyList->GetHead();
         CPolygon polygon(*poly,camCadDatabase.getPageUnits());
         apertureInsert = getComplexApertureInsert(camCadDatabase,parameters,layerIndex,width,tolerance,polygon);
      }

      if (polyCount >= 1 && apertureInsert == NULL)
      {
         CTMatrix matrix;
         CPoint2d origin = getExtent().getCenter();
         matrix.translateCtm(-origin);
         mergedPolyList->transform(matrix);

         BlockStruct* apertureBlock = camCadDatabase.getNewBlock("AP_RegionAperture_%04d",blockTypeUnknown);
         DataStruct* data = camCadDatabase.getCamCadData().getNewDataStruct(dataTypePoly);
         //data->setDataType(dataTypePoly);
         data->setLayerIndex(layerIndex);
         //data->getPolyList() = new CPolyList(*mergedPolyList);
         *(data->getPolyList()) = *mergedPolyList;
         apertureBlock->getDataList().AddTail(data);

         apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
            origin.x,origin.y,0.,false);
      }

      delete mergedPolyList;
      //delete region;
   }

   TimeIndex stopTime = getTimeIndex();
   //TimeSpan duration = stopTime - startTime;
   //double durationInMilliseconds = timeToMilliseconds(duration);

   if (apertureInsert != NULL)
   {
      camCadDatabase.addAttribute(&(apertureInsert->getAttributesRef()),
         getFlashRecognitionTimeKeywordIndex(camCadDatabase),
         fpfmt(timeToMilliseconds(stopTime - startTime)));
   }

#ifdef EnablePerformanceReporter
   CPerformanceReporter::getPerformanceReporter().addEntry(apertureInsert,flashRecognitionMethodRegionMerged,startTime,stopTime);
#endif

   return apertureInsert;
}

void CSegmentCluster::normalizeForSignature(CTMatrix& matrix)
{
   if (m_list.GetCount() > 0)
   {
      m_list.sortByAscendingLength();

      // translate polygon to centroid
      CPoint2d centroid = m_list.getCentroid();
      matrix.initMatrix();
      matrix.translateCtm(centroid);
      CTMatrix translationMatrix;
      translationMatrix.translateCtm(-centroid);
      m_list.transform(translationMatrix);

      // get bottom edge
      CSegmentQfe* segmentQfe = m_list.GetHead();

      // rotate polygon so bottom edge is horizontal and below the origin
      double theta = segmentQfe->getSegment().getLine().getTheta();
      CTMatrix rotationMatrix;
      rotationMatrix.rotateRadiansCtm(-theta);
      CPoint2d p0 = segmentQfe->getSegment().getLine().get0();
      rotationMatrix.transform(p0);

      if (p0.y > 0.)
      {
         theta += Pi;
         rotationMatrix.initMatrix();
         rotationMatrix.rotateRadiansCtm(-theta);
      }

      m_list.transform(rotationMatrix);
      matrix.rotateRadiansCtm(theta);
   }
}

CString CSegmentCluster::getSignature() const
{
   CString signature;

   signature.Format("%d",m_list.GetCount());

   return signature;
}

bool CSegmentCluster::normalizedVerticesEqual(const CSegmentCluster& other,double toleranceDistance) const
{
   bool retval = false;
   double toleranceDistanceSquared = toleranceDistance * toleranceDistance;

   if (m_list.GetCount() == other.m_list.GetCount())
   {
      retval = true;
      POSITION pos      = m_list.GetHeadPosition();
      POSITION otherPos = other.m_list.GetHeadPosition();

      while (pos != NULL)
      {
         POSITION otherPos0 = otherPos;

         CSegmentQfe* segmentQfe      = m_list.GetNext(pos);
         CSegmentQfe* otherSegmentQfe = other.m_list.GetNext(otherPos);

         CSegment& segment      = segmentQfe->getSegment();
         CSegment& otherSegment = otherSegmentQfe->getSegment();

         const CLine2d& line      = segment.getLine();
         const CLine2d& otherLine = otherSegment.getLine();

         bool matchFlag = line.verticesWithinDistanceSquared(otherLine,toleranceDistanceSquared);

         if (!matchFlag)
         {
            bool matchFlag = false;
            double lenSquared    = line.getLengthSquared();
            double len           = sqrt(lenSquared);
            double minLenSquared = len - toleranceDistance;  minLenSquared *= minLenSquared;
            double maxLenSquared = len + toleranceDistance;  maxLenSquared *= maxLenSquared;

            for (POSITION otherPos2 = otherPos0;!matchFlag && otherPos2 != NULL;)
            {
               otherSegmentQfe = other.m_list.GetPrev(otherPos2);
               CSegment& otherSegment = otherSegmentQfe->getSegment();
               const CLine2d& otherLine = otherSegment.getLine();

               if (otherLine.getLengthSquared() < minLenSquared)
               {
                  break;
               }

               matchFlag = line.verticesWithinDistanceSquared(otherLine,toleranceDistanceSquared);
            }

            for (POSITION otherPos2 = otherPos;!matchFlag && otherPos2 != NULL;)
            {
               otherSegmentQfe = other.m_list.GetNext(otherPos2);
               CSegment& otherSegment = otherSegmentQfe->getSegment();
               const CLine2d& otherLine = otherSegment.getLine();

               if (otherLine.getLengthSquared() > maxLenSquared)
               {
                  break;
               }

               matchFlag = line.verticesWithinDistanceSquared(otherLine,toleranceDistanceSquared);
            }
         }

         if (!matchFlag)
         {
            retval = false;
            break;
         }
      }
   }

   return retval;
}

DataStruct* CSegmentCluster::getDefinedStandardApertureInsert(CCamCadDatabase& camCadDatabase,
   const CFlashRecognizerParameters& parameters,
   StandardApertureTypeTag apertureType,int layerIndex,const CPoint2d& origin,double angle,
   double dimension0,double dimension1,double dimension2,
   double dimension3,double dimension4,double dimension5)
{
   parameters.roundToBaseUnits(dimension0);
   parameters.roundToBaseUnits(dimension1);
   parameters.roundToBaseUnits(dimension2);
   parameters.roundToBaseUnits(dimension3);
   parameters.roundToBaseUnits(dimension4);
   parameters.roundToBaseUnits(dimension5);

   CStandardAperture standardAperture(apertureType,camCadDatabase.getPageUnits(),1);
   standardAperture.setDimensions(dimension0,dimension1,dimension2,dimension3,dimension4,dimension5);
   CString apertureName = "SA_" + standardAperture.getDescriptor();

   BlockStruct* apertureBlock = camCadDatabase.getBlock(apertureName);

   if (apertureBlock == NULL)
   {
      apertureBlock = standardAperture.createNewAperture(camCadDatabase.getCamCadData(),apertureName);
   }

   DataStruct* apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
                                   origin.x,origin.y,angle);

   return apertureInsert;
}

DataStruct* CSegmentCluster::makeApertureInsert(CCamCadDatabase& camCadDatabase,
   int layerIndex,int segregatedLayerIndex,CFlashRecognizer& flashRecognizer,
   bool simpleSearchFlag,CMessageFilter& messageFilter)
{    
   CCamCadData& camCadData = camCadDatabase.getCamCadData();

   CFlashRecognizerParameters& parameters = flashRecognizer.getParameters();
   DataStruct* apertureInsert = NULL;
   FlashRecognitionMethodTag recognitionMethod = flashRecognitionMethodUndefined;

   try
   {
      double rectangleCoordTolerance = parameters.getRectangleCoordTolerance();

      //bool debugFlag = true;
      //if (debugFlag)
      //{
      //   CClusterIndicator clusterIndicator(*this);
      //   clusterIndicator.addSegments();
      //   clusterIndicator.instantiate(camCadDatabase,
      //      camCadDatabase.getFirstVisibleFile()->getBlock(),
      //      *(camCadDatabase.getDefinedLayer("dbDisplay")),
      //      *(camCadDatabase.getDefinedLayer("dbIndicator")),
      //      *(camCadDatabase.getDefinedLayer("dbExtent"))); 
      //}

      //if (m_clusterId == 1953 || m_clusterId == 1959)
      //{
      //   int iii = 3;
      //}
      
      try
      {
         if (m_isCleanAperture)  // aperture clusters
         {
            recognitionMethod = flashRecognitionMethodSingleFlash;
            apertureInsert = camCadData.getNewDataStruct(*m_apertureInsert);
            apertureInsert->setLayerIndex(layerIndex);
         }
         else if (m_list.GetCount() == 1) // single segment clusters
         {
            CSegmentQfe* segmentQfe = m_list.GetHead();
            CSegment& segment = segmentQfe->getSegment();

            StandardApertureTypeTag apertureType = (segment.getRounded() ? standardApertureOval : standardApertureRectangle);

            double length   = segment.getExteriorLength();
            double width    = segment.getWidth();
            CPoint2d origin = segment.getOrigin();
            double angle    = segment.getLine().getTheta();

            if (width > length)
            {
               angle += Pi/2.;
            }

            recognitionMethod = flashRecognitionMethodSingleSegment;
            apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,apertureType,
                              layerIndex,origin,angle,length,width);
         }
      }
      catch (...)
      {
         messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert() - aperture/single segment clusters");
      }

      try
      {
         if (apertureInsert == NULL) 
         {
            // normalize by longest segment
            CSegmentCluster* allocatedSegmentCluster = NULL;
            CSegmentCluster* segmentCluster = this;
            CSegmentQfeList* segmentList = &m_list;
            CExtent clusterPointExtent;
            CTMatrix normalizationMatrix;
            CSegment* longestSegment = NULL;
            double maxSegmentLengthSquared = 0.;
            double normalizationAngleRadians = 0.;

            for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
            {
               CSegmentQfe* segmentQfe = m_list.GetNext(pos);
               CSegment& segment = segmentQfe->getSegment();

               double segmentLengthSquared = segment.getLineLengthSquared();

               if (segmentLengthSquared > maxSegmentLengthSquared)
               {
                  maxSegmentLengthSquared = segmentLengthSquared;
                  longestSegment = &segment;
               }
            }

            if (longestSegment != NULL)
            {
               normalizationAngleRadians = longestSegment->getLine().getTheta();

               if (normalizationAngleRadians != 0.)
               {
                  normalizationMatrix.rotateRadians(-normalizationAngleRadians);
                  allocatedSegmentCluster = new CSegmentCluster(*this,normalizationMatrix);
                  segmentCluster = allocatedSegmentCluster;
                  segmentList = &(allocatedSegmentCluster->getList());

                  //bool debugFlag = true;
                  //if (debugFlag)
                  //{
                  //   int segmentIdKeywordIndex = camCadDatabase.registerKeyword("SegmentId",valueTypeInteger);
                  //   int polygonIdKeywordIndex = camCadDatabase.registerKeyword("PolygonId",valueTypeInteger);
                  //   int clusterIdKeywordIndex = camCadDatabase.registerKeyword("ClusterId",valueTypeInteger);

                  //   CClusterIndicator clusterIndicator(*allocatedSegmentCluster);
                  //   clusterIndicator.addSegments();
                  //   clusterIndicator.instantiate(camCadDatabase,
                  //      camCadDatabase.getFirstVisibleFile()->getBlock(),
                  //      segmentIdKeywordIndex,polygonIdKeywordIndex,clusterIdKeywordIndex,
                  //      *(camCadDatabase.getDefinedLayer("dbDisplay")),
                  //      *(camCadDatabase.getDefinedLayer("dbIndicator")),
                  //      *(camCadDatabase.getDefinedLayer("dbExtent"))); 
                  //}
               }
            }

            // outlined orthogonal rectangle or rounded rectangle
            if (segmentList->GetCount() >= 4) 
            {
               CExtent clusterPointExtent = segmentCluster->getPointExtent();
               //CPoint2d clusterOriginOffset = -clusterPointExtent.getCenter();
               //clusterPointExtent.translate(clusterOriginOffset);

               CSegment *left=NULL,*right=NULL,*top=NULL,*bottom=NULL;
               bool leftRectangleSideFlag=false,rightRectangleSideFlag=false;
               bool topRectangleSideFlag=false,bottomRectangleSideFlag=false;

               for (POSITION pos = segmentList->GetHeadPosition();pos != NULL;)
               {
                  CSegmentQfe* segmentQfe = segmentList->GetNext(pos);
                  CSegment& segment = segmentQfe->getSegment();
                  CExtent lineExtent = segment.getLine().getExtent();
                  //lineExtent.translate(clusterOriginOffset);

                  double lineLengthSquared = segment.getLineLengthSquared();

                  if (fpnear(lineExtent.getXmin(),lineExtent.getXmax(),rectangleCoordTolerance))  // vertical line
                  {
                     if (fpnear(lineExtent.getXmin(),clusterPointExtent.getXmin(),rectangleCoordTolerance)) // vertical line at left extent
                     {
                        if (!leftRectangleSideFlag && (left == NULL || left->getLineLengthSquared() < lineLengthSquared))
                        {
                           left = &segment;
                           leftRectangleSideFlag = (fpnear(lineExtent.getYmin(),clusterPointExtent.getYmin(),rectangleCoordTolerance) &&
                                                   fpnear(lineExtent.getYmax(),clusterPointExtent.getYmax(),rectangleCoordTolerance));
                        }
                     }
                     else if (fpnear(lineExtent.getXmin(),clusterPointExtent.getXmax(),rectangleCoordTolerance)) // vertical line at right extent
                     {
                        if (!rightRectangleSideFlag && (right == NULL || right->getLineLengthSquared() < lineLengthSquared))
                        {
                           right = &segment;
                           rightRectangleSideFlag = (fpnear(lineExtent.getYmin(),clusterPointExtent.getYmin(),rectangleCoordTolerance) &&
                                                   fpnear(lineExtent.getYmax(),clusterPointExtent.getYmax(),rectangleCoordTolerance));
                        }
                     }            
                  }
                  else if (fpnear(lineExtent.getYmin(),lineExtent.getYmax(),rectangleCoordTolerance))  // horizontal line
                  {
                     if (fpnear(lineExtent.getYmin(),clusterPointExtent.getYmin(),rectangleCoordTolerance)) // horizontal line at bottom extent
                     {
                        if (!bottomRectangleSideFlag && (bottom == NULL || bottom->getLineLengthSquared() < lineLengthSquared))
                        {
                           bottom = &segment;
                           bottomRectangleSideFlag = (fpnear(lineExtent.getXmin(),clusterPointExtent.getXmin(),rectangleCoordTolerance) &&
                                                      fpnear(lineExtent.getXmax(),clusterPointExtent.getXmax(),rectangleCoordTolerance));
                        }
                     }
                     else if (fpnear(lineExtent.getYmin(),clusterPointExtent.getYmax(),rectangleCoordTolerance)) // horizontal line at top extent
                     {
                        if (!topRectangleSideFlag && (top == NULL || top->getLineLengthSquared() < lineLengthSquared))
                        {
                           top = &segment;
                           topRectangleSideFlag = (fpnear(lineExtent.getXmin(),clusterPointExtent.getXmin(),rectangleCoordTolerance) &&
                                                   fpnear(lineExtent.getXmax(),clusterPointExtent.getXmax(),rectangleCoordTolerance));
                        }
                     }            
                  }

                  if (leftRectangleSideFlag && rightRectangleSideFlag && topRectangleSideFlag && bottomRectangleSideFlag)
                  {  // rectangle
                     double diameter = left->getWidth();

                     if (fpeq(diameter,right->getWidth()) &&
                        fpeq(diameter,top->getWidth())   &&
                        fpeq(diameter,bottom->getWidth())   )
                     {
                        double radius = diameter/2.;
                        bool roundedRectangleFlag = (radius >= parameters.getMinimumCornerRadius());

                        StandardApertureTypeTag apertureType = (roundedRectangleFlag ? standardApertureRoundedRectangle : 
                                                                                       standardApertureRectangle);
                        radius *= roundedRectangleFlag;
                        CSize2d extentSize = segmentCluster->getExtent().getSize();
                        double length   = Max(extentSize.cx,extentSize.cy);
                        double width    = Min(extentSize.cx,extentSize.cy);
                        double angle    = ((extentSize.cx >= extentSize.cy) ? 0. : Pi/2.);
                        CPoint2d origin = segmentCluster->getExtent().getCenter();

                        //CStandardAperture standardAperture(apertureType,camCadDatabase.getPageUnits());
                        //standardAperture.setDimensions(length,width,radius);
                        //CString apertureName = "SA_" + standardAperture.getDescriptor();

                        //BlockStruct* apertureBlock = camCadDatabase.getBlock(apertureName);

                        //if (apertureBlock == NULL)
                        //{
                        //   apertureBlock = standardAperture.createNewAperture(camCadDatabase,apertureName);
                        //}

                        //apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
                        //                                             origin.x,origin.y,angle);
                        recognitionMethod = flashRecognitionMethodOrthogonalRectangle;
                        apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,apertureType,
                                          layerIndex,origin,angle,length,width,radius);
                     }

                     break;
                  }
               }

               if (apertureInsert == NULL && left != NULL && right != NULL && top != NULL && bottom != NULL &&
                  left->getRounded() && right->getRounded() && top->getRounded() && bottom->getRounded())
               {  // possible rectangle variation
                  double minLeftY   = min(  left->get0().y,  left->get1().y);
                  double maxRightY  = max( right->get0().y, right->get1().y);
                  double minTopX    = min(   top->get0().x,   top->get1().x);
                  double maxBottomX = max(bottom->get0().x,bottom->get1().x);

                  if (minLeftY < maxRightY && minTopX < maxBottomX)
                  {  // preclude skewed sides
                     const double chamferTolerance = parameters.getRadialTolerance();
                     CExtent extent     = segmentCluster->getExtent();
                     CSize2d extentSize = extent.getSize();
                     double leftDelta     = extentSize.cy - left->getLine().getLength();
                     double rightDelta    = extentSize.cy - right->getLine().getLength();
                     double topDelta      = extentSize.cx - top->getLine().getLength();
                     double bottomDelta   = extentSize.cx - bottom->getLine().getLength();
                     double delta         = Max(Max(leftDelta,rightDelta),Max(topDelta,bottomDelta));
                     StandardApertureTypeTag apertureType = standardApertureChamferedRectangle;

                     // chamfer diameters must be within chamfer tolerance of each other
                     if (fpeq(delta,leftDelta  ,chamferTolerance) &&
                        fpeq(delta,rightDelta ,chamferTolerance) &&
                        fpeq(delta,topDelta   ,chamferTolerance) &&
                        fpeq(delta,bottomDelta,chamferTolerance)   )
                     {
                        double radius   = delta/2.;
                        double originalRadius = radius;
                        double length   = Max(extentSize.cx,extentSize.cy);
                        double width    = Min(extentSize.cx,extentSize.cy);
                        double angle    = ((extentSize.cx >= extentSize.cy) ? 0. : Pi/2.);
                        CPoint2d origin = segmentCluster->getExtent().getCenter();

                        if (radius < parameters.getMinimumCornerRadius())
                        {
                           recognitionMethod = flashRecognitionMethodOrthogonalRectangleVariant;
                           apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,
                                             standardApertureRectangle,
                                             layerIndex,origin,angle,length,width);
                        }
                        else
                        {
                           for (int pass = 0;pass < 2 && apertureInsert == NULL;pass++)
                           {
                              if (pass == 1)
                              {
                                 apertureType = standardApertureRoundedRectangle;
                                 double roundedRectangleChamfer = radius*(SqrtOf2 - 1.);
                                 //extentSize.cx += parameters.getRoundedCornerRadiusTolerance();
                                 //extentSize.cy += parameters.getRoundedCornerRadiusTolerance();
                                 extentSize.cx *= 1.001;
                                 extentSize.cy *= 1.001;

                                 radius /= SqrtOf2;
                                 radius -= parameters.getRoundedCornerRadiusTolerance();

                                 if (radius < 0.) radius = 0.;

                                 delta  = radius * 2.;
                              }

                              // chamfered rectangle test
                              CPolygon chamferedRectangle(camCadDatabase.getPageUnits());
                              CPoint2d point(origin);

                              point.x +=  extentSize.cx/2.;
                              point.y += -extentSize.cy/2. + radius;
                              chamferedRectangle.addVertex(point);

                              point.y += extentSize.cy - delta;
                              chamferedRectangle.addVertex(point);

                              point.x -= radius;
                              point.y += radius;
                              chamferedRectangle.addVertex(point);

                              point.x -= extentSize.cx - delta;;
                              chamferedRectangle.addVertex(point);

                              point.x -= radius;
                              point.y -= radius;
                              chamferedRectangle.addVertex(point);

                              point.y -= extentSize.cy - delta;
                              chamferedRectangle.addVertex(point);

                              point.x += radius;
                              point.y -= radius;
                              chamferedRectangle.addVertex(point);

                              point.x += extentSize.cx - delta;;
                              chamferedRectangle.addVertex(point);

                              //point.x += radius;
                              //point.y -= radius;
                              //chamferedRectangle.addPoint(point);

                              //if (pass == 3)
                              //{
                              //   int layerIndex = camCadDatabase.getDefinedLayerIndex("test");
                              //   apertureInsert = camCadDatabase.newPolyData(chamferedRectangle,
                              //                           layerIndex,0,false,graphicClassNormal);
                              //   break;
                              //}

                              if (segmentCluster->allSegmentsInsidePolygon(chamferedRectangle))
                              {
                                 //CStandardAperture standardAperture(standardApertureChamferedRectangle,camCadDatabase.getPageUnits());
                                 //standardAperture.setDimensions(length,width,radius);
                                 //CString apertureName = "SA_" + standardAperture.getDescriptor();

                                 //BlockStruct* apertureBlock = camCadDatabase.getBlock(apertureName);

                                 //if (apertureBlock == NULL)
                                 //{
                                 //   apertureBlock = standardAperture.createNewAperture(camCadDatabase,apertureName);
                                 //}

                                 //apertureInsert = camCadDatabase.insertBlock(apertureBlock,insertTypeFreePad,"",layerIndex,
                                 //                                             origin.x,origin.y,angle);
                                 recognitionMethod = flashRecognitionMethodOrthogonalRectangleVariant;
                                 apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,apertureType,
                                                   layerIndex,origin,angle,length,width,originalRadius,1234.);
                              }
                              //else if (pass == 2)
                              //{
                              //   CPolygon unitSquare;
                              //   unitSquare.addPoint(CPoint2d( 100., 100.));
                              //   unitSquare.addPoint(CPoint2d(-100., 100.));
                              //   unitSquare.addPoint(CPoint2d(-100.,-100.));
                              //   unitSquare.addPoint(CPoint2d( 100.,-100.));

                              //   bool inFlag = allSegmentsInsidePolygon(unitSquare);
                              //}
                           }
                        }
                     }
                  }
               }
            }

            if (apertureInsert != NULL && allocatedSegmentCluster != NULL)
            {
               normalizationMatrix.invert();
               apertureInsert->transform(normalizationMatrix);
            }

            delete allocatedSegmentCluster;
         }
      }
      catch (...)
      {
         messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert() - outlined orthogonal rectangle or rounded rectangle");
      }

      try
      {
         // orthogonal rectangle composed of 2 round draws and 1 rectangular draw
         if (apertureInsert == NULL && m_list.GetCount() == 3)
         {
            CExtent clusterPointExtent = getPointExtent();

            CSegment *round0=NULL,*round1=NULL,*rectangle=NULL;
            bool leftRectangleSideFlag=false,rightRectangleSideFlag=false;
            bool topRectangleSideFlag=false,bottomRectangleSideFlag=false;

            for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
            {
               CSegmentQfe* segmentQfe = m_list.GetNext(pos);
               CSegment& segment = segmentQfe->getSegment();

               if (segment.getRounded())
               {
                  if (round0 == NULL)
                  {
                     round0 = &segment;
                  }
                  else
                  {
                     round1 = &segment;
                  }
               }
               else
               {
                  rectangle = &segment;
               }
            }

            if (round0 != NULL && round1 != NULL && rectangle != NULL)
            {
               if (fpeq(round0->getWidth(),round1->getWidth()))
               {
                  CExtent round0Extent    = round0->getExtent();
                  CExtent round1Extent    = round1->getExtent();
                  CExtent rectangleExtent = rectangle->getExtent();
                  CExtent clusterExtent   = getExtent();
                  double tolerance = Max(.005,Min(round0Extent.getXsize(),round0Extent.getYsize())/100.);

                  if (fpnear(round0Extent.getXsize(),round1Extent.getXsize(),tolerance) &&
                      fpnear(round0Extent.getYsize(),round1Extent.getYsize(),tolerance)    )
                  {
                     bool horizontalFlag = false;
                     bool alignedFlag    = false;
                     bool coveredFlag    = false;
                     bool verticalFlag = (fpeq(round0->get0().x,round0->get1().x) && 
                                          fpeq(round1->get0().x,round1->get1().x) );

                     if (verticalFlag)
                     {
                        alignedFlag = (fpnear(round0Extent.getYmax(),round1Extent.getYmax()   ,tolerance) &&
                                       fpnear(round0Extent.getYmax(),rectangleExtent.getYmax(),tolerance));

                        coveredFlag = (fpnear(rectangleExtent.getYsize(),clusterExtent.getYsize(),tolerance) &&
                                       fpnear(rectangleExtent.getXsize(),fabs(round0->get0().x - round1->get0().x),tolerance));
                     }
                     else
                     {
                        horizontalFlag = (fpeq(round0->get0().y,round0->get1().y) && 
                                          fpeq(round1->get0().y,round1->get1().y) );

                        if (horizontalFlag)
                        {
                           alignedFlag = (fpnear(round0Extent.getXmax(),round1Extent.getXmax()   ,tolerance) &&
                                          fpnear(round0Extent.getXmax(),rectangleExtent.getXmax(),tolerance));

                           coveredFlag = (fpnear(rectangleExtent.getXsize(),clusterExtent.getXsize(),tolerance) &&
                                          fpnear(rectangleExtent.getYsize(),fabs(round0->get0().y - round1->get0().y),tolerance));
                        }
                     }

                     if (alignedFlag && coveredFlag)
                     {
                        CSize2d extentSize = clusterExtent.getSize();
                        double radius      = round0->getWidth()/2.;
                        double length      = Max(extentSize.cx,extentSize.cy);
                        double width       = Min(extentSize.cx,extentSize.cy);
                        double angle       = ((extentSize.cx >= extentSize.cy) ? 0. : Pi/2.);
                        CPoint2d origin    = clusterExtent.getCenter();

                        recognitionMethod = flashRecognitionMethodThreeEntityOblong;
                        apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,standardApertureRoundedRectangle,
                                          layerIndex,origin,angle,length,width,radius,1234.);
                     }
                  }
               }
            }
         }
      }
      catch (...)
      {
         messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert() - orthogonal rectangle composed of 2 round draws and 1 rectangular draw");
      }

      try
      {
         if (apertureInsert == NULL && !simpleSearchFlag) 
         {
            if (parameters.getSearchForBoundedClusters()) 
            {
               CPolygon boundaryPolygon(camCadDatabase.getPageUnits());
               double width;

               if (getBoundaryPolygon(camCadDatabase,boundaryPolygon,parameters.getClusterBoundaryAffinity(),width,
                     parameters.getMaxBoundarySearchIterationCount()))
               {
                  double boundaryPolygonArea = boundaryPolygon.getArea();
                  double clusterExtentArea   = getExtent().getArea();
                  double areaRatio           = ((clusterExtentArea != 0.) ? boundaryPolygonArea/clusterExtentArea : 0.);

                  if (areaRatio > parameters.getClusterBoundaryFitnessFactor())
                  {
                     if (boundaryPolygon.getVertexCount() >= parameters.getMinimumCircleVertexCount())
                     {
                        double minRadius,maxRadius;

                        if (boundaryPolygon.getCircumcircleRadii(minRadius,maxRadius))
                        {
                           if (fpnear(minRadius,maxRadius,parameters.getCircumcircleTolerance()))
                           {
                              double area = boundaryPolygon.getArea();
                              double circularArea = Pi * maxRadius * maxRadius;
                              double areaRatio = Min(circularArea,area) / Max(circularArea,area);

                              if (areaRatio > .99)
                              {
                                 // circular boundary found
                                 CPoint2d origin = boundaryPolygon.getCenterOfVertices();
                                 double angle = 0.;

                                 recognitionMethod = flashRecognitionMethodCircularBoundary;
                                 apertureInsert = getDefinedStandardApertureInsert(camCadDatabase,parameters,standardApertureCircle,
                                                   layerIndex,origin,angle,maxRadius * 2.0);
                              }
                           }
                        }
                     }

                     if (apertureInsert == NULL)
                     {
                        CTMatrix matrix,iMatrix;
                        CBasesVector basesVector;

                        CPolygon normalizedBoundaryPolygon(boundaryPolygon);
                        normalizedBoundaryPolygon.normalizeForSignature(matrix);
                        matrix.inverse(iMatrix);
                        basesVector.transform(iMatrix);

                        if (!fpeq(basesVector.getRotationRadians(),0.))
                        {
                           CSegmentCluster rotatedCluster(*this,iMatrix);

                           recognitionMethod = flashRecognitionMethodUndefined;  // allows assigned attribute to remain
                           apertureInsert = rotatedCluster.makeApertureInsert(camCadDatabase,
                                             layerIndex,segregatedLayerIndex,flashRecognizer,true,messageFilter);

                           if (apertureInsert != NULL)
                           {
                              apertureInsert->transform(matrix);
                           }
                        }

                        if (apertureInsert == NULL)
                        {
                           recognitionMethod = flashRecognitionMethodPolygonBoundary;
                           apertureInsert = getComplexApertureInsert(camCadDatabase,parameters,layerIndex,
                                             width,parameters.getApertureNormalizationAffinity(),boundaryPolygon);
                        }
                     }
                  }
               }
            }

            if (apertureInsert == NULL && parameters.getMergeClustersIntoPolygon()) 
            {
               try
               {
                  recognitionMethod = flashRecognitionMethodPolygonMerged;
                  apertureInsert = getMergedClusterPolygonApertureInsert(camCadDatabase,parameters,layerIndex,
                                    0.,parameters.getApertureNormalizationAffinity());
               }
               catch (...)
               {
                  //messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert() - getMergedClusterPolygonApertureInsert()");
                  messageFilter.formatMessage(
"Some clusters cannot be merged using the polygon merge algorithm; "
"suggest unchecking the \"Merge clusters into Polygons\" option.");
               }
            }

            if (apertureInsert == NULL && parameters.getMergeClustersIntoRegion()) 
            {
               recognitionMethod = flashRecognitionMethodRegionMerged;
               apertureInsert = getDefinedRegionedComplexApertureInsert(camCadDatabase,flashRecognizer,layerIndex,
                                 0.,parameters.getApertureNormalizationAffinity());
            }

            if (apertureInsert == NULL && parameters.getConvertComplexClusters()) 
            {
               recognitionMethod = flashRecognitionMethodComplexCluster;
               apertureInsert = getComplexClusterApertureInsert(camCadDatabase,parameters,segregatedLayerIndex,
                                 0.,parameters.getApertureNormalizationAffinity());
            }
         }
      }
      catch (...)
      {
         messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert() - complex search");
      }
   }
   catch (...)
   {
      messageFilter.formatMessage("Exception caught: CSegmentCluster::makeApertureInsert()");
   }

   if (apertureInsert != NULL && recognitionMethod != flashRecognitionMethodUndefined)
   {
      camCadDatabase.addAttribute(&(apertureInsert->getAttributesRef()),
         getFlashRecognitionMethodKeywordIndex(camCadDatabase),
         flashRecognitionMethodToFriendlyString(recognitionMethod));
   }

   return apertureInsert;
}

void CSegmentCluster::dump(CWriteFormat& writeFormat,int depth)
{
   writeFormat.pushHeader(".  ");
   writeFormat.writef("CSegmentCluster %p\n",this);

   for (POSITION pos = m_list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_list.GetNext(pos);
      segmentQfe->getSegment().dump(writeFormat,depth);
   }

   writeFormat.popHeader();
}

//_____________________________________________________________________________
CSegmentClusterMap::CSegmentClusterMap()
{
   m_map.InitHashTable(nextPrime2n(20000));
}

CSegmentClusterMap::~CSegmentClusterMap()
{
}

//bool CSegmentClusterMap::lookup(CSegmentQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}
//
//bool CSegmentClusterMap::setAt(CSegmentQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}
//
//bool CSegmentClusterMap::deleteAt(CSegmentQfeList* qfeList)
//{
//   bool retval = true;
//
//   return retval;
//}

bool CSegmentClusterMap::merge(CSegmentQfe& segmentQfe0,CSegmentQfe& segmentQfe1)
{
   bool retval = true;

   if (segmentQfe0.getCluster() == NULL)
   {
      if (segmentQfe1.getCluster() == NULL)
      {
         CSegmentCluster* cluster = new CSegmentCluster();
         m_map.SetAt(cluster,cluster);

         cluster->merge(segmentQfe0);
         cluster->merge(segmentQfe1);
      }
      else
      {
         segmentQfe1.getCluster()->merge(segmentQfe0);
      }
   }
   else if (segmentQfe1.getCluster() == NULL)
   {
      segmentQfe0.getCluster()->merge(segmentQfe1);
   }
   else if (segmentQfe0.getCluster() != segmentQfe1.getCluster())
   {
      CSegmentCluster* cluster1 = segmentQfe1.getCluster();
      segmentQfe0.getCluster()->merge(segmentQfe1);
      m_map.RemoveKey(cluster1);
      delete cluster1;
   }
   else
   {
      retval = false;
   }

   return retval;
}

bool CSegmentClusterMap::add(CSegmentQfe& segmentQfe)
{
   bool retval = (segmentQfe.getCluster() == NULL);

   if (retval)
   {
      CSegmentCluster* cluster = new CSegmentCluster();
      m_map.SetAt(cluster,cluster);

      cluster->merge(segmentQfe);
   }

   return retval;
}

void CSegmentClusterMap::add(CSegmentCluster& segmentCluster)
{
   m_map.SetAt(&segmentCluster,&segmentCluster);
}

//_____________________________________________________________________________
CFlashRecognizerParameters::CFlashRecognizerParameters(CCamCadDatabase& camCadDatabase) :
   m_camCadDatabase(camCadDatabase)
{
   setToDefaults();
}

CFlashRecognizerParameters::CFlashRecognizerParameters(const CFlashRecognizerParameters& other) :
   m_camCadDatabase(other.m_camCadDatabase)
{
   *this = other;
}

CFlashRecognizerParameters& CFlashRecognizerParameters::operator=(const CFlashRecognizerParameters& other)
{
   if (&other != this)
   {
      m_minimumFeatureSize              = other.m_minimumFeatureSize;
      m_maximumFeatureSize              = other.m_maximumFeatureSize;
      m_ignoreWidthLimit                = other.m_ignoreWidthLimit;
      m_minimumWidthToSet               = other.m_minimumWidthToSet;
      m_minimumCornerRadius             = other.m_minimumCornerRadius;
      m_roundedCornerRadiusTolerance    = other.m_roundedCornerRadiusTolerance;
      m_radialTolerance                 = other.m_radialTolerance;
      m_baseUnit                        = other.m_baseUnit;
      m_apertureNormalizationAffinity   = other.m_apertureNormalizationAffinity;
      m_clusterBoundaryAffinity         = other.m_clusterBoundaryAffinity;
      m_clusterBoundaryFitnessFactor    = other.m_clusterBoundaryFitnessFactor;
      m_circumcircleTolerance           = other.m_circumcircleTolerance;
      m_rectangleCoordTolerance         = other.m_rectangleCoordTolerance;
      m_minimumCircleVertexCount        = other.m_minimumCircleVertexCount;
      m_maxBoundarySearchIterationCount = other.m_maxBoundarySearchIterationCount;
      m_maxClusterSegments              = other.m_maxClusterSegments;

      m_selectedFlag                    = other.m_selectedFlag;
      m_searchForBoundedClustersFlag    = other.m_searchForBoundedClustersFlag;
      m_mergeClustersIntoPolygonFlag    = other.m_mergeClustersIntoPolygonFlag;
      m_mergeClustersIntoRegionFlag     = other.m_mergeClustersIntoRegionFlag;
      m_generateClusterDisplayFlag      = other.m_generateClusterDisplayFlag;
      m_generateDiagnosticLayersFlag    = other.m_generateDiagnosticLayersFlag;
      m_segregateClusterAperturesFlag   = other.m_segregateClusterAperturesFlag;
      m_convertComplexClusters          = other.m_convertComplexClusters;
   }

   return *this;
}

void CFlashRecognizerParameters::setToDefaults()
{
   switch (m_camCadDatabase.getPageUnits())
   {
   case pageUnitsMilliMeters:
   case pageUnitsCentiMicroMeters:
   case pageUnitsMicroMeters:
   case pageUnitsNanoMeters:
      m_minimumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .00);
      m_maximumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters,8.00);
      m_ignoreWidthLimit              = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .02);
      m_minimumWidthToSet             = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .04);
      m_minimumCornerRadius           = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .16);
      m_roundedCornerRadiusTolerance  = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .04);
      m_baseUnit                      = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .02);
      m_apertureNormalizationAffinity = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .04);
      m_clusterBoundaryAffinity       = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .0004);
      m_circumcircleTolerance         = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .004);
      m_rectangleCoordTolerance       = m_camCadDatabase.convertToPageUnits(pageUnitsMilliMeters, .004);
      break;

   case pageUnitsInches:
   case pageUnitsMils:
   case pageUnitsHpPlotter:
   default:
      m_minimumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.0);
      m_maximumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnitsMils,200.0);
      m_ignoreWidthLimit              = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.5);
      m_minimumWidthToSet             = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  1.0);
      m_minimumCornerRadius           = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  4.0);
      m_roundedCornerRadiusTolerance  = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  1.0);
      m_baseUnit                      = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.5);
      m_apertureNormalizationAffinity = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  1.0);
      m_clusterBoundaryAffinity       = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.01);
      m_circumcircleTolerance         = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.1);
      m_rectangleCoordTolerance       = m_camCadDatabase.convertToPageUnits(pageUnitsMils,  0.1);
      break;
   }

   m_radialTolerance                 =   .06;
   m_minimumCircleVertexCount        =    16;
   m_maxBoundarySearchIterationCount = 10000;
   m_maxClusterSegments              =  5000;
   m_clusterBoundaryFitnessFactor    =    .5;

   m_selectedFlag                  = false;
   m_generateClusterDisplayFlag    = false;
   m_generateDiagnosticLayersFlag  = false;
   m_segregateClusterAperturesFlag = false;
   m_searchForBoundedClustersFlag  = false;
   m_mergeClustersIntoRegionFlag   = true;
   m_mergeClustersIntoPolygonFlag  = true;
   m_convertComplexClusters        = true;
}

void CFlashRecognizerParameters::roundToBaseUnits(double& dimension) const
{
   double sign = ((dimension < 0.) ? -1. : 1.);
   dimension *= sign;

   if (dimension != 0. && m_baseUnit != 0.)
   {
      if (dimension != 0. && dimension < m_baseUnit)
      {
         dimension = m_baseUnit;
      }
      else
      {
         int baseUnits = DcaRound(dimension/m_baseUnit);
         dimension = baseUnits * m_baseUnit;
      }

      dimension *= sign;
   }
}

void CFlashRecognizerParameters::loadFromRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("FlashRecognizerParameters");

      if (registryKey.isOpen())
      {
         registryKey.getValue("MinimumFeatureSize"             ,m_minimumFeatureSize             );
         registryKey.getValue("MaximumFeatureSize"             ,m_maximumFeatureSize             );
         registryKey.getValue("IgnoreWidthLimit"               ,m_ignoreWidthLimit               );
         registryKey.getValue("MinimumCornerRadius"            ,m_minimumCornerRadius            );
         registryKey.getValue("RoundedCornerRadiusTolerance"   ,m_roundedCornerRadiusTolerance   );
         registryKey.getValue("RadialTolerance"                ,m_radialTolerance                );
         registryKey.getValue("BaseUnit"                       ,m_baseUnit                       );
         registryKey.getValue("AffinityDistance"               ,m_apertureNormalizationAffinity  );
         registryKey.getValue("ClusterBoundaryAffinity"        ,m_clusterBoundaryAffinity        );
         registryKey.getValue("ClusterBoundaryFitnessFactor"   ,m_clusterBoundaryFitnessFactor   );
         registryKey.getValue("CircumcircleTolerance"          ,m_circumcircleTolerance          );
         registryKey.getValue("RectangleCoordTolerance"        ,m_rectangleCoordTolerance        );
         registryKey.getValue("minimumCircleVertexCount"       ,m_minimumCircleVertexCount       );
         registryKey.getValue("maxBoundarySearchIterationCount",m_maxBoundarySearchIterationCount);
         registryKey.getValue("maxClusterSegments"             ,m_maxClusterSegments             );
         registryKey.getValue("SelectedFlag"                   ,m_selectedFlag                   );
         registryKey.getValue("SearchForBoundedClustersFlag"   ,m_searchForBoundedClustersFlag   );
         registryKey.getValue("MergeClustersIntoPolygonFlag"   ,m_mergeClustersIntoPolygonFlag   );         
         registryKey.getValue("MergeClustersIntoRegionFlag"    ,m_mergeClustersIntoRegionFlag    );         
         registryKey.getValue("GenerateClusterDisplayFlag"     ,m_generateClusterDisplayFlag     );
         registryKey.getValue("GenerateDiagnosticLayersFlag"   ,m_generateDiagnosticLayersFlag   );
         registryKey.getValue("SegregateClusterApertures"      ,m_segregateClusterAperturesFlag  );
         registryKey.getValue("ConvertComplexClusters"         ,m_convertComplexClusters         );         

         int iPageUnits;

         if (registryKey.getValue("PageUnits",iPageUnits))
         {
            PageUnitsTag pageUnits = intToPageUnitsTag(iPageUnits);

            if (pageUnits != m_camCadDatabase.getPageUnits())
            {
               m_minimumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnits,m_minimumFeatureSize);
               m_maximumFeatureSize            = m_camCadDatabase.convertToPageUnits(pageUnits,m_maximumFeatureSize);
               m_ignoreWidthLimit              = m_camCadDatabase.convertToPageUnits(pageUnits,m_ignoreWidthLimit);
               m_minimumCornerRadius           = m_camCadDatabase.convertToPageUnits(pageUnits,m_minimumCornerRadius);
               m_roundedCornerRadiusTolerance  = m_camCadDatabase.convertToPageUnits(pageUnits,m_roundedCornerRadiusTolerance);
               m_radialTolerance               = m_camCadDatabase.convertToPageUnits(pageUnits,m_radialTolerance);
               m_baseUnit                      = m_camCadDatabase.convertToPageUnits(pageUnits,m_baseUnit);
               m_apertureNormalizationAffinity = m_camCadDatabase.convertToPageUnits(pageUnits,m_apertureNormalizationAffinity);
               m_clusterBoundaryAffinity       = m_camCadDatabase.convertToPageUnits(pageUnits,m_clusterBoundaryAffinity);
               m_clusterBoundaryFitnessFactor  = m_camCadDatabase.convertToPageUnits(pageUnits,m_clusterBoundaryFitnessFactor);
               m_circumcircleTolerance         = m_camCadDatabase.convertToPageUnits(pageUnits,m_circumcircleTolerance);
               m_rectangleCoordTolerance       = m_camCadDatabase.convertToPageUnits(pageUnits,m_rectangleCoordTolerance);
            }
         }
      }
   }
}

void CFlashRecognizerParameters::storeInRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("FlashRecognizerParameters");

      if (registryKey.isOpen())
      {
         registryKey.setValue("PageUnits"                      ,m_camCadDatabase.getPageUnits()  );
         registryKey.setValue("MinimumFeatureSize"             ,m_minimumFeatureSize             );
         registryKey.setValue("MaximumFeatureSize"             ,m_maximumFeatureSize             );
         registryKey.setValue("IgnoreWidthLimit"               ,m_ignoreWidthLimit               );
         registryKey.setValue("MinimumCornerRadius"            ,m_minimumCornerRadius            );
         registryKey.setValue("RoundedCornerRadiusTolerance"   ,m_roundedCornerRadiusTolerance   );
         registryKey.setValue("RadialTolerance"                ,m_radialTolerance                );
         registryKey.setValue("BaseUnit"                       ,m_baseUnit                       );
         registryKey.setValue("AffinityDistance"               ,m_apertureNormalizationAffinity  );
         registryKey.setValue("ClusterBoundaryAffinity"        ,m_clusterBoundaryAffinity        );
         registryKey.setValue("ClusterBoundaryFitnessFactor"   ,m_clusterBoundaryFitnessFactor   );
         registryKey.setValue("CircumcircleTolerance"          ,m_circumcircleTolerance          );
         registryKey.setValue("RectangleCoordTolerance"        ,m_rectangleCoordTolerance        );
         registryKey.setValue("minimumCircleVertexCount"       ,m_minimumCircleVertexCount       );
         registryKey.setValue("maxBoundarySearchIterationCount",m_maxBoundarySearchIterationCount);
         registryKey.setValue("maxClusterSegments"             ,m_maxClusterSegments             );
         registryKey.setValue("SelectedFlag"                   ,m_selectedFlag                   );
         registryKey.setValue("SearchForBoundedClustersFlag"   ,m_searchForBoundedClustersFlag   );
         registryKey.setValue("MergeClustersIntoPolygonFlag"   ,m_mergeClustersIntoPolygonFlag   );         
         registryKey.setValue("MergeClustersIntoRegionFlag"    ,m_mergeClustersIntoRegionFlag    );         
         registryKey.setValue("GenerateClusterDisplayFlag"     ,m_generateClusterDisplayFlag     );
         registryKey.setValue("GenerateDiagnosticLayersFlag"   ,m_generateDiagnosticLayersFlag   );         
         registryKey.setValue("SegregateClusterApertures"      ,m_segregateClusterAperturesFlag  );
         registryKey.setValue("ConvertComplexClusters"         ,m_convertComplexClusters         );
      }
   }
}

//_____________________________________________________________________________
//CCamCadDatabase* CClusterIndicator::m_camCadDatabase        = NULL;
//BlockStruct*     CClusterIndicator::m_pcbBlock              = NULL;
//LayerStruct*     CClusterIndicator::m_clusterDisplayLayer   = NULL;
//LayerStruct*     CClusterIndicator::m_clusterIndicatorLayer = NULL;
//LayerStruct*     CClusterIndicator::m_clusterExtentLayer    = NULL;

CClusterIndicator::CClusterIndicator(CSegmentCluster& segmentCluster) :
   m_segmentCluster(segmentCluster)
{
   m_clusterOrigin = segmentCluster.getExtent().getCenter();
   m_segmentQuadrants[0] = &m_segmentArray0;
   m_segmentQuadrants[1] = &m_segmentArray1;
   m_segmentQuadrants[2] = &m_segmentArray2;
   m_segmentQuadrants[3] = &m_segmentArray3;
}

void CClusterIndicator::addSegment(CSegment& segment)
{
   CPoint2d perimeterPoint = segment.getOrigin() - m_clusterOrigin;
   int quadrant;

   if (perimeterPoint.x >= 0)
   {
      quadrant = ((perimeterPoint.y >= 0) ? 0 : 3);
   }
   else
   {
      quadrant = ((perimeterPoint.y >= 0) ? 1 : 2);
   }

   //double theta  = perimeterPoint.getTheta();
   //double degrees = normalizeDegrees(radiansToDegrees(theta));
   //int quadrant = (int)(degrees / 90.);
   CSegmentArray& segmentArray = *(m_segmentQuadrants[quadrant]);

   CPoint2d perspectivePoint((quadrant == 0 || quadrant == 3) ? -1. : 1.,
                             (quadrant == 0 || quadrant == 1) ? -1. : 1.);

   double theta = CPoint2d(perimeterPoint - perspectivePoint).getTheta();

   int index;

   for (index = 0;index < segmentArray.GetSize();index++)
   {
      CSegment* existingSegment = segmentArray.GetAt(index);
      CPoint2d existingPerimeterPoint = existingSegment->getOrigin() - m_clusterOrigin;
      double existingTheta = CPoint2d(existingPerimeterPoint - perspectivePoint).getTheta();

      if (existingTheta > theta)
      {
         break;
      }
   }

   segmentArray.InsertAt(index,&segment);
}

void CClusterIndicator::addSegments()
{
   CSegmentQfeList& list = m_segmentCluster.getList();

   for (POSITION pos = list.GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = list.GetNext(pos);
      addSegment(segmentQfe->getSegment());
   }
}

void CClusterIndicator::instantiate(CCamCadDatabase& camCadDatabase,BlockStruct* pcbBlock,
   int segmentIdKeywordIndex,int polygonIdKeywordIndex,int clusterIdKeywordIndex,
   LayerStruct& clusterDisplayLayer,LayerStruct& clusterIndicatorLayer,LayerStruct& clusterExtentLayer)
{
   CNullWriteFormat errorLog;

   //m_camCadDatabase        = & camCadDatabase;
   //m_pcbBlock              = pcbBlock;
   //m_clusterDisplayLayer   = &clusterDisplayLayer;
   //m_clusterIndicatorLayer = &clusterIndicatorLayer;
   //m_clusterExtentLayer    = &clusterExtentLayer;

   int zeroWidthIndex = camCadDatabase.getDefinedWidthIndex(0.);
   CExtent clusterExtent  = m_segmentCluster.getExtent();
   CSize2d clusterSize    = clusterExtent.getSize();
   double clusterRadius   = sqrt(clusterSize.cx*clusterSize.cx + clusterSize.cy*clusterSize.cy)/2.;

   DataStruct* indicatorExtentData = camCadDatabase.addPolyStruct(pcbBlock,clusterExtentLayer.getLayerIndex(),
                                                               0,false,graphicClassNormal);

   CPoly* indicatorExtentPoly = camCadDatabase.addClosedPoly(indicatorExtentData,zeroWidthIndex);
   indicatorExtentPoly->addVertex(m_clusterOrigin.x + clusterRadius,m_clusterOrigin.y ,1.0);        
   indicatorExtentPoly->addVertex(m_clusterOrigin.x - clusterRadius,m_clusterOrigin.y,1.0);        
   indicatorExtentPoly->addVertex(m_clusterOrigin.x + clusterRadius,m_clusterOrigin.y);  
   camCadDatabase.addAttribute(&(indicatorExtentData->getAttributeMap()),clusterIdKeywordIndex,m_segmentCluster.getId(),errorLog);

   DataStruct* indicatorData = camCadDatabase.addPolyStruct(pcbBlock,clusterIndicatorLayer.getLayerIndex(),
                                                               0,false,graphicClassNormal);
   camCadDatabase.addAttribute(&(indicatorData->getAttributeMap()),clusterIdKeywordIndex,m_segmentCluster.getId(),errorLog);

   for (int quadrant = 0;quadrant < 4;quadrant++)
   {
      CSegmentArray& segmentArray = *(m_segmentQuadrants[quadrant]);

      if (segmentArray.GetCount() > 0)
      {
         double deltaDegrees = 90. / (segmentArray.GetCount() + 2);
         double startDegrees = 90. * quadrant;

         for (int index = 0;index < segmentArray.GetCount();index++)
         {
            CSegment* segment = segmentArray.GetAt(index);

            DataStruct* segmentData = camCadDatabase.addPolyStruct(pcbBlock,clusterDisplayLayer.getLayerIndex(),
                                                                  0,false,graphicClassNormal);
            CPoly* segmentPoly      = segment->addPoly(camCadDatabase,*segmentData,zeroWidthIndex);

            camCadDatabase.addAttribute(&(segmentData->getAttributeMap()),segmentIdKeywordIndex,segment->getId(),errorLog);

            if (segment->getParentPolygon() != NULL)
            {
               camCadDatabase.addAttribute(&(segmentData->getAttributeMap()),polygonIdKeywordIndex,segment->getParentPolygon()->getId(),errorLog);
            }

            double indicatorRadians = degreesToRadians(startDegrees + deltaDegrees*(index + 1));
            CPoint2d perimeterRelativePoint(clusterRadius*cos(indicatorRadians),clusterRadius*sin(indicatorRadians));
            CPoint2d perimeterPoint  = m_clusterOrigin + perimeterRelativePoint;
            
            CPoly* indicatorPoly = camCadDatabase.addOpenPoly(indicatorData,zeroWidthIndex);
            indicatorPoly->addVertex(segment->getOrigin());
            indicatorPoly->addVertex(perimeterPoint); 
         }
      }
   }
}

//void CClusterIndicator::instantiate()
//{
//   if (m_camCadDatabase != NULL &&
//       m_pcbBlock != NULL &&
//       m_clusterDisplayLayer != NULL &&
//       m_clusterIndicatorLayer != NULL &&
//       m_clusterExtentLayer != NULL        )
//   {
//      instantiate(*m_camCadDatabase,m_pcbBlock,*m_clusterDisplayLayer,*m_clusterIndicatorLayer,*m_clusterExtentLayer);
//   }
//}

//_____________________________________________________________________________
CFlashRecognizer::CFlashRecognizer(CCamCadDatabase& camCadDatabase,
                                   CFlashRecognizerParameters& parameters,int layerIndex) :
   m_camCadDatabase(camCadDatabase)
   ,m_parameters(parameters)
   ,m_layerIndex(layerIndex)
   ,m_diagnosticLayerMap(nextPrime2n(20),false)
   //,m_layerTree(camCadDatabase.convertToPageUnits(pageUnitsMils,  1.0),
   //            camCadDatabase.convertToPageUnits(pageUnitsMils,200.0))
{
   m_complexLayer = m_camCadDatabase.getLayer("ComplexApertureLayer");
   CSegmentCluster::resetKeywordIndices();
}

CCamCadData& CFlashRecognizer::getCamCadData()
{
   return getCamCadDatabase().getCamCadData();
}

LayerStruct& CFlashRecognizer::getDefinedComplexLayer()
{
   if (m_complexLayer == NULL)
   {
      m_complexLayer = m_camCadDatabase.getDefinedLayer("ComplexApertureLayer");
   }

   return *m_complexLayer;
}

//CPolygonQfeTree& CFlashRecognizer::getLayerTree(int layerIndex)
//{
//   CPolygonQfeTree* layerTree = NULL;
//
//   if (layerIndex >= 0 && layerIndex < m_layerTrees.GetSize())
//   {
//      layerTree = m_layerTrees.GetAt(layerIndex);
//   }
//
//   if (layerTree = NULL)
//   {
//      layerTree = new CPolygonQfeTree();
//   }
//
//   return *layerTree;
//}

CSegmentPolygon* CFlashRecognizer::addSegmentPolygon()
{
   CSegmentPolygon* segmentPolygon = new CSegmentPolygon(m_segmentPolygons.GetCount(),getCamCadDatabase().getPageUnits());

   m_segmentPolygons.AddTail(segmentPolygon);

   return segmentPolygon;
}

CSegmentPoly* CFlashRecognizer::addSegmentPoly(CPoly& poly)
{
   CSegmentPoly* segmentPoly = new CSegmentPoly(m_segmentPolys.GetCount(),poly);

   m_segmentPolys.AddTail(segmentPoly);

   return segmentPoly;
}

bool CFlashRecognizer::scanData()
{
   bool retval = true;

   //CPolygonQfeTree& m_layerTree = getLayerTree(layerIndex);
   FileStruct* file = m_camCadDatabase.getFirstVisibleFile();

   if (file != NULL)
   {
      m_pcbBlock = file->getBlock();

      if (getComplexLayer() != NULL)
      {
         StopEditing();

         m_camCadDatabase.deleteEntitiesOnLayer(*(getComplexLayer()));
      }

      retval = scanLayer(false);

      if (getComplexLayer() != NULL)
      {
         retval = scanLayer(true) && retval;
      }
   }

   return retval;
}

bool CFlashRecognizer::scanLayer(bool processComplexLayerFlag)
{
   bool retval = true;
   const double minimumWidth = .001;

   CDataStructQfeTree dataTree;
   CDataStructQfeList foundDataList;
   CExtent searchExtent;
   double searchRadius = m_camCadDatabase.convertToPageUnits(pageUnitsMils,.1);  // set search radius to .1 mil

   int layerIndex = (processComplexLayerFlag ? getComplexLayer()->getLayerIndex() : m_layerIndex);

   CString statusString;
   statusString.Format("Scanning Data for layer %d",layerIndex);
   COperationProgress progress;
   progress.updateStatus(statusString,m_pcbBlock->getDataCount());

   for (POSITION blockPos = m_pcbBlock->getHeadDataPosition();blockPos != NULL;)
   {
      progress.incrementProgress();
      DataStruct* data = m_pcbBlock->getNextData(blockPos);

      if (data != NULL && data->getLayerIndex() == layerIndex &&
         (!m_parameters.getSelectedFlag() || data->isMarked()))
      {
         CPolygonQfe* qfeData = NULL;

         if (data->getDataType() == dataTypePoly)
         {
            CPolyList& polyList = *(data->getPolyList());

            for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
            {
               CPoly* poly = polyList.GetNext(polyPos);
               BlockStruct* widthBlock = m_camCadDatabase.getCamCadDoc().getWidthBlock(poly->getWidthIndex());
               bool roundedFlag = (widthBlock->getShape() == apertureRound);
               double width = widthBlock->getSizeA();

               //if (width < minimumWidth)
               //{
               //   width = minimumWidth;
               //}

               if (poly != NULL)
               {
                  CPoly vectorPoly(*poly);
                  vectorPoly.vectorize(getCamCadDatabase().getPageUnits());
                  CSegmentPolygon* parentPolygon = NULL;
                  CSegmentPoly* parentPoly = NULL;

                  if (poly->isClosed())
                  {
                     parentPolygon = addSegmentPolygon();
                     parentPoly    = addSegmentPoly(*poly);
                  }

                  const CPntList& pointList = vectorPoly.getPntList();
                  CPnt* previousPnt = NULL;

                  for (POSITION pointPos = pointList.GetHeadPosition();pointPos != NULL;)
                  {
                     CPnt* pnt = pointList.GetNext(pointPos);
                     CPoint2d point(pnt->x,pnt->y);

                     if (previousPnt != NULL)
                     {
                        if (parentPolygon != NULL)
                        {
                           parentPolygon->addVertex(point);
                        }

                        CLine2d line(CPoint2d(previousPnt->x,previousPnt->y),point);
                        double length = line.getLength();

                        if (length >= m_parameters.getMinimumFeatureSize() &&
                            length <= m_parameters.getMaximumFeatureSize()    )
                        {
                           CSegment* segment = new CSegment(getCamCadDatabase(),line,width,data,roundedFlag,parentPolygon,parentPoly);

                           //CPoint2d origin = segment->getOrigin();
                           CSegmentQfe* segmentQfe = new CSegmentQfe(*segment);
                           m_layerTree.setAt(segmentQfe);
                        }
                     }

                     previousPnt = pnt;
                  }

                  //CPolygon* newPolygon = new CPolygon(*poly,width);
                  //CExtent polygonxtent = newPolygon->getExtent();
                  //CPoint2d origin = polygonxtent.getCenter();
                  //CPolygonQfe* polygonQfe = new CPolygonQfe(origin,newPolygon);
                  //m_layerTree.setAt(polygonQfe);
               }
            }
         }
         else if (data->getDataType() == dataTypeInsert)
         {
            InsertStruct* dataInsert = data->getInsert();
            bool duplicateFlag = false;

            searchExtent.setToSearchRadius(data->getInsert()->getOrigin2d(),searchRadius);
            foundDataList.empty();
            dataTree.search(searchExtent,foundDataList);
            
            if (foundDataList.GetCount() > 0)
            {
               for (POSITION pos = foundDataList.GetHeadPosition();pos != NULL;)
               {
                  CDataStructQfe* dataStructQfe = foundDataList.GetNext(pos);
                  DataStruct& foundData = dataStructQfe->getDataStruct();
                  InsertStruct* foundDataInsert = foundData.getInsert();

                  if (foundDataInsert->getBlockNumber() == dataInsert->getBlockNumber())
                  {
                     duplicateFlag = foundDataInsert->insertGeometryFpeq(*dataInsert);
                  }
               }
            }

            if (!duplicateFlag)
            {
               dataTree.setAt(new CDataStructQfe(dataInsert->getOrigin2d(),data));

               BlockStruct* geometry = m_camCadDatabase.getBlock(data->getInsert()->getBlockNumber());

               if (geometry->isAperture() && geometry->getShape() == apertureComplex)
               {
                  if (!processComplexLayerFlag)
                  {
                     copyComplexApertureDataToComplexLayer(*data);
                  }
               }
               else
               {
                  CSegmentCluster* segmentCluster = new CSegmentCluster(*this,*data,layerIndex);

                  if (segmentCluster->getList().GetCount() > 0)
                  {
                     for (POSITION segmentQfePos = segmentCluster->getList().GetHeadPosition();segmentQfePos != NULL;)
                     {
                        CSegmentQfe* segmentQfe = segmentCluster->getList().GetNext(segmentQfePos);
                        m_layerTree.setAt(segmentQfe);
                     }

                     m_clusterMap.add(*segmentCluster);
                  }
                  else
                  {
                     delete segmentCluster;
                  }
               }
            }
         }
      }
   }

   return retval;
}

void CFlashRecognizer::copyComplexApertureDataToComplexLayer(DataStruct& complexApertureInsert)
{
   if (complexApertureInsert.getDataType() == dataTypeInsert)
   {
      BlockStruct* geometry = m_camCadDatabase.getBlock(complexApertureInsert.getInsert()->getBlockNumber());

      if (geometry->isAperture() && geometry->getShape() == apertureComplex)
      {
         BlockStruct* subBlock = m_camCadDatabase.getBlock(geometry->getComplexApertureSubBlockNumber());

         LayerStruct* layer = m_camCadDatabase.getCamCadDoc().getLayer(complexApertureInsert.getLayerIndex());

         if (layer != NULL && (layer->getLayerIndex() == m_layerIndex || layer->isFloating()))
         {
            bool nonFloatingComplexInsertFlag = (layer->getLayerIndex() == m_layerIndex);
            bool legalComplexFlag = true;

            for (POSITION pos = subBlock->getHeadDataPosition();pos != NULL && legalComplexFlag;)
            {
               DataStruct* subData = subBlock->getNextData(pos);
               LayerStruct* subLayer = m_camCadDatabase.getCamCadDoc().getLayer(subData->getLayerIndex());

               if (subLayer == NULL)
               {
                  legalComplexFlag = false;
                  break;
               }

               legalComplexFlag = (subLayer->getLayerIndex() == m_layerIndex) ||
                                  (nonFloatingComplexInsertFlag && subLayer->isFloating());
            }

            if (legalComplexFlag)
            {
               int complexLayerIndex = getDefinedComplexLayer().getLayerIndex();
               CTMatrix insertMatrix = complexApertureInsert.getInsert()->getTMatrix();

               for (POSITION pos = subBlock->getHeadDataPosition();pos != NULL && legalComplexFlag;)
               {
                  DataStruct* subData = subBlock->getNextData(pos);

                  DataStruct* subDataCopy = getCamCadData().getNewDataStruct(*subData);
                  subDataCopy->setLayerIndex(complexLayerIndex);
                  subDataCopy->transform(insertMatrix);

                  m_pcbBlock->getDataList().AddTail(subDataCopy);
               }
            }
         }
      }
   }
}

bool CFlashRecognizer::clusterSegments()
{
   bool retval = true;

   //CDebugWriteFormat::getWriteFormat().pushHeader(".  ");
   //CDebugWriteFormat::getWriteFormat().writef("CFlashRecognizer::clusterSegments()\n");

   CSegmentQfeList segmentList;
   CExtent extent;
   extent.maximize();
   m_layerTree.search(extent,segmentList);

   CString statusString;
   statusString.Format("Clustering segments for layer %d",m_layerIndex);
   COperationProgress progress;
   progress.updateStatus(statusString,segmentList.GetCount());

   CSegmentQfeList foundList;
   double maxFeatureSize = m_parameters.getMaximumFeatureSize();

   // when using CSegmentQfeTree instead of CSegmentGraduatedQfeTree
   m_layerTree.setMaxElementExtentSize(CSize2d(maxFeatureSize,maxFeatureSize));

   for (POSITION segmentPos = segmentList.GetHeadPosition();segmentPos != NULL;)
   {
      progress.incrementProgress();
      CSegmentQfe* segmentQfe = segmentList.GetNext(segmentPos);

      if (segmentQfe != NULL)
      {
         CSegment& segment = segmentQfe->getSegment();
         extent = segment.getExtent();

         //CDebugWriteFormat::getWriteFormat().writef("Segment loop - segmentQfe->getCluster()=%p\n",segmentQfe->getCluster());
         //segment.dump(CDebugWriteFormat::getWriteFormat());

         foundList.empty();
         m_layerTree.search(extent,foundList);
         int foundCount = foundList.GetCount();

         for (POSITION foundSegmentPos = foundList.GetHeadPosition();foundSegmentPos != NULL;)
         {
            CSegmentQfe* foundSegmentQfe = foundList.GetNext(foundSegmentPos);

            if (foundSegmentQfe == segmentQfe)
            {
               continue;
            }

            if (foundSegmentQfe->getCluster() == segmentQfe->getCluster() && segmentQfe->getCluster() != NULL)
            {
               continue;
            }

            CSegment& foundSegment = foundSegmentQfe->getSegment();

            //CDebugWriteFormat::getWriteFormat().writef("foundSegment loop\n");
            //foundSegment.dump(CDebugWriteFormat::getWriteFormat());

            if (segment.intersects(foundSegment))
            {
               //int foundClusterId = ((foundSegmentQfe->getCluster() != NULL) ? foundSegmentQfe->getCluster()->getId() : -1);

               m_clusterMap.merge(*segmentQfe,*foundSegmentQfe);
               int mapCount = m_clusterMap.getCount();

               //CDebugWriteFormat::getWriteFormat().writef("segment.getId()=%d, foundSegment.getId()=%d, clusterId=%d, foundClusterId=%d\n",
               //   segment.getId(),foundSegment.getId(),
               //   segmentQfe->getCluster()->getId(),foundClusterId);

               //CDebugWriteFormat::getWriteFormat().writef("intersection foundSegmentQfe->getCluster()=%p, clusterCount=%d\n",
               //   foundSegmentQfe->getCluster(),mapCount);

               //foundSegmentQfe->getCluster()->dump(CDebugWriteFormat::getWriteFormat());
            }
            else
            {
               //CDebugWriteFormat::getWriteFormat().writef("no intersections\n");
            }
         }

         if (segmentQfe->getCluster() == NULL)
         {
            m_clusterMap.add(*segmentQfe);
         }
      }
   }

   //CDebugWriteFormat::getWriteFormat().popHeader();

   return retval;
}

void CFlashRecognizer::generateClusterDisplay()
{
   LayerStruct* clusterDisplayLayer   = m_camCadDatabase.getLayer("Cluster Display");
   LayerStruct* clusterIndicatorLayer = m_camCadDatabase.getLayer("Cluster Indicator");
   LayerStruct* clusterExtentLayer    = m_camCadDatabase.getLayer("Cluster Extent");

   CCamCadLayerMask clusterDisplayLayersMask(m_camCadDatabase);

   if (clusterDisplayLayer != NULL)
   {
      clusterDisplayLayersMask.add(clusterDisplayLayer->getLayerIndex());
   }
   else
   {
      clusterDisplayLayer   = m_camCadDatabase.getDefinedLayer("Cluster Display");
   }

   if (clusterIndicatorLayer != NULL)
   {
      clusterDisplayLayersMask.add(clusterIndicatorLayer->getLayerIndex());
   }
   else
   {
      clusterIndicatorLayer = m_camCadDatabase.getDefinedLayer("Cluster Indicator");
   }

   if (clusterExtentLayer != NULL)
   {
      clusterDisplayLayersMask.add(clusterExtentLayer->getLayerIndex());
   }
   else
   {
      clusterExtentLayer = m_camCadDatabase.getDefinedLayer("Cluster Extent");
   }

   StopEditing();

   COperationProgress progress;
   progress.updateStatus("Removing data on cluster display layers");

   m_camCadDatabase.deleteEntitiesOnLayers(clusterDisplayLayersMask,progress);

   int zeroWidthIndex = m_camCadDatabase.getDefinedWidthIndex(0.);
   int segmentIdKeywordIndex = m_camCadDatabase.registerKeyword("SegmentId",valueTypeInteger);
   int polygonIdKeywordIndex = m_camCadDatabase.registerKeyword("PolygonId",valueTypeInteger);
   int clusterIdKeywordIndex = m_camCadDatabase.registerKeyword("ClusterId",valueTypeInteger);

   clusterDisplayLayer->setColor(colorMagenta);
   clusterIndicatorLayer->setColor(colorCyan);
   clusterExtentLayer->setColor(colorBlue);

   CString statusString;
   statusString.Format("Generating cluster display for layer %d",m_layerIndex);
   progress.updateStatus(statusString,m_clusterMap.getCount());

   for (POSITION clusterPos = m_clusterMap.getStartPosition();clusterPos != NULL;)
   {
      progress.incrementProgress();
      CSegmentCluster* segmentCluster;

      m_clusterMap.getNextAssoc(clusterPos,segmentCluster);
                                                                 
      CClusterIndicator clusterIndicator(*segmentCluster);

      for (POSITION segmentQfePos = segmentCluster->getList().GetHeadPosition();segmentQfePos != NULL;)
      {
         CSegmentQfe* segmentQfe = segmentCluster->getList().GetNext(segmentQfePos);
         CSegment& segment       = segmentQfe->getSegment();
         clusterIndicator.addSegment(segment);
      }

      clusterIndicator.instantiate(m_camCadDatabase,m_pcbBlock,
         segmentIdKeywordIndex,polygonIdKeywordIndex,clusterIdKeywordIndex,
         *clusterDisplayLayer,*clusterIndicatorLayer,*clusterExtentLayer);
   }
}

CString CFlashRecognizer::getUniqueLayerName(const CString& layerNamePrefix)
{
   CString layerName,layerNameSuffix;

   for (int index = 2;;index++)
   {
      layerName = layerNamePrefix + layerNameSuffix;

      if (m_camCadDatabase.getCamCadDoc().getLayer(layerName) == NULL)
      {
         break;
      }

      layerNameSuffix.Format(" %d",index);
   }

   return layerName;
}


LayerStruct* CFlashRecognizer::getDiagnosticLayer(const CString& baseLayerName)
{
   LayerStruct* diagnosticLayer = NULL;
   CString diagnosticLayerName = "Diagnostic: " + baseLayerName;

   if (!m_diagnosticLayerMap.Lookup(diagnosticLayerName,diagnosticLayer))
   {
      diagnosticLayer = m_camCadDatabase.getDefinedLayer(diagnosticLayerName);
      m_diagnosticLayerMap.SetAt(diagnosticLayerName,diagnosticLayer);
   }

   return diagnosticLayer;
}

void CFlashRecognizer::addDiagnosticAperture(CArray<int,int>& methodCountArray,DataStruct* apertureInsert)
{
   if (getParameters().getGenerateDiagnosticLayersFlag())
   {
      DataStruct* diagnosticApertureInsert = getCamCadData().getNewDataStruct(*apertureInsert);
      CString flashRecognitionMethodString;

      m_camCadDatabase.getAttributeStringValue(flashRecognitionMethodString,
                              &(diagnosticApertureInsert->getAttributeMap()),
                              CSegmentCluster::getFlashRecognitionMethodKeywordIndex(m_camCadDatabase));

      LayerStruct* diagnosticLayer = getDiagnosticLayer(flashRecognitionMethodString);
      diagnosticApertureInsert->setLayerIndex(diagnosticLayer->getLayerIndex());
      m_pcbBlock->getDataList().AddTail(diagnosticApertureInsert);

      FlashRecognitionMethodTag flashRecognitionMethod = stringToFlashRecognitionMethod(flashRecognitionMethodString);

      int methodCount = 0;

      if (flashRecognitionMethod < methodCountArray.GetSize())
      {
         methodCount = methodCountArray.GetAt(flashRecognitionMethod) + 1;
      }

      methodCountArray.SetAtGrow(flashRecognitionMethod,methodCount);
   }
}

LayerStruct* CFlashRecognizer::convertClustersToApertures()
{
   LayerStruct* destinationlayer = NULL;

   CDataStructQfeTree apertureTree;
   CFilePath logFilePath(GetLogfilePath("FlashRecognizer.txt"));
   //logFilePath.pushLeaf("FlashRecognizer.txt");
   CMessageFilter messageFilter(messageFilterTypeFormat,logFilePath.getPath());

   CArray<int,int> methodCountArray;
   LayerStruct* sourceLayer = m_camCadDatabase.getCamCadDoc().getLayer(m_layerIndex);
   CString destinationLayerName = getUniqueLayerName(sourceLayer->getName() + " - Apertures");
   CString segregatedLayerName  = getUniqueLayerName(sourceLayer->getName() + " - Cluster Apertures");

   LayerStruct* clusterSegregatedLayer = NULL;
   LayerStruct* clusterApertureLayer   = m_camCadDatabase.getDefinedLayer(destinationLayerName);
   destinationlayer = clusterApertureLayer;

   clusterApertureLayer->setColor(colorRed);

   if (m_parameters.getSegregateClusterAperturesFlag())
   {
      clusterSegregatedLayer = m_camCadDatabase.getDefinedLayer(segregatedLayerName);
      clusterSegregatedLayer->setColor(colorYellow);
   }

   int segregatedLayerIndex = ((clusterSegregatedLayer != NULL) ? 
                                    clusterSegregatedLayer->getLayerIndex() : 
                                    clusterApertureLayer->getLayerIndex()     );

   //CCamCadLayerMask clusterApertureLayersMask(m_camCadDatabase);

   //if (clusterApertureLayer != NULL)
   //{
   //   clusterApertureLayersMask.add(clusterApertureLayer->getLayerIndex());
   //}
   //else
   //{
   //   clusterApertureLayer   = m_camCadDatabase.getDefinedLayer("Cluster Apertures");
   //}

   COperationProgress progress;
   //progress.updateStatus("Removing data on cluster Aperture Layer");

   //m_camCadDatabase.deleteEntitiesOnLayers(clusterApertureLayersMask,progress);

   CString statusString;
   statusString.Format("Generating aperture inserts for layer %d - \"%s\"",m_layerIndex,
      sourceLayer->getName());
   progress.updateStatus(statusString,m_clusterMap.getCount());

   double searchRadius = m_camCadDatabase.convertToPageUnits(pageUnitsMils,2.);
   int complexCnt = 0;
   int simpleCnt  = 0;
   bool removeConcentricAperturesFlag = true;

   try
   {
      for (POSITION clusterPos = m_clusterMap.getStartPosition();clusterPos != NULL;)
      {
         progress.incrementProgress();
         CSegmentCluster* segmentCluster;

         m_clusterMap.getNextAssoc(clusterPos,segmentCluster);

         if (segmentCluster->getSegmentCount() < getParameters().getMaxClusterSegments())
         {
            //if (segmentCluster->getId() == 1881)
            //{
            //   int iii = 3;
            //}

            DataStruct* apertureInsert = segmentCluster->makeApertureInsert(m_camCadDatabase,
                                            clusterApertureLayer->getLayerIndex(),segregatedLayerIndex,
                                            *this,false,messageFilter);

            if (apertureInsert != NULL)
            {
               if (removeConcentricAperturesFlag)
               {
                  CPoint2d apertureOrigin(apertureInsert->getInsert()->getOrigin2d());
                  CExtent searchExtent(apertureOrigin,searchRadius);
                  CDataStructQfe* existingApertureQfe = apertureTree.findFirst(searchExtent);
                  double existingApertureArea = 0.;
                  double apertureArea = 0.;

                  if (existingApertureQfe != NULL)
                  {
                     DataStruct* existingAperture = existingApertureQfe->getObject();
                     
                     BlockStruct* existingApertureGeometry = m_camCadDatabase.getBlock(existingAperture->getInsert()->getBlockNumber());
                     BlockStruct* apertureGeometry         = m_camCadDatabase.getBlock(apertureInsert->getInsert()->getBlockNumber());

                     if (existingApertureGeometry->isAperture() && apertureGeometry->isAperture())
                     {
                        if (existingApertureGeometry->isComplexAperture())
                        {
                           existingApertureGeometry = m_camCadDatabase.getBlock(existingApertureGeometry->getComplexApertureSubBlockNumber());
                           complexCnt++;
                        }
                        else
                        {
                           simpleCnt++;
                        }

                        if (apertureGeometry->isComplexAperture())
                        {
                           apertureGeometry = m_camCadDatabase.getBlock(apertureGeometry->getComplexApertureSubBlockNumber());
                           complexCnt++;
                        }
                        else
                        {
                           simpleCnt++;
                        }

                        for (CDataListIterator polygonIterator(*existingApertureGeometry,dataTypePoly);polygonIterator.hasNext();)
                        {
                           DataStruct* polyStruct = polygonIterator.getNext();

                           if (polyStruct->getPolyList()->GetCount() > 0)
                           {
                              CPoly* poly = polyStruct->getPolyList()->GetHead();
                              existingApertureArea = poly->getArea();
                           }
                        }

                        for (CDataListIterator polygonIterator(*apertureGeometry,dataTypePoly);polygonIterator.hasNext();)
                        {
                           DataStruct* polyStruct = polygonIterator.getNext();

                           if (polyStruct->getPolyList()->GetCount() > 0)
                           {
                              CPoly* poly = polyStruct->getPolyList()->GetHead();
                              apertureArea = poly->getArea();
                           }
                        }

                        if (apertureArea > existingApertureArea)
                        {
                           existingApertureQfe->setObject(apertureInsert);
                           delete existingAperture;
                        }
                        else
                        {
                           delete apertureInsert;
                        }
                     }
                  }
                  else
                  {
                     apertureTree.setAt(new CDataStructQfe(apertureOrigin,apertureInsert));
                  }
               }
               else
               {
                  m_pcbBlock->getDataList().AddTail(apertureInsert);
                  addDiagnosticAperture(methodCountArray,apertureInsert);
               }
            }
         }
      }

      if (removeConcentricAperturesFlag)
      {
         CDataStructQfeList apertureList;
         CExtent entireExtent;
         entireExtent.maximize();

         apertureTree.search(entireExtent,apertureList);

         for (POSITION pos = apertureList.GetHeadPosition();pos != NULL;)
         {
            CDataStructQfe* apertureQfe = apertureList.GetNext(pos);

            if (apertureQfe != NULL)
            {
               DataStruct* apertureInsert = apertureQfe->getObject();

               m_pcbBlock->getDataList().AddTail(apertureInsert);
               addDiagnosticAperture(methodCountArray,apertureInsert);
            }
         }
      }
   }
   catch (...)
   {
      messageFilter.formatMessage("Exception caught: CFlashRecognizer::convertClustersToApertures()");
      destinationlayer = NULL;
   }

#ifdef EnablePerformanceReporter
   CPerformanceReporter::getPerformanceReporter().writeReport(
      CDebugWriteFormat::getWriteFormat(),
      getCamCadDatabase().getCamCadDoc());

   CPerformanceReporter::getPerformanceReporter().close();
#endif

   CFilePath reportFilePath(GetLogfilePath("FlashRecognizerReport.txt"));
   //reportFilePath.pushLeaf("FlashRecognizerReport.txt");
   CStdioFileWriteFormat report(reportFilePath.getPath());

   for (int index = 0;;index++)
   {
      if (index > flashRecognitionMethodUndefined)
      {
         break;
      }

      int count = ((index < methodCountArray.GetSize()) ? methodCountArray.GetAt(index) : 0);

      report.writef("%4d %s\n",count,flashRecognitionMethodToFriendlyString((FlashRecognitionMethodTag)index));
   }

   if (messageFilter.getMessageFormatCount() > 0)
   {
      messageFilter.flush();
      formatMessageBox(MB_ICONINFORMATION,"See the logfile \"%s\" for execution details.",logFilePath.getPath());
   }

   return destinationlayer;
}

//_____________________________________________________________________________
CPerformanceReportEntry::CPerformanceReportEntry(DataStruct& aperture,FlashRecognitionMethodTag recognitionMethod,TimeIndex startTime,TimeIndex stopTime) :
   m_aperture(aperture),
   m_recognitionMethod(recognitionMethod),
   m_startTime(startTime),
   m_stopTime(stopTime)
{
}

void CPerformanceReportEntry::writeReport(CWriteFormat& writeFormat,CCEtoODBDoc& camCadDoc)
{
   BlockStruct* apertureBlock = camCadDoc.getBlockAt(m_aperture.getInsert()->getBlockNumber());
   CString apertureBlockName;

   if (apertureBlock != NULL)
   {
      apertureBlockName = apertureBlock->getName();
   }

   CString coordinates;
   coordinates.Format("(%s,%s)",
      fpfmt(m_aperture.getInsert()->getOriginX(),4),
      fpfmt(m_aperture.getInsert()->getOriginY(),4));


   writeFormat.writef(
"%-14s entity=%7d %-16s %-20s %smS\n",
apertureBlockName,
m_aperture.getEntityNumber(),
coordinates,
flashRecognitionMethodToFriendlyString(m_recognitionMethod),
fpfmt(getDurationInMilliseconds(),4));
}

double CPerformanceReportEntry::getDurationInMilliseconds() const
{
   TimeSpan duration = m_stopTime - m_startTime;
   double durationInMilliseconds = timeToMilliseconds(duration);

   return durationInMilliseconds;
}

//_____________________________________________________________________________
CPerformanceReporter* CPerformanceReporter::m_reporter = NULL;

void CPerformanceReporter::addEntry(DataStruct* aperture,FlashRecognitionMethodTag recognitionMethod,TimeIndex startTime,TimeIndex stopTime)
{
   if (aperture != NULL)
   {
      CPerformanceReportEntry* entry = new CPerformanceReportEntry(*aperture,recognitionMethod,startTime,stopTime);

      m_entries.AddTail(entry);
   }
}

CPerformanceReporter& CPerformanceReporter::getPerformanceReporter()
{
   if (m_reporter == NULL)
   {
      m_reporter = new CPerformanceReporter();
   }

   return *m_reporter;
}

void CPerformanceReporter::writeReport(CWriteFormat& writeFormat,CCEtoODBDoc& camCadDoc)
{
   double totalTime = 0.;
   double maxTime = DBL_MIN;
   double minTime = DBL_MAX;

   for (POSITION pos = m_entries.GetHeadPosition();pos != NULL;)
   {
      CPerformanceReportEntry* entry = m_entries.GetNext(pos);

      entry->writeReport(writeFormat,camCadDoc);

      double duration = entry->getDurationInMilliseconds();

      if (duration < minTime)
      {
         minTime = duration;
      }

      if (duration > maxTime)
      {
         maxTime = duration;
      }

      totalTime += duration;
   }

   int count = m_entries.GetCount();

   if (count < 1)
   {
      count = 1;
   }

   writeFormat.writef(
"min=%s, max=%s, count=%d, total=%s, average=%s\n",
fpfmt(minTime,3),
fpfmt(maxTime,3),
count,
fpfmt(totalTime,3),
fpfmt(totalTime/count,3)
);

}

void CPerformanceReporter::close()
{
   delete m_reporter;
   m_reporter = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////

//  This needs reworking into a class of its own

static CMapWordToPtr ProcessedBlockNumberMap;

static POSITION GetFirstPolylistDataPosition(BlockStruct *block)
{
   if (block != NULL)
   {
      POSITION datapos = block->getHeadDataPosition();
      while (datapos != NULL)
      {
         DataStruct *data = block->getNextData(datapos);

         if (data != NULL && data->getDataType() == dataTypePoly)
         {
            return datapos;
         }
      }
   }

   return NULL;
}

// A better overall architecture might be to process all complex apertures and collect
// all blocks involved and then process that block list. THen the converter step here
// is concise and only needs to convert the polys without concern for the inserts of
// other blocks. Wouldn't need camcaddata arg in call anymore.

static void ConvertComplexApertureBlock(ScannerEngine &scanner, CCamCadData *camCadData, BlockStruct *apGeomBlock)
{
   void *trash = NULL; // being in map or not is what matters, mapped value is not used

   CArray<int,int> insertedBlockNums;

   if (apGeomBlock != NULL && !ProcessedBlockNumberMap.Lookup(apGeomBlock->getBlockNumber(), trash))
   {
      scanner.ScannerInit();

      int polyDataCount = 0;
      int nonPolyDataCount = 0;

      POSITION datapos = apGeomBlock->getHeadDataPosition();
      while (datapos != NULL)
      {
         DataStruct *data = apGeomBlock->getNextData(datapos);

         if (data != NULL && data->getDataType() == dataTypePoly)
         {
            scanner.AddPolyListData( data );
            polyDataCount++;
         }
         else
         {
            // Can't be recursive at this point, scanner engine is shared and gets reset
            // upon entry above. So make list of inserted blocks, when completely done
            // with scanner in this pass, then loop on each block in the list.
            nonPolyDataCount++;
            if (data->getDataType() == dataTypeInsert)
            {
               int insertedBlkNum = data->getInsert()->getBlockNumber();
               insertedBlockNums.Add(insertedBlkNum);  // okay if dup, dups weeded out by ProcessedBlockNumberMap at top
            }

         }
      }

      if (polyDataCount > 0)
      {
         // To see raw input to scanner appear in ccz data, disable 
         // this call to scanner.
         scanner.Merge();

         if (nonPolyDataCount == 0)
         {
            apGeomBlock->getDataList().RemoveAll();
         }
         else
         {
            // Selective remove
            POSITION polypos = NULL;
            while ((polypos = GetFirstPolylistDataPosition(apGeomBlock)) != NULL)
            {
               apGeomBlock->getDataList().RemoveAt(polypos);
            }
         }

         DataStruct *newData = scanner.GetNewPolyListData();
         if (newData != NULL)
         {
            apGeomBlock->getDataList().AddTail(newData);
         }
      }

      // Tally this block as processed
      ProcessedBlockNumberMap.SetAt(apGeomBlock->getBlockNumber(), NULL);

      // Now we can process the inserted blocks
      for (int indx = 0; indx < insertedBlockNums.GetCount(); indx++)
      {
         BlockStruct *insertedBlk = camCadData->getBlockAt( insertedBlockNums.GetAt(indx) );
         ConvertComplexApertureBlock(scanner, camCadData, insertedBlk);
      }

   }
}

void CCEtoODBDoc::OnConvertComplexApertureNonZeroWidthPolys()
{
   CComplexAperturePolyConverter converter(this->getCamCadData());
   converter.ConvertGeometries();
   OnRedraw();
}

//////////////////////////////////////////////////////////////////////////////////////

CComplexAperturePolyConverter::CComplexAperturePolyConverter(CCamCadData &camcadData)
: m_camcadData(camcadData)
{
}

void CComplexAperturePolyConverter::ConvertGeometries()
{
   // Convert complex aperture geometries with non-zero width polys to zero-width outline polys

   ProcessedBlockNumberMap.RemoveAll();

   //CCamCadDatabase camCadDatabase(*this);
   //FileStruct* file = camCadDatabase.getFirstVisibleFile();

   /*
   if (file != NULL)
   {
   //m_pcbBlock = file->getBlock();

   BlockStruct *pcbBlock = file->getBlock();
   if (pcbBlock != NULL)
   {
   }
   }
   */

   ScannerEngine scanner( &m_camcadData );

   for (int i = 0; i < m_camcadData.getMaxBlockIndex(); i++)
   {
      BlockStruct* block = m_camcadData.getBlockAt(i);
      if (block != NULL)
      {
         if (block->isComplexAperture())
         {
            BlockStruct *apGeomBlock = m_camcadData.getBlockAt( block->getComplexApertureSubBlockNumber() );

            ConvertComplexApertureBlock(scanner, &m_camcadData, apGeomBlock);

         }
      }
   }


   ProcessedBlockNumberMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////

// Ought to be a class member, but have .h file conflicts between camcad and scanner lib stuff,
// so some hacks are in place to keep header file usage separated.

ScannerEngine::ScannerEngine(CCamCadData *camCadData)
: m_geomhdr(NULL)
, m_mergedGeomhdr(NULL)
, m_camCadData(camCadData)
{
}

ScannerEngine::~ScannerEngine()
{
   ClearGeom();
}

void ScannerEngine::ClearGeom()
{
	if (m_geomhdr != NULL)
		m_geomhdr = DelPolyList(m_geomhdr);

   if (m_mergedGeomhdr != NULL)
      m_mergedGeomhdr = DelPolyList(m_mergedGeomhdr);
}

void ScannerEngine::ScannerInit()
{
   ClearGeom();
}

void ScannerEngine::GetCounts(CPoly *poly, int &straights, int &arcs)
{
   // Count straight vectors and arcs in poly

   straights = 0;
   arcs = 0;

   CPnt *p1 = NULL;

   POSITION vertexPos = poly->getHeadVertexPosition();
   while (vertexPos != NULL)
   {
      CPnt *p2 = poly->getNextVertex(vertexPos);
      if (p2 != NULL)
      {
         if (p1 != NULL)
         {
            if (p1->bulge != 0.)
               arcs++;
            else
               straights++;
         }

         p1 = p2;
      }
   }
}

void ScannerEngine::AddPoly(CPoly *poly)
{
   // Must be atleast one vector, i.e. at least two points.
   // If we get a one point poly it will be skipped, there's 
   // no line if there is only one point.

   if (!m_camCadData)
      return;

   if (poly != NULL)
   {
      int pntcount = poly->getPntList().GetCount();
      int othercnt = poly->getVertexCount();

      IterPolyGeom crn;

      if (!poly->isHidden())
      {
         double cx, cy, radius;
         if (PolyIsCircle(poly, &cx, &cy, &radius))
         {
            double halfWidth = m_camCadData->getWidth( poly->getWidthIndex() ) * 0.5;

            if (halfWidth > 0.)
            {
               int jj = 0;
            }

            // Do a circle
            int jj = 0;
            GL_DPoint glPt;
            glPt.x      = DcaRound(cx * this->GetConversionFactor());
            glPt.y      = DcaRound(cy * this->GetConversionFactor());
            long glRad  = DcaRound((radius + halfWidth) * this->GetConversionFactor());

            GeomHdr *gh = glCircle::Create(glPt, glRad);
            //gh->flags |= flag;
            gh->SetType(SGN_TYPE_CIRCLE);
            //if (poly->isClosed() && poly->isFilled())
               gh->SetFilledFlag(true);

            crn.SetParent(gh);
            crn.GetFirst();

            gh->next_geom = m_geomhdr;

            //if (m_geomhdr != NULL)
            //   m_geomhdr->next_geom = gh;

             m_geomhdr = gh;
         }
         else if (poly->getPntList().GetCount() > 1)
         {
            // P1 gets set only when dealing with an arc, it will be the start
            // point of the arc, and have non-zero bulge.
            // Note that P2 might be start of an arc also.

            // Haven't any success trying to use gh->SetWidth, the result ends up empty!
            // So just use camcad processing to get outline of poly, this applies the width.
            // Just convert the input poly, we're going to delete it later anyway.
            
            bool closed = poly->isClosed();
            bool filled = poly->isFilled();
            int ptcnt = poly->getVertexCount();

            double width = m_camCadData->getWidth( poly->getWidthIndex() );

#define USE_SYSGL_WIDTH
#ifndef USE_SYSGL_WIDTH
            // If not using width on segments in SysGl then convert poly with width
            // to zero-width outline of poly
            BlockStruct *widthBlock = m_camCadData->getWidthBlock( poly->getWidthIndex() );
            bool convertResult = poly->convertToOutline(width, 0.0, m_camCadData->getZeroWidthIndex(), widthBlock->getShape());
#else
            // Use convert-to-outline for square widths, use SysGL width for rest (rest == round)
            BlockStruct *widthBlock = m_camCadData->getWidthBlock( poly->getWidthIndex() );
            if (widthBlock->getShape() == apertureSquare)
            {
               bool convertResult = poly->convertToOutline(width, 0.0, m_camCadData->getZeroWidthIndex(), widthBlock->getShape());
            }
#endif

            // Reset working width after possible conversion of outline above, since conversion of thick poly
            // to outline usually changes pen width setting.
            width = m_camCadData->getWidth( poly->getWidthIndex() );


            bool ccwRes = poly->makeCcw();
            // This makes bad result:  bool revRes = poly->reverse();

            if (ptcnt != poly->getVertexCount()) // poly changed
            {
               int jj = 0;
               if (poly->isClosed())
                  poly->setFilled(true);
            }

            int straights = 0;
            int arcs = 0;
            this->GetCounts(poly, straights, arcs);

            GeomHdr *gh = glPoly::Create(straights, arcs);
            //gh->flags |= flag;
            gh->SetType(SGN_TYPE_POLY);

				//gh->hdr_flags |= (~(HDR_FLAG_BASE|HDR_FLAG_FREE_MASK)); //???
				//gh->clip_mask = mask;
				//gh->SetWidth(DcaRound(width * this->GetConversionFactor()));
            //long ghwidth = gh->GetWidth();
#ifdef USE_SYSGL_WIDTH
#define REALLY_USE_WIDTH
#ifdef  REALLY_USE_WIDTH
            // the problem with this width is it assumes round pen
            // need to look at camcad poly, sometimes round pen is right
            if (width > 0.)
               gh->SetWidth(DcaRound(width * this->GetConversionFactor()));
#else
            // Use Expand(), but that has to be aplied below, after poly pts are added
#endif
#endif
            if (poly->isClosed() && poly->isFilled())
               gh->SetFilledFlag(true);



            crn.SetParent(gh);
            crn.GetFirst();

            //gh->next_geom = m_geomhdr;
            //m_geomhdr = gh;

            CPnt *firstPt = NULL;

            CPnt *p1 = NULL;

            POSITION vertexPos = poly->getHeadVertexPosition();
            while (vertexPos != NULL)
            {
               CPnt *p2 = poly->getNextVertex(vertexPos);
               if (p2 != NULL)
               {
                  if (firstPt == NULL)
                     firstPt = p2;

                  GL_Point  ct2;
                  ct2.x = DcaRound(p2->x * this->GetConversionFactor());
                  ct2.y = DcaRound(p2->y * this->GetConversionFactor());

                  if (p1 != NULL && p1->bulge != 0.)
                  {
                     // Arc from p1 to p2
                     GL_DPoint ct1; // No idea why these are different types, but that is what AddArcCrnToTail wants

                     double da, sa, r, cx, cy;
                     da = atan(p1->bulge) * 4;
                     ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
                     //double ea = sa + da;

                     ct1.x = DcaRound(cx * this->GetConversionFactor());
                     ct1.y = DcaRound(cy * this->GetConversionFactor());
                     // In scanner lib, sign of radius indicated CW/CCW, in
                     // CAMCAD, sign of bulge indicates same, transfer the cw-ccw-ness.
                     // For both, CCW is positive, CW is negative
                     long radius = DcaRound(r * this->GetConversionFactor()) * ((p1->bulge < 0) ? -1 : 1);

							crn.AddArcCrnToTail(ct1, radius, ct2);
                  }
                  else
                  {
                     // Straight from p1 to p2, p1 is already output
                     crn.AddCrnToTail(ct2);
                  }

                  if (p2->bulge != 0.0)
                     p1 = p2;
                  else
                     p1 = NULL;
               }
            }

            // guarantee perfect closure
            if (poly->isClosed())
            {
               GL_Point  ct2;
               ct2.x = DcaRound(firstPt->x * this->GetConversionFactor());
               ct2.y = DcaRound(firstPt->y * this->GetConversionFactor());
               crn.AddCrnToTail(ct2);
            }

            // Width handled after expand can't be done until after
            // pts are in gh.

            // BUT THIS STILL MAKES ROUND ENDS !
#ifdef USE_SYSGL_WIDTH
#ifndef REALLY_USE_WIDTH
            if (width > 0.)
            {
               long halfWidthSysGlUnits = DcaRound(width * this->GetConversionFactor() * 0.5);
               DFF *df = &AF_Table::dc;
               // kinda works GeomHdr *othergh = df->Expand(&gh, halfWidthSysGlUnits, 0, 0);
               gh = df->Expand(gh, halfWidthSysGlUnits, 0, 0);


               if (gh == NULL)// || othergh == NULL)
               {
                  int jjj = 0;
               }
            }
#endif
#endif

            if (gh != NULL)
            {
               gh->next_geom = m_geomhdr;
               m_geomhdr = gh;
            }
         }
      }
   }
}

void ScannerEngine::AddPolyList(CPolyList *polylist)
{
#ifdef ORIGINAL_WORKS_OKAY
   if (polylist != NULL)
   {
      for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
      {
         CPoly *poly = polylist->GetNext(polyPos);
         AddPoly(poly);
      }
   }
#else
   if (polylist != NULL)
   {
      bool hasSquareWidth = false;

      for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL && !hasSquareWidth;)
      {
         CPoly *poly = polylist->GetNext(polyPos);
         BlockStruct *widthBlock = m_camCadData->getWidthBlock( poly->getWidthIndex() );
            if (widthBlock != NULL && widthBlock->getShape() == apertureSquare)
               hasSquareWidth = true;
      }

      if (hasSquareWidth)
      {
         DataStruct *newPolylistData = NULL;
	      if (m_camCadData != NULL) 
         {
            newPolylistData = m_camCadData->getNewDataStruct(dataTypePoly);
            newPolylistData->setLayerIndex(m_camcadPolyListLayerIndex);

            CPolyList *newPolylist = newPolylistData->getPolyList();

            for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
            {
               CPoly *poly = polylist->GetNext(polyPos);

               if (true)//!poly->isHidden())
               {
                  double cx, cy, radius;
                  bool polyIsCircle = PolyIsCircle(poly, &cx, &cy, &radius);

                  if (polyIsCircle)
                  {
                     int jj = 0;
                  }

                  double width = m_camCadData->getWidth( poly->getWidthIndex() );
                  if (width > 0. && !polyIsCircle)
                  {
                     // Explode poly so polylist is full of single segment lines

                     CPnt *p1 = NULL;
                     POSITION vertexPos = poly->getHeadVertexPosition();
                     while (vertexPos != NULL)
                     {
                        CPnt *p2 = poly->getNextVertex(vertexPos);

                        if (p1 != NULL && p2 != NULL)
                        {
                           CPoly *newPoly = new CPoly();
                           newPoly->addVertex(p1->x, p1->y, p1->bulge);
                           newPoly->addVertex(p2->x, p2->y, p2->bulge);

                           newPoly->setWidthIndex( poly->getWidthIndex() );

                           newPolylist->AddTail(newPoly); 
                        }

                        p1 = p2;
                     }
                  }
                  else
                  {
                     // Either is zero width or is circle, both okay as-is, just copy
                     CPoly *newPoly = new CPoly( *poly );
                     newPolylist->AddTail(newPoly);
                  }
               }
            }

            polylist->empty();
            polylist->takeData(*newPolylist);
            delete newPolylistData;
         }
      }

      // Now ship it to Scanner
      for (POSITION polyPos = polylist->GetHeadPosition();polyPos != NULL;)
      {
         CPoly *poly = polylist->GetNext(polyPos);
         AddPoly(poly);
      }
   }
#endif
}

void ScannerEngine::AddPolyListData(DataStruct *polylistData)
{
   if (polylistData != NULL && polylistData->getDataType() == dataTypePoly)
   {
      m_camcadPolyListLayerIndex = polylistData->getLayerIndex();
      AddPolyList(polylistData->getPolyList());
   }
}

void ScannerEngine::SetOrientation()
{
   // from sample app from Alex

	// correct orientation, so the area is to the left from the polyline direction
	for (GeomHdr *gh = m_geomhdr; gh; gh = gh->next_geom) {
		IterPolyGeom itr(gh);
		if (SGU::GetArea(gh) > 0.0) {
			if (IsDataFlag(gh, GL_BELOW)) {
				itr.Invert();
				gh->SetClockwiseFlag(true);
			} else {
				gh->SetClockwiseFlag(false);
			}
		} else {
			if (IsDataFlag(gh, GL_ABOVE)) {
				itr.Invert();
				gh->SetClockwiseFlag(false);
			} else {
				gh->SetClockwiseFlag(true);
			}
		}
		if(itr.IsClosed()) {
			gh->SetFilledFlag(true);
		}
	}
}

void ScannerEngine::Merge()
{
   //SetOrientation();


	// remove overlaps
   if (m_geomhdr != NULL)
   {
      // Debug - Input polys before merge
      ////F_Poly(m_geomhdr,     "c:\\temp\\aa1.txt", "L");

      DFF *df = &AF_Table::dc;			// processor of algebraic operation
      m_mergedGeomhdr = df->Merge(m_geomhdr, NULL);	// remove overlaps

      // Debug - Input polys after merge, is it still same as aa1 ?
      ////F_Poly(m_geomhdr,     "c:\\temp\\aa2.txt", "L");

      // Debug - Result of merge
      ////F_Poly(m_mergedGeomhdr,     "c:\\temp\\bb.txt", "L");

      DelPolyList(m_geomhdr); // delete original polylines
      m_geomhdr = NULL;
   }
}

CPoly *ScannerEngine::GetNewPoly(const GeomHdr *gh)
{
   CPoly *newPoly = new CPoly();

	if (gh) {
		GL_Point cent;
		long r;
		if(gh->GetGeomType() == SGN_TYPE_POLY){
			IterPolyGeom geom(gh);
			if (geom.HasArcs()) 
         {
				GL_Point beg, end;
				///glLow lc;
				///GL_Box box;
				geom.GetPoint(&beg);
				bool arcstat = geom.GetArc(&cent, &r);

            while (geom.GetNext()) 
            {
					end = geom.GetPoint();
               Point2 begPt((double)beg.x / this->GetConversionFactor(), (double)beg.y / this->GetConversionFactor());
               Point2 endPt((double)end.x / this->GetConversionFactor(), (double)end.y / this->GetConversionFactor());
               if (r != 0) 
               {
                  double centX = (double)cent.x / this->GetConversionFactor();
                  double centY = (double)cent.y / this->GetConversionFactor();
                  //double radius = (double)r / this->GetConversionFactor();

#ifdef FIRST_TRY
                  // Not sure how to really tell if arc should be clockwise or counter-clockwise, so
                  // get delta angles for both.
                  double rad, cwSA, cwDA, ccwSA, ccwDA;
                  ArcCenter2(begPt.x, begPt.y, endPt.x,endPt.y, centX, centY, &rad, &cwSA,  &cwDA,  TRUE  /*clockwise*/ );
                  ArcCenter2(begPt.x, begPt.y, endPt.x,endPt.y, centX, centY, &rad, &ccwSA, &ccwDA, FALSE /*counter cw*/ );

                  // Punt, take one with smaller delta angle
                  if (abs(cwDA) < abs(ccwDA))
                     begPt.bulge = tan(cwDA/4.);
                  else
                     begPt.bulge = tan(ccwDA/4.);
#else
                  // CCW arc has the radius > 0
                  // CW arc has the radius < 0
                  bool clockwiseFlag = (r < 0);
                  double rad, sa, da;
                  ArcCenter2(begPt.x, begPt.y, endPt.x,endPt.y, centX, centY, &rad, &sa,  &da,  clockwiseFlag);
                  begPt.bulge = tan(da/4.);
#endif
               }

               newPoly->addVertex(begPt);
               ///newPoly->addVertex(endPt);

					beg = end;
					arcstat = geom.GetArc(&cent, &r);
				}

            // Don't forget the last point!
            // Ignore arc possibly set on this one, with no following point there is no place to arc to.
            newPoly->addVertex((double)beg.x / this->GetConversionFactor(), (double)beg.y / this->GetConversionFactor());

			}
			else 
         {
            // there are no arcs in this polyline object
            geom.GetFirst();
            GL_Point pnt;

            geom.GetPoint(&pnt);
            newPoly->addVertex((double)pnt.x / this->GetConversionFactor(), (double)pnt.y / this->GetConversionFactor());

            while (geom.GetNext()) 
            {
               geom.GetPoint(&pnt);
               newPoly->addVertex((double)pnt.x / this->GetConversionFactor(), (double)pnt.y / this->GetConversionFactor());
            }
         }
		} 
      else if(gh->GetGeomType() == SGN_TYPE_CIRCLE)
      {
			IterPolyGeom geom(gh);
			geom.GetCircle(&cent, &r);
			if (r < 0) 
            r = -r;

			///ext_box->Set(cent.x - r, cent.y - r, cent.x + r, cent.y + r);
         double topY = (double)(cent.y + r) / this->GetConversionFactor();
         double botY = (double)(cent.y - r) / this->GetConversionFactor();
         double cczX = (double)(cent.x)     / this->GetConversionFactor();

         newPoly->addVertex(cczX, topY, 1.0);
         newPoly->addVertex(cczX, botY, 1.0);
         newPoly->addVertex(cczX, topY, 0.0);
		} 
      else 
      {
			///ASSERT_WRN (0,"ItemExtents:: type error");
         int jj = 0;
		}

		///if (a_add_width) 
      ///{
		///	ext_box->Expand(gh->GetWidth()/2);
		///}
	}

   return newPoly;
}

DataStruct *ScannerEngine::GetNewPolyListData()
{
   // This is just for convenient debug when Merge is disabled
   if (m_mergedGeomhdr == NULL)
      return GetOriginalPolyListData();

   DataStruct *newPolylistData = NULL;
	if (m_camCadData != NULL && m_mergedGeomhdr != NULL) 
   {
      newPolylistData = m_camCadData->getNewDataStruct(dataTypePoly);
      newPolylistData->setLayerIndex(m_camcadPolyListLayerIndex);
      //data->setFlags(flg);
      //data->setNegative(negative);
      CPolyList *newPolylist = newPolylistData->getPolyList();

		for(GeomHdr *gh = m_mergedGeomhdr; gh != NULL; gh = gh->next_geom) 
      {
         CPoly *newPoly = this->GetNewPoly(gh);
         if (newPoly != NULL)
         {
            // Probably should check data to see that if it is really closed, i.e.
            // if start==end, but for now since we're coercing the input to the merge
            // to be closed and expect nothing in return but closed shapes, we'll just
            // set the flags.
            newPoly->setClosed(true);
            newPoly->setFilled(true);
            newPolylist->AddTail(newPoly);

            // Hack to just get first poly
            ///return newPolylistData;
         }
		}
	}

   return newPolylistData;
}
DataStruct *ScannerEngine::GetOriginalPolyListData()
{
   DataStruct *newPolylistData = NULL;
	if (m_camCadData != NULL && m_geomhdr != NULL) 
   {
      newPolylistData = m_camCadData->getNewDataStruct(dataTypePoly);
      newPolylistData->setLayerIndex(m_camcadPolyListLayerIndex);
      //data->setFlags(flg);
      //data->setNegative(negative);
      CPolyList *newPolylist = newPolylistData->getPolyList();

		for(GeomHdr *gh = m_geomhdr; gh != NULL; gh = gh->next_geom) 
      {
         CPoly *newPoly = this->GetNewPoly(gh);
         if (newPoly != NULL)
         {
            newPolylist->AddTail(newPoly);
         }
		}
	}

   return newPolylistData;
}