// $Header: /CAMCAD/4.6/Debug.cpp 11    10/12/06 9:21p Kurt Van Ness $

#include "Stdafx.h"
#include "Debug.h"
#include "CcDoc.h"
#include "Polygon.h"
#include "Region.h"
#include "CamCadDatabase.h"

int  CDebug::m_nextDebugPolygonId = 0;
int  CDebug::m_nextDebugPolyId    = 0;
int  CDebug::m_nextDebugPointId   = 0;
int  CDebug::m_nextDebugExtentId  = 0;
int  CDebug::m_nextDebugRegionId  = 0;
bool CDebug::m_addEnable          = true;

DataStruct* CDebug::addDebugPolygon(CCEtoODBDoc& camCadDoc,CPolygon& polygon,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   int debugLayerIndex = camCadDoc.getDefinedLayer(layerName).getLayerIndex();
   FileStruct* firstVisibleFileStruct = NULL;

   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   if (firstVisibleFileStruct != NULL)
   {
      CPoly* poly = polygon.makeNewPoly();
      DataStruct* polyStruct = camCadDoc.getCamCadData().getNewDataStruct(dataTypePoly);
      polyStruct->setLayerIndex(debugLayerIndex);
      polyStruct->getPolyList()->AddTail(poly);

      firstVisibleFileStruct->getBlock()->getDataList().AddTail(polyStruct);

      addIdAttribute(camCadDoc,*polyStruct,m_nextDebugPolygonId);

      retval = polyStruct;
   }

   return retval;
}

DataStruct* CDebug::addDebugPolygon(CPolygon& polygon,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   CCEtoODBDoc* camCadDoc = getActiveDocument();

   if (camCadDoc != NULL)
   {
      retval = addDebugPolygon(*camCadDoc,polygon,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugPoly(CCEtoODBDoc& camCadDoc,CPoly& poly,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   int debugLayerIndex = camCadDoc.getDefinedLayer(layerName).getLayerIndex();
   FileStruct* firstVisibleFileStruct = NULL;

   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   if (firstVisibleFileStruct != NULL)
   {
      CPoly* polyCopy = new CPoly(poly);
      DataStruct* polyStruct = camCadDoc.getCamCadData().getNewDataStruct(dataTypePoly);
      polyStruct->setLayerIndex(debugLayerIndex);
      polyStruct->getPolyList()->AddTail(polyCopy);

      firstVisibleFileStruct->getBlock()->getDataList().AddTail(polyStruct);

      addIdAttribute(camCadDoc,*polyStruct,m_nextDebugPolyId);

      retval = polyStruct;
   }

   return retval;
}

DataStruct* CDebug::addDebugPoly(CPoly& poly,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   CCEtoODBDoc* camCadDoc = getActiveDocument();

   if (camCadDoc != NULL)
   {
      retval = addDebugPoly(*camCadDoc,poly,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugPoint(CCEtoODBDoc& camCadDoc,const CPoint2d& point,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   int debugLayerIndex = camCadDoc.getDefinedLayer(layerName).getLayerIndex();
   FileStruct* firstVisibleFileStruct = NULL;

   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   if (firstVisibleFileStruct != NULL)
   {
      DataStruct* pointData = camCadDoc.getCamCadData().getNewDataStruct(dataTypePoint);
      pointData->getPoint()->setOrigin(point);
      pointData->setLayerIndex(debugLayerIndex);

      firstVisibleFileStruct->getBlock()->getDataList().AddTail(pointData);

      if (m_nextDebugPointId == 18998)
      {
         int iii = 3;
      }

      addIdAttribute(camCadDoc,*pointData,m_nextDebugPointId);

      retval = pointData;
   }

   return retval;
}

DataStruct* CDebug::addDebugPoint(const CPoint2d& point,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   CCEtoODBDoc* camCadDoc = getActiveDocument();

   if (camCadDoc != NULL)
   {
      retval = addDebugPoint(*camCadDoc,point,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugExtent(CCEtoODBDoc& camCadDoc,const CExtent& extent,double rotationDegrees,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   int debugLayerIndex = camCadDoc.getDefinedLayer(layerName).getLayerIndex();
   FileStruct* firstVisibleFileStruct = NULL;

   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   if (firstVisibleFileStruct != NULL)
   {
      CPoly* poly = new CPoly(extent);

      CTMatrix matrix;
      matrix.translate(-extent.getCenter());
      matrix.rotateDegrees(rotationDegrees);
      matrix.translate( extent.getCenter());

      poly->transform(matrix);

      DataStruct* polyStruct = camCadDoc.getCamCadData().getNewDataStruct(dataTypePoly);
      polyStruct->setLayerIndex(debugLayerIndex);
      polyStruct->getPolyList()->AddTail(poly);

      firstVisibleFileStruct->getBlock()->getDataList().AddTail(polyStruct);

      addIdAttribute(camCadDoc,*polyStruct,m_nextDebugExtentId);

      retval = polyStruct;
   }

   return retval;
}

DataStruct* CDebug::addDebugExtent(const CExtent& extent,double rotationDegrees,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   CCEtoODBDoc* camCadDoc = getActiveDocument();

   if (camCadDoc != NULL)
   {
      retval = addDebugExtent(*camCadDoc,extent,rotationDegrees,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugRegion(CCEtoODBDoc& camCadDoc,const CExtendedRgn& region,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   int debugLayerIndex = camCadDoc.getDefinedLayer(layerName).getLayerIndex();
   FileStruct* firstVisibleFileStruct = NULL;

   for (POSITION filePos = camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(filePos);

      if (fileStruct->isHidden() || !fileStruct->isShown())
      {
         continue;
      }

      firstVisibleFileStruct = fileStruct;

      break;
   }

   if (firstVisibleFileStruct != NULL)
   {
      DataStruct* polyStruct = camCadDoc.getCamCadData().getNewDataStruct(dataTypePoly);
      polyStruct->setLayerIndex(debugLayerIndex);

      firstVisibleFileStruct->getBlock()->getDataList().AddTail(polyStruct);

      addIdAttribute(camCadDoc,*polyStruct,m_nextDebugRegionId);

      retval = polyStruct;

      for (int index = 0;index < region.getRegionDataRectCount();index++)
      {
         const RECT* rect = region.getRegionDataRect(index);

         CPoly* poly = new CPoly();

         poly->addVertex(rect->left ,rect->top   );
         poly->addVertex(rect->left ,rect->bottom);
         poly->addVertex(rect->right,rect->bottom);
         poly->addVertex(rect->right,rect->top   );
         poly->addVertex(rect->left ,rect->top   );

         polyStruct->getPolyList()->AddTail(poly);
      }

      CTMatrix matrix;
      matrix.scaleCtm(isEnglish(camCadDoc.getPageUnits()) ? .001 : 1.);

      polyStruct->transform(matrix);
   }

   return retval;
}

DataStruct* CDebug::addDebugRegion(const CExtendedRgn& region,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   CString layerName;
   layerName.FormatV(format,args);

   CCEtoODBDoc* camCadDoc = getActiveDocument();

   if (camCadDoc != NULL)
   {
      retval = addDebugRegion(*camCadDoc,region,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugAxis(CCamCadDatabase& camCadDatabase,const CBasesVector& basesVector,double unitSize,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   if (m_addEnable)
   {
      CString layerName;
      layerName.FormatV(format,args);

      CTMatrix matrix = basesVector.getTransformationMatrix();
      matrix.scaleCtm(unitSize);

      retval = camCadDatabase.drawAxes(matrix,layerName);
   }

   return retval;
}

DataStruct* CDebug::addDebugAxis(CCamCadData& camCadData,const CBasesVector& basesVector,double unitSize,const char* format,...)
{
   va_list args;
   va_start(args,format);

   DataStruct* retval = NULL;

   if (m_addEnable)
   {
      CString layerName;
      layerName.FormatV(format,args);

      CTMatrix matrix = basesVector.getTransformationMatrix();
      matrix.scaleCtm(unitSize);

      retval = camCadData.drawAxes(matrix,layerName);
   }

   return retval;
}

void CDebug::addIdAttribute(CCEtoODBDoc& camCadDoc,DataStruct& data,int& id)
{
   int idKeywordIndex = camCadDoc.getStandardAttributeKeywordIndex(standardAttributeDebugId);
   camCadDoc.SetAttrib(&(data.getDefinedAttributes()),idKeywordIndex,id);

   id++;
}

void CDebug::writefTimeStampFile(const char* filePath,const char* format,...)
{
   va_list args;
   va_start(args,format);

   CTime dateTime = CTime::GetCurrentTime();

   CString timeStamp,message;

   timeStamp.Format("%s - ",dateTime.Format("%Y%m%d.%H%M%S"));
   message.FormatV(format,args);

   message = timeStamp + message;

   CStdioFile debugFile;

   if (debugFile.Open(filePath,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone))
   {
      debugFile.WriteString(message);
   }
}

void CDebug::writefTimeStamp(const char* format,...)
{
   va_list args;
   va_start(args,format);

   CTime dateTime = CTime::GetCurrentTime();

   CString timeStamp,message;

   timeStamp.Format("%s - ",dateTime.Format("%Y%m%d.%H%M%S"));
   message.FormatV(format,args);

   message = timeStamp + message;

   CStdioFile debugFile;

   if (debugFile.Open("c:\\CDebug.txt",CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone))
   {
      debugFile.SeekToEnd();
      debugFile.WriteString(message);
   }
}

//_____________________________________________________________________________
static CMemoryTracker<SelectStruct>* s_selectStructTracker = NULL;

CMemoryTracker<SelectStruct>& getSelectStructTracker()
{
   if (s_selectStructTracker == NULL)
   {
      s_selectStructTracker = new CMemoryTracker<SelectStruct>(nextPrime2n(1000));
   }

   return *s_selectStructTracker;
}

void releaseSelectStructTracker()
{
   delete s_selectStructTracker;
   s_selectStructTracker = NULL;
}

//_____________________________________________________________________________
static CMemoryTracker<DataStruct>* s_dataStructTracker = NULL;

CMemoryTracker<DataStruct>& getDataStructTracker()
{
   if (s_dataStructTracker == NULL)
   {
      s_dataStructTracker = new CMemoryTracker<DataStruct>(nextPrime2n(1000));
   }

   return *s_dataStructTracker;
}

void releaseDataStructTracker()
{
   delete s_dataStructTracker;
   s_dataStructTracker = NULL;
}

////_____________________________________________________________________________
//CDataStructTracker* CDataStructTracker::m_dataStructTracker = NULL;
//
//CDataStructTracker& CDataStructTracker::getDataStructTracker()
//{
//   if (m_dataStructTracker == NULL)
//   {
//      m_dataStructTracker = new CDataStructTracker();
//   }
//
//   return *m_dataStructTracker;
//}
//
//void CDataStructTracker::releaseDataStructTracker()
//{
//   delete m_dataStructTracker;
//   m_dataStructTracker = NULL;
//}
//
//CDataStructTracker::CDataStructTracker()
//: m_dataStructMap(10,false)
//{
//}
//
//void CDataStructTracker::empty()
//{
//   m_dataStructMap.empty();
//}
//
//int CDataStructTracker::incrementReference(DataStruct* data)
//{
//   int referenceCount = 0;
//
//   if (data != NULL)
//   {
//      m_dataStructMap.Lookup(data,referenceCount);
//
//      referenceCount++;
//
//      m_dataStructMap.SetAt(data,referenceCount);
//   }
//
//   return referenceCount;
//}
//
//int CDataStructTracker::decrementReference(DataStruct* data)
//{
//   int referenceCount = 0;
//
//   if (data != NULL)
//   {
//      m_dataStructMap.Lookup(data,referenceCount);
//
//      referenceCount--;
//
//      if (referenceCount > 0)
//      {
//         m_dataStructMap.SetAt(data,referenceCount);
//      }
//      else
//      {
//         m_dataStructMap.RemoveKey(data);
//      }
//   }
//
//   return referenceCount;
//}
//
//bool CDataStructTracker::contains(DataStruct* data)
//{
//   int referenceCount = 0;
//
//   bool retval = (m_dataStructMap.Lookup(data,referenceCount) != 0);
//
//   return retval;
//}
//
//bool CDataStructTracker::isValid()
//{
//   return true;
//}



