// $Header: /CAMCAD/5.0/GerberEducator.cpp 156   3/22/07 12:54a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "Resource.h"
#include "GerberEducator.h"
#include "GerberEducatorDialog.h"
#include "RwLib.h"
#include "CamCadDatabase.h"
#include "TMState.h"
#include "StandardAperture.h"
#include "CCEtoODB.h"
#include "PcbComponentPinAnalyzer.h"
#include "GerberEducatorReviewGeometries.h"
#include "GerberEducatorUi.h"
#include "GerberEducatorToolbar.h"
#include "Edit.h"

#include <Float.h>

CString gerberEducationMethodTagToString(GerberEducationMethodTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case gerberEducationMethodByShape:       stringValue = QEducationMethodShape;       break;
   case gerberEducationMethodByRefDes:      stringValue = QEducationMethodRefDes;      break;
   case gerberEducationMethodByPartNumber:  stringValue = QEducationMethodPartNumber;  break;
   case gerberEducationMethodByPinPattern:  stringValue = QEducationMethodPinPattern;  break;
   case gerberEducationMethodAutomatic:     stringValue = QEducationMethodAutomatic;   break;
   default:                                 stringValue = "?";                         break;
   }

   return stringValue;
}

bool isMultipleOfRadians(double angleRadians,double factorRadians)
{
   angleRadians = normalizeRadians(angleRadians);

   const double angleToleranceRadians = .0005;
   double modAngleRadians = fmod(angleRadians,factorRadians);

   bool retval = (fpnear(modAngleRadians,0.           ,angleToleranceRadians) || 
                  fpnear(modAngleRadians,factorRadians,angleToleranceRadians));

   return retval;
}

//_________________________________________________________________________________________________
#define QGerber   "Gerber"
#define QCentroid "Centroid"
#define QDrill    "Drill"
#define QUnknown  "Unknown"

GerberEducatorDataSourceTag stringToGerberEducatorDataSource(const CString& tagValue)
{
   GerberEducatorDataSourceTag value = gerberEducatorDataSourceUndefined;

   if (tagValue.CompareNoCase(QGerber) == 0)
   {
      value = gerberEducatorDataSourceGerber;
   }
   else if (tagValue.CompareNoCase(QCentroid) == 0)
   {
      value = gerberEducatorDataSourceCentroid;
   }
   else if (tagValue.CompareNoCase(QDrill) == 0)
   {
      value = gerberEducatorDataSourceDrill;
   }

   return value;
}

CString gerberEducatorDataSourceToString(GerberEducatorDataSourceTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case gerberEducatorDataSourceGerber:    stringValue = QGerber;    break;
   case gerberEducatorDataSourceCentroid:  stringValue = QCentroid;  break;
   case gerberEducatorDataSourceDrill:     stringValue = QDrill;     break;
   default:                                stringValue = QUnknown;   break;
   }

   return stringValue;
}

//_________________________________________________________________________________________________
#define QPadTop            "Pad - Top"
#define QPadBottom         "Pad - Bottom"
#define QTopPad            "Top Pad"
#define QBottomPad         "Bottom Pad"
#define QSilkscreenTop     "Silkscreen - Top"
#define QSilkscreenBottom  "Silkscreen - Bottom"
#define QTopSilkscreen     "Top Silkscreen"
#define QBottomSilkscreen  "Bottom Silkscreen"
#define QTop               "Top"
#define QBottom            "Bottom"
#define QBoardOutline      "Board Outline"
#define QThru              "Thru"
#define QBlind             "Blind"
#define QBuried            "Buried"

GerberEducatorDataSourceTypeTag stringToGerberEducatorDataSourceType(const CString& tagValue)
{
   GerberEducatorDataSourceTypeTag value = gerberEducatorDataSourceTypeUndefined;

   if (tagValue.CompareNoCase(QPadTop) == 0 ||
       tagValue.CompareNoCase(QTopPad) == 0)
   {
      value = gerberEducatorDataSourceTypePadTop;
   }
   else if (tagValue.CompareNoCase(QPadBottom) == 0 ||
            tagValue.CompareNoCase(QBottomPad) == 0)
   {
      value = gerberEducatorDataSourceTypePadBottom;
   }
   else if (tagValue.CompareNoCase(QSilkscreenTop) == 0 ||
            tagValue.CompareNoCase(QTopSilkscreen) == 0)
   {
      value = gerberEducatorDataSourceTypeSilkscreenTop;
   }
   else if (tagValue.CompareNoCase(QSilkscreenBottom) == 0 ||
            tagValue.CompareNoCase(QBottomSilkscreen) == 0)
   {
      value = gerberEducatorDataSourceTypeSilkscreenBottom;
   }
   else if (tagValue.CompareNoCase(QTop) == 0)
   {
      value = gerberEducatorDataSourceTypeTop;
   }
   else if (tagValue.CompareNoCase(QBottom) == 0)
   {
      value = gerberEducatorDataSourceTypeBottom;
   }
   else if (tagValue.CompareNoCase(QBoardOutline) == 0)
   {
      value = gerberEducatorDataSourceTypeBoardOutline;
   }
   else if (tagValue.CompareNoCase(QThru) == 0)
   {
      value = gerberEducatorDataSourceTypeThru;
   }
   else if (tagValue.CompareNoCase(QBlind) == 0)
   {
      value = gerberEducatorDataSourceTypeBlind;
   }
   else if (tagValue.CompareNoCase(QBuried) == 0)
   {
      value = gerberEducatorDataSourceTypeBuried;
   }

   return value;
}

CString gerberEducatorDataSourceTypeToString(GerberEducatorDataSourceTypeTag tagValue)
{
   CString stringValue;

   switch (tagValue)
   {
   case gerberEducatorDataSourceTypePadTop:            stringValue = QPadTop;            break;
   case gerberEducatorDataSourceTypePadBottom:         stringValue = QPadBottom;         break;
   case gerberEducatorDataSourceTypeSilkscreenTop:     stringValue = QSilkscreenTop;     break;
   case gerberEducatorDataSourceTypeSilkscreenBottom:  stringValue = QSilkscreenBottom;  break;
   case gerberEducatorDataSourceTypeTop:               stringValue = QTop;               break;
   case gerberEducatorDataSourceTypeBottom:            stringValue = QBottom;            break;
   case gerberEducatorDataSourceTypeBoardOutline:      stringValue = QBoardOutline;      break;
   case gerberEducatorDataSourceTypeThru:              stringValue = QThru;              break;
   case gerberEducatorDataSourceTypeBlind:             stringValue = QBlind;             break;
   case gerberEducatorDataSourceTypeBuried:            stringValue = QBuried;            break;
   default:                                            stringValue = QUnknown;           break;
   }

   return stringValue;
}

//_____________________________________________________________________________
CWriteFormat& getDebugWriteFormat()
{
   return CDebugWriteFormat::getWriteFormat();
}

//_____________________________________________________________________________
void CCEtoODBDoc::updateGerberEducator() 
{ 
#if defined(EnableGerberEducator)
   if (m_gerberEducator != NULL) 
   {
      m_gerberEducator->update(); 
   }
#endif  // EnableGerberEducator
}

void CCEtoODBDoc::updateGerberEducator(int originalSelectionListModificationCount) 
{ 
#if defined(EnableGerberEducator)
   if (m_gerberEducator != NULL) 
   {
      if (SelectList.getModificationCount() != originalSelectionListModificationCount)
      {
         m_gerberEducator->update(); 
      }
   }
#endif  // EnableGerberEducator
}

#if defined(EnableGerberEducator)

CGerberEducator& CCEtoODBDoc::getDefinedGerberEducator(EducatorFunctionalityTag educatorFunctionality)
{
   if (m_gerberEducator == NULL)
   {
      StopEditing();
      UnselectAll(true);

      switch (educatorFunctionality)
      {
      case educatorFunctionalityBasic:
         m_gerberEducator = new CGerberCentroidMergeTool(*this);
         break;
      case educatorFunctionalityLegacy:
         m_gerberEducator = new CGerberEducator(*this);
         break;
      case educatorFunctionalityCreateGeometry:
      default:
         m_gerberEducator = new CGerberEducatorGeometryCreator(*this);
         break;
      }

      try
      {
         m_gerberEducator->initialize();
      }
      catch (...)
      {
         formatMessageBox("Caught exception in CCEtoODBDoc::getDefinedGerberEducator()");
      }

      CFilePath filePath(this->GetProjectPath());
      filePath.pushLeaf("GerberEducatorDebug.txt");
      CDebugWriteFormat::setFilePath(filePath.getPath());
   }

   return *m_gerberEducator;
}

void CCEtoODBDoc::terminateGerberEducatorSession()
{
   if (m_gerberEducator != NULL)
   {
      m_gerberEducator->deleteEducatorData(true);

      CDebugWriteFormat::close();
      CDebugWriteFormat::setFilePath("");
   }

   delete m_gerberEducator;
   m_gerberEducator = NULL;

   purgeUnusedWidthsAndBlocks(false);
}

void CCEtoODBDoc::OnTerminateGerberEducatorSession()
{
   terminateGerberEducatorSession();
}

//_____________________________________________________________________________
CPinTreeByPolarCoordinate::CPinTreeByPolarCoordinate(double searchTolerance) :
   CTypedRedBlackTreeContainer<CPolarCoordinate,DataStruct>(CPolarCoordinate(-DBL_MAX,-DBL_MAX),true)
{
   m_searchTolerance = searchTolerance;
}

//bool CPinTreeByPolarCoordinate::findMeshSolution(
//   CTypedPtrListContainer<DataStruct*>& padList,POSITION& padPos,
//   CTypedPtrListContainer<DataStruct*>& pinList,double& deltaAngle)
//{
//}

bool CPinTreeByPolarCoordinate::findMeshSolution(CPinTreeByPolarCoordinate& subTree,double& deltaAngle)
{
   bool retval = false;
   //CPinTreeNodeList padNodeList,pinNodeList;

   //getAscendingNodeList(padList);
   //subTree.getAscendingNodeList(pinList);

   //POSITION padPos = padList.GetHeadPosition();
   //POSITION pinPos = pinList.GetHeadPosition();

   //if (pinPos != NULL && padPos != NULL)
   //{
   //   DataStruct* pin1 = pinList.GetNext(pinPos);

   //   if (pinPos != NULL)
   //   {
   //      DataStruct* pin2 = pinList.GetNext(pinPos);

   //      while (pinPos != NULL)
   //      {

   //      }
   //   }
   //}

   return retval;
}

void CPinTreeByPolarCoordinate::test(CWriteFormat* writeFormat)
{
//  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z
//  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
//
//  a  s  e  a  r  c  h  i  n  g  e  x  a  m  p  l  e
//  1 19  5  1 18  3  8  9 14  7  5 24  1 13 16 12  5
   if (writeFormat == NULL)
   {
      return;
   }

   int testVals[] = {1,19, 5, 1,18, 3, 8, 9,14, 7, 5,24, 1,13,16,12, 5};

   CPinTreeByPolarCoordinate testTree(0.);
   CPolarCoordinate polarCoordinate(0,0);

   for (int ind = 0;ind < sizeof(testVals)/sizeof(int);ind++)
   {
      polarCoordinate.setR(testVals[ind]);
      testTree.setAt(polarCoordinate,NULL);
      bool isValid = testTree.isValid();

      writeFormat->writef(
         "CMapCoordToOb::test() - after inserting (%s,%s) %s -------------------------------------------------\n",
         fpfmt(polarCoordinate.getR()),fpfmt(polarCoordinate.getTheta()),
         isValid ? "" : "*** Invalid ***");

      testTree.trace(writeFormat);
   }
}

//_____________________________________________________________________________
CQfePhysicalPin::CQfePhysicalPin(const CPoint2d& origin,const CString& pinRef,bool topPadFlag,bool bottomPadFlag,bool viaFlag)
: CTypedQfe<int>(origin,NULL)
, m_pinRef(pinRef)
, m_topPadFlag(topPadFlag)
, m_bottomPadFlag(bottomPadFlag)
, m_viaFlag(viaFlag)
{
}

CQfePhysicalPin::~CQfePhysicalPin()
{
}

bool CQfePhysicalPin::isVisible(bool topFlag)
{
   bool retval = (topFlag ? m_topPadFlag : m_bottomPadFlag);

   return retval;
}

//_____________________________________________________________________________
CQfeApertureInsert::CQfeApertureInsert(DataStruct* apertureInsert)
{
   m_apertureInsert = apertureInsert;
//   m_pinTree        = NULL;
}

CQfeApertureInsert::~CQfeApertureInsert()
{
//   delete m_pinTree;
}

CExtent CQfeApertureInsert::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

bool CQfeApertureInsert::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

bool CQfeApertureInsert::isExtentIntersecting(const CExtent& extent)
{
   bool retval = (extent.distanceTo(getOrigin()) == 0.);

   return retval;
}

bool CQfeApertureInsert::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

CString CQfeApertureInsert::getInfoString() const
{
   CString retval;

   retval.Format("CQfeApertureInsert: origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

bool CQfeApertureInsert::isValid() const
{
   bool retval = (AfxIsValidAddress(m_apertureInsert,sizeof(DataStruct),true) != 0);

   return retval;
}

void CQfeApertureInsert::updatePinClaim(CGerberEducatorPinClaim& pinClaim)
{
   if (pinClaim.compare(m_pinClaim) > 0)
   {
      m_pinClaim = pinClaim;
   }
}

//CPinTreeByPolarCoordinate* CQfeApertureInsert::getPinTree(CGerberEducator& gerberEducator,double searchTolerance)
//{
//   if (m_pinTree != NULL)
//   {
//      if (m_pinTree->getSearchTolerance() < searchTolerance)
//      {
//         delete m_pinTree;
//         m_pinTree = NULL;
//      }
//   }
//
//   if (m_pinTree == NULL)
//   {
//      m_pinTree = gerberEducator.calcPinTree(*this,searchTolerance);
//   }
//
//   return m_pinTree;
//}

//_____________________________________________________________________________
//CQfeApertureInsertList::CQfeApertureInsertList(bool isContainer,int nBlockSize) : 
//   CTypedPtrList<CQfeList,CQfeApertureInsert*>(nBlockSize)
//{
//   m_isContainer = isContainer;
//}
//
//CQfeApertureInsertList::~CQfeApertureInsertList()
//{
//   empty();
//}
//
//void CQfeApertureInsertList::empty()
//{
//   if (m_isContainer)
//   {
//      for (POSITION pos = GetHeadPosition();pos != NULL;)
//      {
//         CQfeApertureInsert* qfe = GetNext(pos);
//         delete qfe;
//      }
//   }
//
//   RemoveAll();
//}

//_____________________________________________________________________________
//CQfeApertureInsertTree::CQfeApertureInsertTree(bool isContainer) : 
//   CQfeExtentLimitedContainer(CSize2d(0.,0.),false,isContainer)
//{
//}
//
//CQfeApertureInsertTree::~CQfeApertureInsertTree()
//{
//}

//_____________________________________________________________________________
CGerberEducatorPinClaim::CGerberEducatorPinClaim(CGerberEducatorCentroid* centroid)
{
   m_centroid = centroid;

   m_componentCount        = 0;
   m_pinCount              = 0;
   m_distanceToCentroid    = 0.;
   m_centroidOriginFlag    = false;
   m_radialSymmetryFlag    = false;
   m_xAxisSymmetryFlag     = false;
   m_yAxisSymmetryFlag     = false;
}

CGerberEducatorPinClaim& CGerberEducatorPinClaim::operator=(const CGerberEducatorPinClaim& other)
{
   if (&other != this)
   {
      m_centroid              = other.m_centroid;
      m_componentCount        = other.m_componentCount;
      m_pinCount              = other.m_pinCount;
      m_distanceToCentroid    = other.m_distanceToCentroid;
      m_centroidOriginFlag    = other.m_centroidOriginFlag;
      m_radialSymmetryFlag    = other.m_radialSymmetryFlag;
      m_xAxisSymmetryFlag     = other.m_xAxisSymmetryFlag;
      m_yAxisSymmetryFlag     = other.m_yAxisSymmetryFlag;
   }

   return *this;
}

int CGerberEducatorPinClaim::compare(const CGerberEducatorPinClaim& other) const
{
   int retval = m_componentCount - other.m_componentCount;

   if (retval == 0)
   {
      if (getCentroidOriginFlag() != other.getCentroidOriginFlag())
      {
         retval = (getCentroidOriginFlag() ? 1 : -1);
      }
      else if (getBilateralSymmetryFlag() != other.getBilateralSymmetryFlag())
      {
         retval = (getBilateralSymmetryFlag() ? 1 : -1);
      }
      else if (getRadialSymmetryFlag() != other.getRadialSymmetryFlag())
      {
         retval = (getRadialSymmetryFlag() ? 1 : -1);
      }
      else if (getLateralSymmetryFlag() != other.getLateralSymmetryFlag())
      {
         retval = (getLateralSymmetryFlag() ? 1 : -1);
      }
      else if (getDistanceToCentroid() < other.getDistanceToCentroid())
      {
         retval = 1;
      }
      else if (getDistanceToCentroid() > other.getDistanceToCentroid())
      {
         retval = -1;
      }
      else if (m_componentCount > 1)
      {
         retval = getPinCount() - other.getPinCount();
      }
   }

   if (m_componentCount <= 1)
   {
      if (! getCentroidOriginFlag() && ! getBilateralSymmetryFlag() &&
          ! getLateralSymmetryFlag()&& ! getRadialSymmetryFlag() )
      {
         retval = -1;
      }
   }

   return retval;
}

void CGerberEducatorPinClaim::dump(CWriteFormat& writeFormat,CGerberEducator& gerberEducator)
{
   if (m_centroid != NULL)
   {
      writeFormat.writef("RefDes = '%s'\n",m_centroid->getRefDes(gerberEducator));
   }
   else
   {
      writeFormat.writef("m_centroid = NULL\n");
   }

   writeFormat.writef("m_componentCount = %d\n",m_componentCount);
   writeFormat.writef("m_pinCount = %d\n",m_pinCount);
   writeFormat.writef("m_distanceToCentroid = %f\n",m_distanceToCentroid);
   writeFormat.writef("flags [centroid,xSymmetry,ySymmetry,radialSymmetry = %d%d%d%d\n",
      (int)m_centroidOriginFlag,(int)m_xAxisSymmetryFlag,(int)m_yAxisSymmetryFlag,(int)m_radialSymmetryFlag);
}

//_____________________________________________________________________________
CGerberEducatorRefDes::CGerberEducatorRefDes()
: m_refDesSuffixFormat("%d")
{
   m_suffix = -1;   
}

CGerberEducatorRefDes::CGerberEducatorRefDes(const CString& refDes)
{
   setRefDes(refDes);
}

void CGerberEducatorRefDes::setPrefix(const CString& prefix) 
{ 
   m_prefix = prefix;
   build();
}

void CGerberEducatorRefDes::setSuffix(int suffix) 
{ 
   m_suffix = ((suffix < -1) ? -1 : suffix); 
   build();
}

void CGerberEducatorRefDes::setSuffix(int suffix,const CString& refDesSuffixFormat) 
{ 
   m_refDesSuffixFormat = refDesSuffixFormat;

   m_suffix = ((suffix < -1) ? -1 : suffix); 

   build();
}

void CGerberEducatorRefDes::build()
{
   m_refDes = m_prefix;

   if (m_suffix >= 0)
   {
      m_refDes.AppendFormat(m_refDesSuffixFormat,m_suffix);
   }
}

void CGerberEducatorRefDes::setRefDes(const CString& refDes)
{
   m_refDes = refDes;

   const char* firstPrefixChar = refDes;
   const char* lastPrefixChar = NULL;

   for (const char* p = firstPrefixChar;*p != '\0';p++)
   {
      if (isdigit(*p))
      {
         if (m_suffix < 0)
         {
            m_suffix = 0;
         }
         else
         {
            m_suffix *= 10;
         }

         m_suffix += *p - '0';
      }
      else
      {
         m_suffix = -1;
         lastPrefixChar = p;
      }
   }

   if (lastPrefixChar != NULL)
   {
      m_prefix = refDes.Left(lastPrefixChar - firstPrefixChar + 1);
   }
   else
   {
      m_prefix = refDes;
   }
}

//_____________________________________________________________________________
WORD CGerberEducatorRefDesSuffixMap::m_value = 1;

CGerberEducatorRefDesSuffixMap::CGerberEducatorRefDesSuffixMap(const CString& refDesPrefix)
{
   m_refDes.setPrefix(refDesPrefix);
   m_maxSuffix = -1;
}

void CGerberEducatorRefDesSuffixMap::calcMaxSuffix() const
{
   if (m_maxSuffix == -1)
   {
      if (m_suffixMap.GetCount() > 0)
      {
         int suffix;

         for (POSITION pos = m_suffixMap.GetStartPosition();pos != NULL;)
         {
            m_suffixMap.GetNextAssoc(pos,(void*&)suffix,m_value);

            if (suffix > m_maxSuffix)
            {
               m_maxSuffix = suffix;
            }
         }
      }
   }
}

void CGerberEducatorRefDesSuffixMap::define(int refDesSuffix)
{
   if (refDesSuffix >= 0)
   {
      m_suffixMap.SetAt((void*)refDesSuffix,m_value);

      if (refDesSuffix > m_maxSuffix)
      {
         m_maxSuffix = refDesSuffix;
      }
   }
}

CString CGerberEducatorRefDesSuffixMap::defineCeiling(const CString& refDesSuffixFormat)
{
   calcMaxSuffix();

   m_maxSuffix++;
   m_suffixMap.SetAt((void*)m_maxSuffix,m_value);

   m_refDes.setSuffix(m_maxSuffix,refDesSuffixFormat);

   return m_refDes.getRefDes();
}

// if refDesSuffix is already defined, define the ceiling
CString CGerberEducatorRefDesSuffixMap::defineNew(int refDesSuffix,const CString& refDesSuffixFormat)
{
   if (isDefined(refDesSuffix))
   {
      defineCeiling(refDesSuffixFormat);
   }
   else
   {
      m_refDes.setSuffix(refDesSuffix);
   }

   return m_refDes.getRefDes();
}

CString CGerberEducatorRefDesSuffixMap::queryCeiling() const
{
   calcMaxSuffix();

   m_refDes.setSuffix(m_maxSuffix + 1);

   return m_refDes.getRefDes();
}

CString CGerberEducatorRefDesSuffixMap::queryNextUndefined(int refDesSuffix) const
{
   do
   {
      ++refDesSuffix;
   } while (isDefined(refDesSuffix));

   m_refDes.setSuffix(refDesSuffix);

   return m_refDes.getRefDes();
}

CString CGerberEducatorRefDesSuffixMap::queryPrevUndefined(int refDesSuffix) const
{
   do
   {
      if (refDesSuffix < 0)
      {
         break;
      }

      --refDesSuffix;
   } while (isDefined(refDesSuffix));

   m_refDes.setSuffix(refDesSuffix);

   return m_refDes.getRefDes();
}

bool CGerberEducatorRefDesSuffixMap::undefine(int refDesSuffix)
{
   if (refDesSuffix == m_maxSuffix)
   {
      m_maxSuffix = -1;
   }

   return (m_suffixMap.RemoveKey((void*)refDesSuffix) != 0);
}

bool CGerberEducatorRefDesSuffixMap::isDefined(int refDesSuffix) const
{
   return (m_suffixMap.Lookup((void*)refDesSuffix,m_value) != 0);
}

//_____________________________________________________________________________
CGerberEducatorRefDesMap::CGerberEducatorRefDesMap()
{
}

void CGerberEducatorRefDesMap::define(const CString& refDesString)
{
   CGerberEducatorRefDes refDes(refDesString);
   getDefinedSuffixMap(refDes.getPrefix()).define(refDes.getSuffix());
}

CString CGerberEducatorRefDesMap::defineCeiling(const CString& refDesPrefix,const CString& refDesSuffixFormat)
{
   return getDefinedSuffixMap(refDesPrefix).defineCeiling(refDesSuffixFormat);
}

CString CGerberEducatorRefDesMap::defineNew(const CString& refDesString,const CString& refDesSuffixFormat)
{
   CGerberEducatorRefDes refDes(refDesString);
   return getDefinedSuffixMap(refDes.getPrefix()).defineNew(refDes.getSuffix(),refDesSuffixFormat);
}

CString CGerberEducatorRefDesMap::queryCeiling(const CString& refDesPrefix) const
{
   CString retval;

   CGerberEducatorRefDesSuffixMap* suffixMap = getSuffixMap(refDesPrefix);

   if (suffixMap != NULL)
   {
      retval = suffixMap->queryCeiling();
   }
   else
   {
      retval = refDesPrefix + "0";
   }

   return retval;
}

CString CGerberEducatorRefDesMap::queryNextUndefined(const CString& refDesString) const
{
   CString retval;
   CGerberEducatorRefDes refDes(refDesString);
   CGerberEducatorRefDesSuffixMap* suffixMap = getSuffixMap(refDes.getPrefix());

   if (suffixMap != NULL)
   {
      retval = suffixMap->queryNextUndefined(refDes.getSuffix());
   }
   else
   {
      refDes.setSuffix(0);
      retval = refDes.getRefDes();
   }

   return retval;
}

CString CGerberEducatorRefDesMap::queryPrevUndefined(const CString& refDesString) const
{
   CString retval;
   CGerberEducatorRefDes refDes(refDesString);
   CGerberEducatorRefDesSuffixMap* suffixMap = getSuffixMap(refDes.getPrefix());

   if (suffixMap != NULL)
   {
      retval = suffixMap->queryPrevUndefined(refDes.getSuffix());
   }
   else
   {
      refDes.setSuffix(0);
      retval = refDes.getRefDes();
   }

   return retval;
}

bool CGerberEducatorRefDesMap::undefine(const CString& refDesString)
{
   bool retval = false;
   CGerberEducatorRefDes refDes(refDesString);
   CGerberEducatorRefDesSuffixMap* suffixMap = getSuffixMap(refDes.getPrefix());

   if (suffixMap != NULL)
   {
      retval = suffixMap->undefine(refDes.getSuffix());
   }

   return retval;
}

bool CGerberEducatorRefDesMap::isDefined(const CString& refDesString) const
{
   bool retval = false;
   CGerberEducatorRefDes refDes(refDesString);
   CGerberEducatorRefDesSuffixMap* suffixMap = getSuffixMap(refDes.getPrefix());

   if (suffixMap != NULL)
   {
      retval = suffixMap->isDefined(refDes.getSuffix());
   }

   return retval;
}

CGerberEducatorRefDesSuffixMap* CGerberEducatorRefDesMap::getSuffixMap(const CString& refDesPrefix) const
{
   CGerberEducatorRefDesSuffixMap* suffixMap = NULL;

   m_prefixMap.Lookup(refDesPrefix,suffixMap);

   return suffixMap;
}

CGerberEducatorRefDesSuffixMap& CGerberEducatorRefDesMap::getDefinedSuffixMap(const CString& refDesPrefix)
{
   CGerberEducatorRefDesSuffixMap* suffixMap = NULL;

   m_prefixMap.Lookup(refDesPrefix,suffixMap);

   if (suffixMap == NULL)
   {
      suffixMap = new CGerberEducatorRefDesSuffixMap(refDesPrefix);
      m_prefixMap.SetAt(refDesPrefix,suffixMap);
   }

   return *suffixMap;
}

//_____________________________________________________________________________
CGerberEducatorComponents::CGerberEducatorComponents()
{
}

bool CGerberEducatorComponents::Lookup(const CString& refDes,DataStruct*& component)
{
   return (m_components.Lookup(refDes,component) != 0);
}

void CGerberEducatorComponents::SetAt(const CString& refDes,DataStruct* component)
{
   m_components.SetAt(refDes,component);
   m_refDesMap.define(refDes);
}

bool CGerberEducatorComponents::RemoveKey(const CString& refDes)
{
   bool retval = (m_components.RemoveKey(refDes) != 0);

   m_refDesMap.undefine(refDes);

   return retval;
}

//_____________________________________________________________________________
//CGerberEducatorPin::CGerberEducatorPin(DataStruct* m_apertureInsert) : CQfeApertureInsert(m_apertureInsert)
//{
//}
//
//CGerberEducatorPin::~CGerberEducatorPin()
//{
//}
//
//void CGerberEducatorPin::updatePinClaim(CGerberEducatorPinClaim& pinClaim)
//{
//   if (pinClaim.compare(m_pinClaim) > 0)
//   {
//      m_pinClaim = pinClaim;
//   }
//}

//_____________________________________________________________________________
double CGerberEducatorCentroid::m_searchTolerance = 0.;

CGerberEducatorCentroid::CGerberEducatorCentroid(DataStruct& centroid,bool mirrorFlag)
: m_centroid(centroid)
{
   InsertStruct& centroidInsert = *(m_centroid.getInsert());

   if (centroidInsert.getGraphicMirrored())
   {
      mirrorFlag = !mirrorFlag;
   }

   m_localToPcb.translateCtm(centroidInsert.getOriginX(),centroidInsert.getOriginY());
   m_localToPcb.rotateRadiansCtm(centroidInsert.getAngle());
   m_localToPcb.scaleCtm(mirrorFlag ? -1. : 1.,1.);

   m_localToPcb.inverse(m_pcbToLocal);
}

CString CGerberEducatorCentroid::getRefDes(CGerberEducator& gerberEducator)     
{ 
   return gerberEducator.getRefDes(m_centroid); 
}

CString CGerberEducatorCentroid::getShape(CGerberEducator& gerberEducator)      
{ 
   return gerberEducator.getShape(m_centroid); 
}

CString CGerberEducatorCentroid::getPartNumber(CGerberEducator& gerberEducator) 
{ 
   return gerberEducator.getPartNumber(m_centroid); 
}

CPoint2d CGerberEducatorCentroid::transformPcbToLocal(const CPoint2d& point)
{
   CPoint2d retval(point);
   m_pcbToLocal.transform(retval);

   return retval;
}

CPoint2d CGerberEducatorCentroid::transformLocalToPcb(const CPoint2d& point)
{
   CPoint2d retval(point);
   m_localToPcb.transform(retval);

   return retval;
}

CExtent CGerberEducatorCentroid::getPinExtent(CPoint2d pinOrigin)
{
   CExtent pinExtent(pinOrigin.x - m_searchTolerance,pinOrigin.y - m_searchTolerance,
                     pinOrigin.x + m_searchTolerance,pinOrigin.y + m_searchTolerance);

   return pinExtent;
}

void CGerberEducatorCentroid::addPin(CQfeApertureInsert* pinApertureInsert)
{
   m_pinList.AddTail(pinApertureInsert);
}

void CGerberEducatorCentroid::updatePinMask(CByteArray& pinMask)
{
   int pinIndex = 0;

   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;pinIndex++)
   {
      CQfeApertureInsert* pin = m_pinList.GetNext(pos);

      BYTE val = 0;

      if (pinIndex < pinMask.GetSize())
      {
         val = pinMask.GetAt(pinIndex);
      }

      if (val != PinsClash)
      {
         val = ((pin->getPinClaim().getCentroid() == this) ? PinsMatch : PinsClash);
      }

      pinMask.SetAtGrow(pinIndex,val);
   }
}

void CGerberEducatorCentroid::evaluatePinClaims(CGerberEducator& gerberEducator,int componentCount)
{
   CQfeApertureInsertTree pinTree(false);

   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CQfeApertureInsert* pin = m_pinList.GetNext(pos);

      pinTree.setAt(pin);
   }

   CString refDes = getRefDes(gerberEducator);  // for debug

   for (POSITION pos = m_pinList.GetHeadPosition();pos != NULL;)
   {
      CGerberEducatorPinClaim pinClaim(this);

      pinClaim.setComponentCount(componentCount);
      pinClaim.setPinCount(m_pinList.GetCount());

      CQfeApertureInsert* pin = m_pinList.GetNext(pos);
      CPoint2d pinOrigin = pin->getOrigin();
      CPoint2d localPinOrigin = transformPcbToLocal(pinOrigin);
      CPoint2d centroidOrigin = m_centroid.getInsert()->getOrigin2d();

      double dx = fabs(pinOrigin.x - centroidOrigin.x);
      double dy = fabs(pinOrigin.y - centroidOrigin.y);
      double distanceToCentroid = _hypot(dx,dy);

      pinClaim.setDistanceToCentroid(distanceToCentroid);

      if (fpeq(dx,0.) && fpeq(dy,0.))
      {
         pinClaim.setCentroidOriginFlag();
      }
      else
      {
         CPoint2d targetPinOrigin;
         CExtent pinExtent;

         //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
         targetPinOrigin.x = -localPinOrigin.x;
         targetPinOrigin.y = -localPinOrigin.y;

         targetPinOrigin = transformLocalToPcb(targetPinOrigin);

         pinExtent = getPinExtent(targetPinOrigin);

         if (pinTree.findFirst(pinExtent) != NULL)
         {
            pinClaim.setRadialSymmetryFlag();
         }

         //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
         targetPinOrigin.x = -localPinOrigin.x;
         targetPinOrigin.y =  localPinOrigin.y;

         targetPinOrigin = transformLocalToPcb(targetPinOrigin);

         pinExtent = getPinExtent(targetPinOrigin);

         if (pinTree.findFirst(pinExtent) != NULL)
         {
            pinClaim.setYAxisSymmetryFlag();
         }

         //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
         targetPinOrigin.x =  localPinOrigin.x;
         targetPinOrigin.y = -localPinOrigin.y;

         pinExtent = getPinExtent(targetPinOrigin);

         if (pinTree.findFirst(pinExtent) != NULL)
         {
            pinClaim.setXAxisSymmetryFlag();
         }
      }

      if (pinClaim.compare(pin->getPinClaim()) > 0)
      {
         getDebugWriteFormat().writef(
"____________________________________________________________________________________________\n");
         getDebugWriteFormat().writef(
"CGerberEducatorCentroid::evaluatePinClaims() - updating pin claim (%.5f,%.5f)\n",
            pinOrigin.x,pinOrigin.y);

         getDebugWriteFormat().pushHeader("old: ");
         pin->getPinClaim().dump(getDebugWriteFormat(),gerberEducator);
         getDebugWriteFormat().popHeader();

         getDebugWriteFormat().pushHeader("new: ");
         pinClaim.dump(getDebugWriteFormat(),gerberEducator);
         getDebugWriteFormat().popHeader();
      }

      pin->updatePinClaim(pinClaim);
   }
}

//_____________________________________________________________________________
CGerberEducatorComponentGeometryList::CGerberEducatorComponentGeometryList(CGerberEducatorComponentGeometryDirectory& parent,int pinCount)
: m_geometryList(false)
, m_parent(parent)
, m_pinCount(pinCount)
{
}

CGerberEducator& CGerberEducatorComponentGeometryList::getGerberEducator()
{
   return m_parent.getGerberEducator();
}

void CGerberEducatorComponentGeometryList::add(BlockStruct* geometry)
{
   for (POSITION pos = m_geometryList.GetHeadPosition();;)
   {
      if (pos == NULL)
      {
         m_geometryList.AddTail(geometry);
         break;
      }

      BlockStruct* existingGeometry = m_geometryList.GetNext(pos);

      if (existingGeometry == geometry)
      {
         break;
      }
   }
}

BlockStruct* CGerberEducatorComponentGeometryList::findMatchingComponentGeometry(
   CPinDataTreeList& pinList,DataStruct* centroidData,CBasesVector& insertBasesVector,bool placedTopFlag,bool forceThFlag)
{
   BlockStruct* matchingGeometry = NULL;

   CCamCadDatabase& camCadDatabase = getGerberEducator().getCamCadDatabase();
   const CInsertTypeMask& pinInsertMask = getGerberEducator().getPinInsertTypeMask();
   double pinPitch = 0.,geometryPinAngle = 0.;
   double pinPitchTolerance = getGerberEducator().getPinPitchTolerance();
   bool allPinsFoundFlag = false;
   bool onePinComponentFlag = (pinList.GetCount() == 1);

   // geometry
   for (POSITION pos = m_geometryList.GetHeadPosition();pos != NULL && !allPinsFoundFlag;)
   {
      BlockStruct* geometry = m_geometryList.GetNext(pos);

      DataStruct* pin1 = NULL;
      DataStruct* pin2 = NULL;
      CPoint2d geometryPin1Origin;
      CPoint2d geometryPin2Origin;
      BlockStruct* geometryPin1PadStackGeometry = NULL;
      BlockStruct* geometryPin2PadStackGeometry = NULL;

      // calculate a pin pitch from the first two pins of the component geometry
      for (CDataListIterator pinIterator(*geometry,pinInsertMask);pinIterator.hasNext();)
      {
         DataStruct* pin = pinIterator.getNext();
         InsertStruct* padStackInsert = pin->getInsert();

         if (pin1 == NULL)
         {
            pin1 = pin;
            geometryPin1Origin = padStackInsert->getOrigin2d();
            geometryPin1PadStackGeometry = camCadDatabase.getBlock(padStackInsert->getBlockNumber());

            // for one pin components
            geometryPin2Origin = geometryPin1Origin;
            geometryPin2PadStackGeometry = geometryPin1PadStackGeometry;
         }
         else if (pin2 == NULL)
         {
            pin2 = pin;
            geometryPin2Origin = padStackInsert->getOrigin2d();
            geometryPin2PadStackGeometry = camCadDatabase.getBlock(padStackInsert->getBlockNumber());

            pinPitch   = geometryPin1Origin.distance(geometryPin2Origin);
            CPoint2d pinDelta = geometryPin2Origin - geometryPin1Origin;
            geometryPinAngle = pinDelta.getTheta();

            break;
         }
      }

      double searchTolerance = 2. * CGerberEducator::getSearchTolerance();

      if (pinPitch < searchTolerance && !onePinComponentFlag)
      {
         continue;
      }

      double pin2SearchTolerance = searchTolerance + pinPitch;
      int tryIndex = 0;

      for (int pass = 0;pass < 2 && !allPinsFoundFlag;pass++)
      {
         bool topFlag = (pass == 0);

         // pinList loop
         for (POSITION geometryPinPos = pinList.GetHeadPosition();geometryPinPos != NULL && !allPinsFoundFlag;)
         {
            double fhs;
            double rotationRadians = 0.;

            CPinData* pin1Data = pinList.GetNext(geometryPinPos);
            BlockStruct* pin1PadStackGeometry = getGerberEducator().getPadStackGeometry(*pin1Data,topFlag,forceThFlag,rotationRadians,fhs);

            if (!getGerberEducator().padStackPadsEqual(*pin1PadStackGeometry,*geometryPin1PadStackGeometry))
            {
               continue;
            }

            CPoint2d pin1Origin = pin1Data->getOrigin();
            
            CPinDataQfeList foundList;
            CExtent searchExtent(pin1Origin,pin2SearchTolerance);

            pinList.getPinDataTree().search(searchExtent,foundList);

            // find all possible pin 2
            for (POSITION pinPos = foundList.GetHeadPosition();pinPos != NULL && !allPinsFoundFlag;tryIndex++)
            {
               CPinData* pin2Data = foundList.GetNext(pinPos)->getObject();
               CPoint2d pin2Origin = pin2Data->getOrigin();

               if (fpnear(pin2Origin.distance(pin1Origin),pinPitch,pinPitchTolerance))
               {
                  BlockStruct* pin2PadStackGeometry = getGerberEducator().getPadStackGeometry(*pin2Data,topFlag,forceThFlag,rotationRadians,fhs);

                  if (!getGerberEducator().padStackPadsEqual(*pin2PadStackGeometry,*geometryPin2PadStackGeometry))
                  {
                     continue;
                  }

                  CPoint2d pinDelta = pin2Origin - pin1Origin;
                  double pinAngle   = pinDelta.getTheta();
                  //double deltaAngle = normalizeRadians(pinAngle - geometryPinAngle);

                  CTMatrix componentGeometryToPcbMatrix;
                  componentGeometryToPcbMatrix.translate(-geometryPin1Origin);
                  componentGeometryToPcbMatrix.mirrorAboutYAxis(!topFlag);

                  CPoint2d transformedGeometryPin2Origin = geometryPin2Origin;
                  componentGeometryToPcbMatrix.transform(transformedGeometryPin2Origin);
                  double transformedGeometryPinAngle = transformedGeometryPin2Origin.getTheta();
                  double deltaAngle = normalizeRadians(pinAngle - transformedGeometryPinAngle);

                  componentGeometryToPcbMatrix.rotateRadians(deltaAngle);
                  componentGeometryToPcbMatrix.translate(pin1Origin);

                  // for debug
                  CPoint2d debugPin1Origin = geometryPin1Origin;
                  componentGeometryToPcbMatrix.transform(debugPin1Origin);
                  bool pin1Flag = debugPin1Origin.fpeq(pin1Origin);

                  CPoint2d debugPin2Origin = geometryPin2Origin;
                  componentGeometryToPcbMatrix.transform(debugPin2Origin);
                  bool pin2Flag = debugPin2Origin.fpeq(pin2Origin);

#if defined(_RDEBUG)
                  // if the transformation matrix is correct, then pin1Flag and pin2Flag should both be true
                  if (!pin1Flag || !pin2Flag)
                  {
                     static bool displayedFlag = false;

                     if (!displayedFlag)
                     {
                        displayedFlag = true;
                        formatMessageBox("logic error: CGerberEducatorComponentGeometryList::findMatchingComponentGeometry()");
                     }

                     int iii = 3;
                  }
                  else
                  {
                     int iii = 3;
                  }
#endif

                  if (centroidData != NULL)
                  {
                     int centroidCount = 0;
                     DataStruct* geometryCentroid = m_parent.getGerberEducator().getSingleComponentCentroid(*geometry);

                     if (geometryCentroid == NULL)
                     {
                        continue;
                     }

                     CPoint2d geometryCentroidOrigin = geometryCentroid->getInsert()->getOrigin2d();
                     CPoint2d geometryCentroidOriginPcb = geometryCentroidOrigin;
                     componentGeometryToPcbMatrix.transform(geometryCentroidOriginPcb);
                     CPoint2d centroidOrigin = centroidData->getInsert()->getOrigin2d();

                     if (! centroidOrigin.fpeq(geometryCentroidOriginPcb,searchTolerance))
                     {
                        continue;
                     }
                  }

                  int matchingPinCount = 0;
                  int nonMatchingPinCount = 0;

                  for (CDataListIterator pinIterator(*geometry,pinInsertMask);pinIterator.hasNext() && nonMatchingPinCount == 0;)
                  {
                     DataStruct* matchingPin = pinIterator.getNext();
                     InsertStruct* matchingPinInsert = matchingPin->getInsert();
                     BlockStruct* matchingPinPadStackGeometry = camCadDatabase.getBlock(matchingPinInsert->getBlockNumber());
                     CPoint2d geometryPinOrigin = matchingPinInsert->getOrigin2d();
                     CPoint2d geometryPinOriginPcb = geometryPinOrigin;
                     componentGeometryToPcbMatrix.transform(geometryPinOriginPcb);

                     CPinDataQfeList foundList2;
                     CExtent searchExtent2(geometryPinOriginPcb,searchTolerance);

                     pinList.getPinDataTree().search(searchExtent2,foundList2);

                     if (foundList2.GetCount() == 1)
                     {
                        CPinData* pinData = foundList2.GetHead()->getObject();
                        BlockStruct* pinPadStackGeometry = getGerberEducator().getPadStackGeometry(*pinData,topFlag,forceThFlag,rotationRadians,fhs);

                        if (getGerberEducator().padStacksEqual(*pinPadStackGeometry,*matchingPinPadStackGeometry,deltaAngle))
                        {
                           matchingPinCount++;
                        }
                        else
                        {
                           nonMatchingPinCount++;
                        }
                     }
                     else
                     {
                        nonMatchingPinCount++;
                     }
                  }

                  if (nonMatchingPinCount == 0 && matchingPinCount == m_pinCount)
                  {
                     allPinsFoundFlag = true;
                     matchingGeometry = geometry;

                     insertBasesVector.set();

                     if (!placedTopFlag)
                     {
                        CTMatrix mirrorMatrix;
                        mirrorMatrix.mirrorAboutYAxis();
                        insertBasesVector.transform(mirrorMatrix);
                     }

                     insertBasesVector.transform(componentGeometryToPcbMatrix);

                     if (insertBasesVector.getMirror())
                     {
                        insertBasesVector.set(insertBasesVector.getOrigin().x,insertBasesVector.getOrigin().y,
                           -insertBasesVector.getRotationDegrees(),false,insertBasesVector.getScale());
                     }

                     break;
                  }
               }
            }  // pin 2 loop
         }  // pinList loop
      }  // pass
   }  // geometry

   return matchingGeometry;
}

//_____________________________________________________________________________
CGerberEducatorComponentGeometryDirectory::CGerberEducatorComponentGeometryDirectory(CGerberEducator& gerberEducator)
: m_geometryListMap(nextPrime2n(500))
, m_gerberEducator(gerberEducator)
{
}

void CGerberEducatorComponentGeometryDirectory::add(BlockStruct* geometry)
{
   int pinCount = geometry->getDataList().getInsertCount(CGerberEducator::getPinInsertTypeMask());

   getGeometryList(pinCount).add(geometry);
}

BlockStruct* CGerberEducatorComponentGeometryDirectory::findMatchingComponentGeometry(
   CPinDataTreeList& pinList,DataStruct* centroidData,CBasesVector& insertBasesVector,bool placedTopFlag,bool forceThFlag)
{
   int pinCount = pinList.GetCount();

   return getGeometryList(pinCount).findMatchingComponentGeometry(pinList,centroidData,insertBasesVector,placedTopFlag,forceThFlag);
}

CGerberEducatorComponentGeometryList& CGerberEducatorComponentGeometryDirectory::getGeometryList(int pinCount)
{
   CGerberEducatorComponentGeometryList* geometryList = NULL;
   
   m_geometryListMap.Lookup(pinCount,geometryList);

   if (geometryList == NULL)
   {
      geometryList = new CGerberEducatorComponentGeometryList(*this,pinCount);
      m_geometryListMap.SetAt(pinCount,geometryList);
   }

   return *geometryList;
}

//_____________________________________________________________________________
CGerberEducatorProgressSummary::CGerberEducatorProgressSummary(CGerberEducator& gerberEducator)
: m_gerberEducator(gerberEducator)
{
   initialize();
}

void CGerberEducatorProgressSummary::initialize()
{
   m_topMatchedPadCount           = 0;
   m_bottomMatchedPadCount        = 0;
   m_topUnmatchedPadCount         = 0;
   m_bottomUnmatchedPadCount      = 0;

   m_topMatchedCentroidCount      = 0;
   m_bottomMatchedCentroidCount   = 0;
   m_topUnmatchedCentroidCount    = 0;
   m_bottomUnmatchedCentroidCount = 0;

   m_topComponentCount            = 0;
   m_bottomComponentCount         = 0;

   m_componentGeometryCountMap.RemoveAll();
   m_componentGeometryCountMap.InitHashTable(nextPrime2n(200));
}

void CGerberEducatorProgressSummary::incrementPadCount(bool topFlag,bool matchedFlag)
{
   if (topFlag)
   {
      if (matchedFlag)
      {
         m_topMatchedPadCount++;
      }
      else
      {
         m_topUnmatchedPadCount++;
      }
   }
   else
   {
      if (matchedFlag)
      {
         m_bottomMatchedPadCount++;
      }
      else
      {
         m_bottomUnmatchedPadCount++;
      }
   }
}

void CGerberEducatorProgressSummary::incrementCentroidCount(bool topFlag,bool matchedFlag)
{
   if (topFlag)
   {
      if (matchedFlag)
      {
         m_topMatchedCentroidCount++;
      }
      else
      {
         m_topUnmatchedCentroidCount++;
      }
   }
   else
   {
      if (matchedFlag)
      {
         m_bottomMatchedCentroidCount++;
      }
      else
      {
         m_bottomUnmatchedCentroidCount++;
      }
   }
}

void CGerberEducatorProgressSummary::addComponent(DataStruct& component)
{
   bool topFlag = ! component.getInsert()->getPlacedBottom();

   if (topFlag)
   {
      m_topComponentCount++;
   }
   else
   {
      m_bottomComponentCount++;
   }

   BlockStruct* componentGeometry = m_gerberEducator.getCamCadDatabase().getBlock(component.getInsert()->getBlockNumber());
   CString geometryName = componentGeometry->getName();

   int count = 0;
   m_componentGeometryCountMap.Lookup(geometryName,count);
   m_componentGeometryCountMap.SetAt(geometryName,count + 1);
}

void CGerberEducatorProgressSummary::removeComponent(DataStruct& component)
{
   bool topFlag = ! component.getInsert()->getPlacedBottom();

   if (topFlag)
   {
      m_topComponentCount--;
   }
   else
   {
      m_bottomComponentCount--;
   }

   BlockStruct* componentGeometry = m_gerberEducator.getCamCadDatabase().getBlock(component.getInsert()->getBlockNumber());
   CString geometryName = componentGeometry->getName();

   int count = 0;

   if (m_componentGeometryCountMap.Lookup(geometryName,count))
   {
      if (count > 1)
      {
         m_componentGeometryCountMap.SetAt(geometryName,count - 1);
      }
      else
      {
         m_componentGeometryCountMap.RemoveKey(geometryName);
      }
   }
}

void CGerberEducatorProgressSummary::matchPad(bool topFlag)
{
   if (topFlag)
   {
      m_topMatchedPadCount++;
      m_topUnmatchedPadCount--;
   }
   else
   {
      m_bottomMatchedPadCount++;
      m_bottomUnmatchedPadCount--;
   }
}

void CGerberEducatorProgressSummary::unmatchPad(bool topFlag)
{
   if (topFlag)
   {
      m_topUnmatchedPadCount++;
      m_topMatchedPadCount--;
   }
   else
   {
      m_bottomUnmatchedPadCount++;
      m_bottomMatchedPadCount--;
   }
}

void CGerberEducatorProgressSummary::matchCentroid(bool topFlag)
{
   if (topFlag)
   {
      m_topMatchedCentroidCount++;
      m_topUnmatchedCentroidCount--;
   }
   else
   {
      m_bottomMatchedCentroidCount++;
      m_bottomUnmatchedCentroidCount--;
   }
}

void CGerberEducatorProgressSummary::unmatchCentroid(bool topFlag)
{
   if (topFlag)
   {
      m_topUnmatchedCentroidCount++;
      m_topMatchedCentroidCount--;
   }
   else
   {
      m_bottomUnmatchedCentroidCount++;
      m_bottomMatchedCentroidCount--;
   }
}

int CGerberEducatorProgressSummary::getPadCount(bool topFlag,bool matchedFlag) const
{
   int retval;
   
   if (matchedFlag)
   {
      retval = (topFlag ? m_topMatchedPadCount : m_bottomMatchedPadCount);
   }
   else 
   {
      retval = (topFlag ? m_topUnmatchedPadCount : m_bottomUnmatchedPadCount);
   }

   return retval;
}

int CGerberEducatorProgressSummary::getCentroidCount(bool topFlag,bool matchedFlag) const
{
   int retval;
   
   if (matchedFlag)
   {
      retval = (topFlag ? m_topMatchedCentroidCount : m_bottomMatchedCentroidCount);
   }
   else 
   {
      retval = (topFlag ? m_topUnmatchedCentroidCount : m_bottomUnmatchedCentroidCount);
   }

   return retval;
}

int CGerberEducatorProgressSummary::getComponentCount(bool topFlag) const
{
   int retval = (topFlag ? m_topComponentCount : m_bottomComponentCount);

   return retval;
}

int CGerberEducatorProgressSummary::getComponentGeometryCount() const
{
   return m_componentGeometryCountMap.GetCount();
}

//_____________________________________________________________________________
CGerberEducatorBase::CGerberEducatorBase()
{
   m_dialog           = NULL;
   m_gerberEducatorUi = NULL;
}

CGerberEducatorBase::~CGerberEducatorBase()
{
   if (m_dialog != NULL)
   {
      m_dialog->DestroyWindow();
   }

   delete m_dialog;
}

CGerberEducatorUi* CGerberEducatorBase::getGerberEducatorUi()
{
   return m_gerberEducatorUi;
}

void CGerberEducatorBase::setGerberEducatorUi(CGerberEducatorUi* gerberEducatorUi)
{
   m_gerberEducatorUi = gerberEducatorUi;
}

//_____________________________________________________________________________
double CGerberEducator::m_searchTolerance     = 0.;
double CGerberEducator::m_padShapeTolerance   = 0.;
double CGerberEducator::m_pinPitchTolerance   = 0.;
double CGerberEducator::m_coordinateTolerance = 0.;
CInsertTypeMask CGerberEducator::m_pinInsertTypeMask(insertTypePin,insertTypeMechanicalPin,insertTypeFiducial);

CGerberEducator::CGerberEducator(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_camCadDatabase(camCadDoc)
, m_componentGeometryDirectory(*this)
, m_gerberEducatorProgressSummary(*this)
, m_centroidList(false)
, m_redrawList(false)
, m_eraseList(false)
, m_topPadLayers(false)
, m_bottomPadLayers(false)
, m_topSilkScreenLayers(false)
, m_bottomSilkScreenLayers(false)
, m_topCentroidLayers(false)
, m_bottomCentroidLayers(false)
//, m_topCentroidTree(true)
//, m_bottomCentroidTree(true)
{
   m_pcbFile = NULL;
   m_topUnmatchedLayerIndex          = -1;
   m_bottomUnmatchedLayerIndex       = -1;
   m_topMatchedLayerIndex            = -1;
   m_bottomMatchedLayerIndex         = -1;
   //m_topPadInputLayerIndex           = -1;
   //m_bottomPadInputLayerIndex        = -1;
   //m_topSilkScreenInputLayerIndex    = -1;
   //m_bottomSilkScreenInputLayerIndex = -1;
   //m_topCentroidInputLayerIndex      = -1;
   //m_bottomCentroidInputLayerIndex   = -1;

   m_refDesKeywordIndex          = -1;
   m_partNumberKeywordIndex      = -1;
   m_shapeKeywordIndex           = -1;
   m_machineFeederKeywordIndex   = -1;
   m_machineKeywordIndex         = -1;
   m_feederKeywordIndex          = -1;
   m_educationMethodKeywordIndex = -1;

   m_searchTolerance     = m_camCadDatabase.convertToPageUnits(pageUnitsMils,2.0);
   m_padShapeTolerance   = m_camCadDatabase.convertToPageUnits(pageUnitsMils,0.2);
   m_pinPitchTolerance   = m_camCadDatabase.convertToPageUnits(pageUnitsMils,0.1);
   m_coordinateTolerance = m_camCadDatabase.convertToPageUnits(pageUnitsMils,0.01);
   m_initialized = false;
}

CGerberEducator::~CGerberEducator()
{
}

EducatorFunctionalityTag CGerberEducator::getEducatorFunctionality()
{
   return educatorFunctionalityLegacy;
}

bool CGerberEducator::getEnableCentroidEditingFlag()
{
   return true;
}

bool CGerberEducator::isInitialized() const
{
   return m_initialized;
}

bool CGerberEducator::isTopGerberLayer(LayerStruct& layer)
{
   int gerberEducatorKeywordIndex = m_camCadDatabase.getKeywordIndex(getEducationSurfaceAttributeName());

   CString surface;
   m_camCadDatabase.getAttributeStringValue(surface,getAttributeMap(&layer),gerberEducatorKeywordIndex);

   bool retval = (surface.CompareNoCase("Top") == 0);

   return retval;
}

bool CGerberEducator::isBottomGerberLayer(LayerStruct& layer)
{
   int gerberEducatorKeywordIndex = m_camCadDatabase.getKeywordIndex(getEducationSurfaceAttributeName());

   CString surface;
   m_camCadDatabase.getAttributeStringValue(surface,getAttributeMap(&layer),gerberEducatorKeywordIndex);

   bool retval = (surface.CompareNoCase("Bottom") == 0);

   return retval;
}

bool CGerberEducator::isTopSilkScreenLayer(LayerStruct& layer)
{
   return false;
}

bool CGerberEducator::isBottomSilkScreenLayer(LayerStruct& layer)
{
   return false;
}

bool CGerberEducator::isTopCentroidLayer(LayerStruct& layer)
{
   return false;
}

bool CGerberEducator::isBottomCentroidLayer(LayerStruct& layer)
{
   return false;
}

bool CGerberEducator::scanForGerberEducatorInputLayers(bool displayMessagesFlag)
{
   bool retval = true;

   for (int layerIndex = 0;layerIndex < m_camCadDatabase.getNumLayers();layerIndex++)
   {
      LayerStruct* layer = m_camCadDatabase.getLayerAt(layerIndex);

      if (layer != NULL)
      {
         if (isTopGerberLayer(*layer))
         {
            m_topPadLayers.add(layerIndex);
         }

         if (isBottomGerberLayer(*layer))
         {
            m_bottomPadLayers.add(layerIndex);
         }

         if (isTopSilkScreenLayer(*layer))
         {
            m_topSilkScreenLayers.add(layerIndex);
         }

         if (isBottomSilkScreenLayer(*layer))
         {
            m_bottomSilkScreenLayers.add(layerIndex);
         }

         if (isTopCentroidLayer(*layer))
         {
            m_topCentroidLayers.add(layerIndex);
         }

         if (isBottomCentroidLayer(*layer))
         {
            m_bottomCentroidLayers.add(layerIndex);
         }
      }
   }

   CString errorMessage;

   if (m_topPadLayers.isEmpty())
   {
      errorMessage += "No gerber layers found.";
      retval = false;
   }

   if (displayMessagesFlag && !retval)
   {
      formatMessageBox(MB_ICONEXCLAMATION,errorMessage);
   }

   return retval;
}

void CGerberEducator::defineLayerDataSourceAttribute(LayerStruct& layer)
{
	CString dataSource = QDataSourceGerberEducatorInternal;
	int dataSourceKeywordIndex = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

   m_camCadDatabase.getAttributeStringValue(dataSource,layer.attributes(),dataSourceKeywordIndex);

   if (dataSource.IsEmpty())
   {
      dataSource = QDataSourceGerberEducatorInternal;
      m_camCadDatabase.addAttribute(layer.attributes(),dataSourceKeywordIndex,dataSource);
   }
}

void CGerberEducator::initialize()
{
   m_initialized = false;

   m_refDesKeywordIndex          = m_camCadDatabase.getKeywordIndex(ATT_REFNAME);
   m_partNumberKeywordIndex      = m_camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);
   m_shapeKeywordIndex           = m_camCadDatabase.registerKeyword("Shape"        ,valueTypeString);
   m_machineFeederKeywordIndex   = m_camCadDatabase.registerKeyword("MachineFeeder",valueTypeString);
   m_machineKeywordIndex         = m_camCadDatabase.registerKeyword("Machine"      ,valueTypeString);
   m_feederKeywordIndex          = m_camCadDatabase.registerKeyword("Feeder"       ,valueTypeString);
   m_educationMethodKeywordIndex = m_camCadDatabase.registerKeyword(getEducationMethodAttributeName(),valueTypeString);

   m_pcbFile = m_camCadDatabase.getSingleVisiblePcb();

   if (m_pcbFile != NULL)
   {
      m_initialized = true;
      BlockStruct* pcbBlock = m_pcbFile->getBlock();

      m_gerberEducatorProgressSummary.initialize();

      LayerStruct* padTop           = getLayer(ccLayerPadTop);
      LayerStruct* padBottom        = getLayer(ccLayerPadBottom);
      LayerStruct* pasteTop         = getLayer(ccLayerPasteTop);
      LayerStruct* pasteBottom      = getLayer(ccLayerPasteBottom);
      LayerStruct* assemblyTop      = getLayer(ccLayerAssemblyTop);
      LayerStruct* assemblyBottom   = getLayer(ccLayerAssemblyBottom);
      LayerStruct* centroidTop      = getLayer(ccLayerCentroidTop);
      LayerStruct* centroidBottom   = getLayer(ccLayerCentroidBottom);
      LayerStruct* geCentroidTop    = getLayer(ccLayerEducatorCentroidTop);
      LayerStruct* geCentroidBottom = getLayer(ccLayerEducatorCentroidBottom);
      LayerStruct* matchedTop       = getLayer(ccLayerEducatorMatchedTop);
      LayerStruct* matchedBottom    = getLayer(ccLayerEducatorMatchedBottom);
      LayerStruct* unmatchedTop     = getLayer(ccLayerEducatorUnmatchedTop);
      LayerStruct* unmatchedBottom  = getLayer(ccLayerEducatorUnmatchedBottom);
      LayerStruct* silkTop          = getLayer(ccLayerEducatorSilkTop);
      LayerStruct* silkBottom       = getLayer(ccLayerEducatorSilkBottom);
      LayerStruct* boardOutline     = getLayer(ccLayerEducatorBoardOutline);
      LayerStruct* drillHoles       = getLayer(ccLayerDrillHoles);
      LayerStruct* layerZero        = m_camCadDatabase.getDefinedLayer("0",true);

      // Set electrical stackup for padTop and padBottom. Only these two electrical layers
      // are produced by Gerber Educator, no internal layers, so we can assign them 1 and 2.
      // Leaving these as zero causes problems in other tools, e.g. ODB++ Export, where non-zero
      // electrical stackup is used to identify electrical layers.
      padTop->setElectricalStackNumber(1);
      padBottom->setElectricalStackNumber(2);

      m_camCadDatabase.mirrorLayers(pasteTop->getLayerIndex(),pasteBottom->getLayerIndex());
      m_camCadDatabase.mirrorLayers(centroidTop->getLayerIndex(),centroidBottom->getLayerIndex());

      assemblyTop->setLayerType(LAYTYPE_SILK_TOP);
      assemblyBottom->setLayerType(LAYTYPE_SILK_BOTTOM);

		CString dataSource = QDataSourceGerberEducatorInternal;
		int dataSourceKeywordIndex = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);

      defineLayerDataSourceAttribute(*padTop          );
      defineLayerDataSourceAttribute(*padBottom       );
      defineLayerDataSourceAttribute(*pasteTop        );
      defineLayerDataSourceAttribute(*pasteBottom     );
      defineLayerDataSourceAttribute(*assemblyTop     );
      defineLayerDataSourceAttribute(*assemblyBottom  );
      defineLayerDataSourceAttribute(*centroidTop     );
      defineLayerDataSourceAttribute(*centroidBottom  );
      defineLayerDataSourceAttribute(*geCentroidTop   );
      defineLayerDataSourceAttribute(*geCentroidBottom);
      defineLayerDataSourceAttribute(*matchedTop      );
      defineLayerDataSourceAttribute(*matchedBottom   );
      defineLayerDataSourceAttribute(*unmatchedTop    );
      defineLayerDataSourceAttribute(*unmatchedBottom );
      defineLayerDataSourceAttribute(*silkTop         );
      defineLayerDataSourceAttribute(*silkBottom      );
      defineLayerDataSourceAttribute(*boardOutline    );
      defineLayerDataSourceAttribute(*drillHoles      );
      defineLayerDataSourceAttribute(*layerZero       );

      if (!scanForGerberEducatorInputLayers(true))
      {
         m_initialized = false;
         return;
      }

      m_camCadDatabase.associateExistingLayers();
      //m_camCadDatabase.standardizeLayerNames();

      deleteEducatorData();

      CDataStructQfeTree topCentroidInstanceTree;
      CDataStructQfeTree bottomCentroidInstanceTree;
      CDataList centroidInstancesFromComponents(true);

      CMapStringToWord refDesMap(nextPrime2n(1000));
      CInsertTypeMask componentTypeMask(insertTypePcbComponent,insertTypeFiducial,insertTypeMechanicalComponent,insertTypeDrillTool,insertTypeVia);

      // component loop
      for (CDataListIterator componentIterator(*pcbBlock,componentTypeMask);componentIterator.hasNext();)
      {
         DataStruct* component = componentIterator.getNext();
         BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component->getInsert()->getBlockNumber());
         CTMatrix componentMatrix = component->getInsert()->getTMatrix();

         bool componentPlacedTopFlag = component->getInsert()->getPlacedTop();
         CDataStructQfeTree& centroidInstanceTree = (componentPlacedTopFlag ? topCentroidInstanceTree : bottomCentroidInstanceTree);

         bool viaFlag = (component->getInsert()->getInsertType() == insertTypeVia);

         if (!viaFlag)
         {
            m_gerberEducatorProgressSummary.addComponent(*component);
         }

         for (CDataListIterator pinIterator(*componentGeometry,m_pinInsertTypeMask);pinIterator.hasNext();)
         {
            DataStruct* pin = pinIterator.getNext();
            BlockStruct* padStackGeometry = m_camCadDatabase.getBlock(pin->getInsert()->getBlockNumber());
            bool topPadFlag    = false;
            bool bottomPadFlag = false;

            for (CDataListIterator padIterator(*padStackGeometry,dataTypeInsert);padIterator.hasNext();)
            {
               DataStruct* pad = padIterator.getNext();
               BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

               if (padGeometry->isAperture())
               {
                  if (pad->getLayerIndex() == padTop->getLayerIndex())
                  {
                     if (component->getInsert()->getPlacedBottom())
                     {
                        bottomPadFlag = true;
                     }
                     else
                     {
                        topPadFlag    = true;
                     }
                  }
                  else if (pad->getLayerIndex() == padBottom->getLayerIndex())
                  {
                     if (component->getInsert()->getPlacedBottom())
                     {
                        topPadFlag    = true;
                     }
                     else
                     {
                        bottomPadFlag = true;
                     }
                  }
               }
            }

            CString pinRef(component->getInsert()->getRefname() + "." + pin->getInsert()->getRefname());

            CTMatrix pinMatrix = pin->getInsert()->getTMatrix();

            // use premultiplication
            CTMatrix matrix = pinMatrix * componentMatrix;

            CPoint2d pinOrigin;
            matrix.transform(pinOrigin);
            CQfePhysicalPin* physicalPin = new CQfePhysicalPin(pinOrigin,pinRef,topPadFlag,bottomPadFlag,viaFlag);

            m_physicalPinTree.setAt(physicalPin);
         }

         DataStruct* componentCentroid = getSingleComponentCentroid(*componentGeometry);

         if (componentCentroid != NULL)
         {
            DataStruct* centroidInstance = getCamCadData().getNewDataStruct(*componentCentroid);
            centroidInstancesFromComponents.AddTail(centroidInstance);
            centroidInstance->transform(componentMatrix);

            CDataStructQfe* qfeCentroid = new CDataStructQfe(centroidInstance->getInsert()->getOrigin2d(),centroidInstance);
            centroidInstanceTree.setAt(qfeCentroid);
         }
      }

      saveNamedView();

      int refDesKeywordIndex = m_camCadDatabase.getKeywordIndex(ATT_REFNAME);
      const double searchTolerance = 1.5 * getSearchTolerance();
      CGerberEducatorCentroid::setSearchTolerance(searchTolerance);
      CMapBlockNumberToBlock standardApertureBlockMap(nextPrime2n(1000),false);

      setTopUnmatchedLayerIndex(   getLayer(ccLayerEducatorUnmatchedTop   )->getLayerIndex());
      setTopMatchedLayerIndex(     getLayer(ccLayerEducatorMatchedTop     )->getLayerIndex());
      setBottomUnmatchedLayerIndex(getLayer(ccLayerEducatorUnmatchedBottom)->getLayerIndex());
      setBottomMatchedLayerIndex(  getLayer(ccLayerEducatorMatchedBottom  )->getLayerIndex());

      // centroid, component, aperture loop
      for (int surfaceIndex = 0;surfaceIndex < 2;surfaceIndex++)
      {
         bool topFlag = (surfaceIndex == 0);

         CDataStructQfeTree& centroidInstanceTree  = (topFlag ? topCentroidInstanceTree        : bottomCentroidInstanceTree);
         CDataStructQfeTree& centroidTree          = (topFlag ? m_topCentroidTree              : m_bottomCentroidTree);
         CamCadLayerTag centroidSourceLayer        = (topFlag ? ccLayerCentroidTop             : ccLayerCentroidBottom);
         LayerTypeTag pasteLayerType               = (topFlag ? layerTypePasteTop              : layerTypePasteBottom);
         CamCadLayerTag matchedLayerType           = (topFlag ? ccLayerEducatorMatchedTop      : ccLayerEducatorMatchedBottom);
         CamCadLayerTag unmatchedLayerType         = (topFlag ? ccLayerEducatorUnmatchedTop    : ccLayerEducatorUnmatchedBottom);
         //int gerberLayerIndex                      = (topFlag ? m_topPadInputLayerIndex        : m_bottomPadInputLayerIndex);
         //int centroidLayerIndex                    = (topFlag ? m_topCentroidInputLayerIndex   : m_bottomCentroidInputLayerIndex);
         //int silkScreenLayerIndex                  = (topFlag ? m_topSilkScreenInputLayerIndex : m_bottomSilkScreenInputLayerIndex);
         CLayerFilter& padLayers                   = (topFlag ? m_topPadLayers                 : m_bottomPadLayers);
         //CLayerFilter& silkScreenLayers            = (topFlag ? m_topSilkScreenLayers          : m_bottomSilkScreenPadLayers);
         CLayerFilter& centroidLayers              = (topFlag ? m_topCentroidLayers            : m_bottomCentroidLayers);

         if (padLayers.isEmpty())
         {
            continue;
         }

         int matchedLayerIndex        = getLayer(matchedLayerType   )->getLayerIndex();
         int unmatchedLayerIndex      = getLayer(unmatchedLayerType )->getLayerIndex();


         CDataList newDataList(true);

         for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            InsertStruct* dataInsert = data->getInsert();
            BlockStruct* geometry = m_camCadDoc.getBlockAt(dataInsert->getBlockNumber());

            // --- Centroid --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
            if (geometry->getBlockType() == blockTypeCentroid)
            {
               if (centroidLayers.contains(data->getLayerIndex()))
               {
                  CExtent searchExtent(data->getInsert()->getOrigin2d(),searchTolerance);

                  bool matchFlag = (centroidInstanceTree.findFirst(searchExtent) != NULL);

                  DataStruct* component;

                  DataStruct* centroid = getCamCadData().getNewDataStruct(*data);
                  centroid->setLayerIndex((matchFlag ? matchedLayerIndex : unmatchedLayerIndex));

                  m_gerberEducatorProgressSummary.incrementCentroidCount(topFlag,matchFlag);

                  newDataList.AddTail(centroid);
                  m_centroidList.AddTail(centroid);
                  centroidTree.setAt(new CDataStructQfe(centroid->getInsert()->getOrigin2d(),centroid));

                  CString refDes;

                  m_camCadDatabase.getAttributeStringValue(refDes,getAttributeMap(data),refDesKeywordIndex);

                  if (! refDes.IsEmpty())
                  {
                     m_centroidMap.SetAt(refDes,centroid);

                     if (m_components.Lookup(refDes,component))
                     {
                        m_components.SetAt(refDes,NULL);
                     }
                  }
               }
            }
            // --- Component --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
            else if (geometry->getBlockType() == blockTypePcbComponent  ||
                     geometry->getBlockType() == blockTypeMechComponent ||
                     geometry->getBlockType() == blockTypeFiducial      ||
                     geometry->getBlockType() == blockTypeTooling          )
            {
               m_components.SetAt(dataInsert->getRefname(),data);
               m_componentGeometryDirectory.add(geometry);
            }
            // --- via --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
            else if (geometry->getBlockType() == blockTypePadstack)
            {
            }
            // --- aperture --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- 
            else
            {
               if (! padLayers.contains(data->getLayerIndex()))
               {
                  continue;
               }

               if (! geometry->isAperture())
               {
                  continue;
               }

               CBasesVector apertureBasesVector = geometry->getApertureBasesVector();
               apertureBasesVector.transform(dataInsert->getTMatrix());

               CExtent searchExtent(apertureBasesVector.getOrigin(),searchTolerance);

               CQfePhysicalPinList foundList;
               m_physicalPinTree.search(searchExtent,foundList);
               CQfePhysicalPin* physicalPin = NULL;
               bool viaFlag = false;

               for (POSITION pos = foundList.GetHeadPosition();pos != NULL && physicalPin == NULL;)
               {
                  physicalPin = foundList.GetNext(pos);

                  if (physicalPin->isVisible(topFlag))
                  {
                     viaFlag = physicalPin->isVia();
                     break;
                  }
                  else
                  {
                     physicalPin = NULL;
                  }
               }

               CQfeApertureInsertList apertureInsertFoundList;
               m_apertureInsertTree.search(searchExtent,apertureInsertFoundList);
               CQfeApertureInsert* apertureInsert = NULL;
               bool duplicateFlag = false;

               for (POSITION pos = apertureInsertFoundList.GetHeadPosition();pos != NULL && !duplicateFlag;)
               {
                  apertureInsert = apertureInsertFoundList.GetNext(pos);
                  DataStruct* aperture = apertureInsert->getAperture();

                  if (aperture != NULL)
                  {
                     duplicateFlag = (aperture->getInsert()->getBlockNumber() == geometry->getBlockNumber() &&
                                      aperture->getLayerIndex() == unmatchedLayerIndex                         );
                  }
               }

               if (!duplicateFlag && !viaFlag)
               {
                  CString pinRef;
                  int layerIndex = unmatchedLayerIndex;
                  bool matchFlag = (physicalPin != NULL);

                  m_gerberEducatorProgressSummary.incrementPadCount(topFlag,matchFlag);

                  if (matchFlag)
                  {
                     pinRef = physicalPin->getPinRef();
                     layerIndex = matchedLayerIndex;
                  }

                  BlockStruct* padBlock = NULL;

                  if (geometry->isComplexAperture())
                  {
                     //geometry = m_camCadDatabase.getBlock(geometry->getComplexApertureSubBlockNumber());
                     padBlock = geometry;
                  }
                  else
                  {
                     // convert to standard aperture
                     BlockStruct* standardPadBlock = NULL;

                     if (!standardApertureBlockMap.Lookup(geometry->getBlockNumber(),standardPadBlock))
                     {
                        CStandardAperture standardAperture(geometry,m_camCadDatabase.getPageUnits());
                        standardAperture.normalizeDimensions();
                        standardAperture.setRotationDegrees(0.);

                        CString standardApertureName = "AP_" + standardAperture.getName();

                        standardPadBlock = m_camCadDatabase.getBlock(standardApertureName);

                        if (standardPadBlock == NULL)
                        {
                           standardPadBlock = standardAperture.createNewAperture(getCamCadData(),standardApertureName);

                           if (standardAperture.getArea() == 0.)
                           {
                              standardPadBlock = NULL;
                           }
                        }

                        standardApertureBlockMap.SetAt(geometry->getBlockNumber(),standardPadBlock);
                     }

                     padBlock = standardPadBlock;
                  }

                  if (padBlock != NULL)
                  {
                     DataStruct* pinInsert = m_camCadDatabase.insertBlock(padBlock,insertTypeUnknown,
                                                pinRef,layerIndex,
                                                apertureBasesVector.getOrigin().x,apertureBasesVector.getOrigin().y,
                                                apertureBasesVector.getRotationRadians(),!topFlag);

                     newDataList.AddTail(pinInsert);
                     CQfeApertureInsert* apertureInsert = new CQfeApertureInsert(pinInsert);
                     m_apertureInsertTree.setAt(apertureInsert); 
                  }
               }
            }
         }

         pcbBlock->getDataList().takeData(newDataList);
      }

      // silkscreen
      int educatorTopSilkLayerIndex      = m_camCadDatabase.getLayer(ccLayerEducatorSilkTop     )->getLayerIndex();
      int educatorBottomSilkLayerIndex   = m_camCadDatabase.getLayer(ccLayerEducatorSilkBottom  )->getLayerIndex();
      int educatorBoardOutlineLayerIndex = m_camCadDatabase.getLayer(ccLayerEducatorBoardOutline)->getLayerIndex();
      int educatorLayerIndex;
      int boardOutlineWidthIndex         = m_camCadDatabase.getDefinedWidthIndex(m_camCadDatabase.convertToPageUnits(pageUnitsMils,50.));
      CDataList educatorPolys(true);

      for (CDataListIterator polyIterator(*pcbBlock,dataTypePoly);polyIterator.hasNext();)
      {
         DataStruct* polyStruct = polyIterator.getNext();
         LayerStruct* layer = m_camCadDatabase.getLayerAt(polyStruct->getLayerIndex());
         int widthIndex = -1;

         if (layer != NULL)
         {
            if (m_topSilkScreenLayers.contains(layer->getLayerIndex()))
            {
               educatorLayerIndex = educatorTopSilkLayerIndex;       
            }
            else if (m_bottomSilkScreenLayers.contains(layer->getLayerIndex()))
            {
               educatorLayerIndex = educatorBottomSilkLayerIndex;       
            }
            else if (layer->getLayerType() == layerTypeBoardOutline)
            {
               educatorLayerIndex = educatorBoardOutlineLayerIndex; 
               widthIndex = boardOutlineWidthIndex;
            }
            else
            {
               educatorLayerIndex = -1;                              
            }

            if (educatorLayerIndex >= 0)
            {
               DataStruct* newPolyStruct = getCamCadData().getNewDataStruct(*polyStruct);
               newPolyStruct->setLayerIndex(educatorLayerIndex);

               if (widthIndex >= 0)
               {
                  newPolyStruct->getPolyList()->setWidthIndex(widthIndex);
               }

               educatorPolys.AddTail(newPolyStruct);
            }
         }         
      }

      pcbBlock->getDataList().takeData(educatorPolys);
   }
   else
   {
      formatMessageBox(MB_ICONEXCLAMATION,"One and only one PCB file must exist to run Gerber Educator.");
   }

   //m_apertureInsertTree.assertValid();
}

void CGerberEducator::deleteEducatorData(bool deleteLayersFlag)
{
   CCamCadLayerMask educatorLayersMask(m_camCadDatabase);

   educatorLayersMask.add(ccLayerEducatorMatchedTop);
   educatorLayersMask.add(ccLayerEducatorMatchedBottom);
   educatorLayersMask.add(ccLayerEducatorUnmatchedTop);
   educatorLayersMask.add(ccLayerEducatorUnmatchedBottom);
   educatorLayersMask.add(ccLayerEducatorSilkTop);
   educatorLayersMask.add(ccLayerEducatorSilkBottom);
   educatorLayersMask.add(ccLayerEducatorCentroidTop);
   educatorLayersMask.add(ccLayerEducatorCentroidBottom);
   educatorLayersMask.add(ccLayerEducatorBoardOutline);

   if (deleteLayersFlag)
   {
      m_camCadDatabase.deleteLayers(educatorLayersMask);
   }
   else
   {
      m_camCadDatabase.deleteEntitiesOnLayers(educatorLayersMask);
   }
}

void CGerberEducator::showDialog()
{
   if (isInitialized())
   {
      getDialog().ShowWindow(SW_SHOW);
      getDialog().UpdateWindow();
   }
   else
   {
      formatMessageBox(MB_ICONEXCLAMATION,"The '%s' tool failed to initialize",getToolName());
   }
}

void CGerberEducator::update()
{
   getDialog().update();
}

void CGerberEducator::saveNamedView()
{
   CCEtoODBView* view = getActiveView();

   if (view != NULL && view->GetDocument() == &(m_camCadDatabase.getCamCadDoc()))
   {
      CNamedViewList& namedViewList = m_camCadDatabase.getCamCadDoc().getNamedViewList();

      CString preViewName(QPreGerberEducator);
      namedViewList.deleteAt(preViewName);

      CString viewName(QGerberEducator);
      namedViewList.deleteAt(viewName);

      CNamedView* namedViewPreDft = view->saveView(preViewName);
      CNamedView* namedView       = view->saveView(viewName);

      namedView->hideAll();

      namedView->setAt(getLayer(ccLayerEducatorUnmatchedTop   )->getLayerIndex(),colorRed           ,true );
      namedView->setAt(getLayer(ccLayerEducatorUnmatchedBottom)->getLayerIndex(),colorMagenta       ,true );
      namedView->setAt(getLayer(ccLayerEducatorMatchedTop     )->getLayerIndex(),colorBlue          ,true );
      namedView->setAt(getLayer(ccLayerEducatorMatchedBottom  )->getLayerIndex(),colorCyan          ,true );
      namedView->setAt(getLayer(ccLayerEducatorSilkTop        )->getLayerIndex(),colorHtmlPlum      ,true );
      namedView->setAt(getLayer(ccLayerEducatorSilkBottom     )->getLayerIndex(),colorHtmlDarkViolet,true );
      namedView->setAt(getLayer(ccLayerEducatorBoardOutline   )->getLayerIndex(),colorYellow        ,true );
      namedView->setAt(getLayer(ccLayerDrillHoles             )->getLayerIndex(),colorWhite         ,true );
      

      //view->recallView(*namedView);
   }
}

CGerberEducatorBaseDialog& CGerberEducator::getDialog()
{
   if (m_dialog == NULL)
   {
      m_dialog = new CGerberEducatorDialog(*this);
      m_dialog->setGerberEducatorUi(getGerberEducatorUi());
      m_dialog->Create(IDD_GerberEducatorDialog);
   }

   return *m_dialog;
}

CString CGerberEducator::getRefDes(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_refDesKeywordIndex);

   return retval;
}

CString CGerberEducator::getShape(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_shapeKeywordIndex);

   return retval;
}

CString CGerberEducator::getMachineFeeder(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_machineFeederKeywordIndex);

   return retval;
}

CString CGerberEducator::getMachine(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_machineKeywordIndex);

   return retval;
}

CString CGerberEducator::getFeeder(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_feederKeywordIndex);

   return retval;
}

CString CGerberEducator::getPartNumber(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_partNumberKeywordIndex);

   return retval;
}

CString CGerberEducator::getPackageIdentifier(DataStruct& data,
   GerberEducatorPackageIdentifierTag packageIdentifierTag) 
{ 
   CString packageIdentifier;

   switch (packageIdentifierTag)
   {
   case gerberEducatorPackageIdentifierShape:       packageIdentifier = getShape(data);       break;
   case gerberEducatorPackageIdentifierPartNumber:  packageIdentifier = getPartNumber(data);  break;
   }

   return packageIdentifier; 
}

CString CGerberEducator::getEducationMethod(DataStruct& data)
{
   CString retval;
   m_camCadDatabase.getAttributeStringValue(retval,getAttributeMap(&data),m_educationMethodKeywordIndex);

   return retval;
}

DataStruct* CGerberEducator::getComponent(const CString& refDes)
{
   DataStruct* component = NULL;

   m_components.Lookup(refDes,component);

   return component;
}

bool CGerberEducator::onTopLayer(DataStruct& centroid)
{
   int layerIndex = centroid.getLayerIndex();

   bool retval = (layerIndex == getTopUnmatchedLayerIndex() ||
                  layerIndex == getTopMatchedLayerIndex()     );

   return retval;
}

//#ifdef CGerberEducator_findMatchingComponentGeometry
//BlockStruct* CGerberEducator::findMatchingComponentGeometry(BlockStruct& geometryTemplate,const CString& geometryBaseName)
//{
//   BlockStruct* matchingGeometry = NULL;
//   CString geometryName,suffix;
//   CTypedPtrMap<CMapStringToPtr,CString,InsertStruct*> pinMap;
//   int pinCount = 0;
//
//   for (CDataListIterator pinIterator(geometryTemplate,getPinInsertTypeMask());pinIterator.hasNext();)
//   {
//      DataStruct* pin = pinIterator.getNext();
//      InsertStruct* pinInsert = pin->getInsert();
//
//      pinMap.SetAt(pinInsert->getRefname(),pinInsert);
//      pinCount++;
//   }
//
//   for (int ind=0;;ind++)
//   {
//      suffix.Format("-%d",ind);
//      geometryName  = geometryBaseName + suffix;
//      matchingGeometry = m_camCadDatabase.getBlock(geometryName,m_pcbFile->getFileNumber());
//
//      if (matchingGeometry == NULL)
//      {
//         break;
//      }
//
//      int matchingPinCount = 0;
//      int nonMatchingPinCount = 0;
//
//      for (CDataListIterator pinIterator(*matchingGeometry,getPinInsertTypeMask());pinIterator.hasNext();)
//      {
//         DataStruct* matchingPin = pinIterator.getNext();
//         InsertStruct* matchingPinInsert = matchingPin->getInsert();
//         InsertStruct* pinInsert;
//
//         if (pinMap.Lookup(matchingPinInsert->getRefname(),pinInsert))
//         {
//            if (   fpeq(pinInsert->getOriginX(),matchingPinInsert->getOriginX()) 
//                && fpeq(pinInsert->getOriginY(),matchingPinInsert->getOriginY()) 
//                && matchingPinInsert->getBlockNumber() == pinInsert->getBlockNumber()
//              //&& pinInsert->getInsertType() == matchingPinInsert->getInsertType()
//              //&& fpeq(pinInsert->angle,matchingPinInsert->angle)   // does not consider pad symmetries!!!
//              //&& pinInsert->getGraphicMirrored() == matchingPinInsert->getGraphicMirrored()
//                  )
//            {
//               matchingPinCount++;
//            }
//            else
//            {
//               nonMatchingPinCount++;
//               break;
//            }
//         }
//         else
//         {
//            nonMatchingPinCount++;
//            break;
//         }
//      }
//
//      if (nonMatchingPinCount == 0 && matchingPinCount == pinCount)
//      {
//         break;
//      }
//
//      //suffix.Format("-%d",ind);
//   }
//
//   return matchingGeometry;
//}
//#endif

bool CGerberEducator::aperturesEqual(CCamCadDatabase& camCadDatabase,DataStruct* aperture0,DataStruct* aperture1,double deltaAngleRadians)
{
   bool retval = false;

   if (aperture0 != NULL && aperture1 != NULL)
   {
      if (aperture0->getDataType() == dataTypeInsert && 
          aperture1->getDataType() == dataTypeInsert    )
      {
         if (aperture0->getInsert()->getBlockNumber() == aperture1->getInsert()->getBlockNumber())
         {
            retval = true;
         }
         else
         {
            BlockStruct* apertureBlock0 = camCadDatabase.getBlock(aperture0->getInsert()->getBlockNumber());
            BlockStruct* apertureBlock1 = camCadDatabase.getBlock(aperture1->getInsert()->getBlockNumber());
            double padShapeTolerance = getPadShapeTolerance();
            double deltaAngleDegrees = radiansToDegrees(deltaAngleRadians);
            double apertureDeltaAngleDegrees = aperture1->getInsert()->getAngleDegrees() - aperture0->getInsert()->getAngleDegrees();
            double resultantDeltaAngleRadians = degreesToRadians(deltaAngleDegrees + apertureDeltaAngleDegrees);

            if (apertureBlock0->isAperture() && apertureBlock1->isAperture())
            {
               if (apertureBlock0->getShape() == apertureBlock1->getShape())
               {
                  switch (apertureBlock0->getShape())
                  {
                  case apertureRound:
                  case apertureSquare:
                  case apertureOctagon:
                     retval = fpeq(apertureBlock0->getSizeA(),apertureBlock1->getSizeA());
                     break;
                  case apertureRectangle:
                  case apertureOblong:
                  case apertureDonut:
                     if (isMultipleOfRadians(resultantDeltaAngleRadians,PiOver2) &&
                        !isMultipleOfRadians(resultantDeltaAngleRadians,Pi)         )
                     {
                        retval = (fpnear(apertureBlock0->getSizeA(),apertureBlock1->getSizeB(),padShapeTolerance) &&
                                  fpnear(apertureBlock0->getSizeB(),apertureBlock1->getSizeA(),padShapeTolerance)     );
                     }
                     else
                     {
                        retval = (fpnear(apertureBlock0->getSizeA(),apertureBlock1->getSizeA(),padShapeTolerance) &&
                                  fpnear(apertureBlock0->getSizeB(),apertureBlock1->getSizeB(),padShapeTolerance)     );
                     }

                     break;
                  }
               }
            }
         }
      }
   }

   return retval;
}

BlockStruct* CGerberEducator::getPadStackGeometry(CPinData& pinData,bool topFlag,bool forceThFlag,double& rotationRadians,double& fhs)
{
   CString descriptor,apertureDescriptor;
   BlockStruct* toolBlock = NULL;
   int toolCount = 0;
   CExtent padsExtent;
   fhs = 0.;

   DataStruct* topPad    = (topFlag ? pinData.getTopPad() : pinData.getBottomPad());
   DataStruct* bottomPad = (topFlag ? pinData.getBottomPad() : pinData.getTopPad());

   if ((topPad != NULL) && (bottomPad != NULL))
   {
      descriptor = "PS_TH";

      //toolBlock = m_camCadDatabase.getDefinedTool(fhs);
      toolCount--;
   }
   else if (!forceThFlag)
   {
      if (topPad != NULL)
      {
         descriptor = "PS";
      }
      else
      {
         descriptor = "PS_INV";
      }
   }
   else
   {
      if (topPad != NULL)
      {
         descriptor = "PS_FTH";
      }
      else
      {
         descriptor = "PS_FTH_INV";
      }

      toolCount--;
   }

   BlockStruct* topPadBlock    = NULL;
   BlockStruct* bottomPadBlock = NULL;
   double topPadRotationRadians;
   double bottomPadRotationRadians;
   double bottomPadRotationDegrees = 0.;
   int topPadCount      = 0;
   int topPasteCount    = 0;
   int bottomPadCount   = 0;
   int bottomPasteCount = 0;

   if (topPad != NULL)
   {
      BlockStruct*& padBlock = topPadBlock;

      DataStruct* pin = topPad;
      InsertStruct& pinInsert = *(pin->getInsert());
      padBlock = m_camCadDatabase.getBlock(pinInsert.getBlockNumber());
      rotationRadians = topPadRotationRadians = pinInsert.getAngleRadians();

      if (padBlock->getShape() == apertureComplex)
      {
         apertureDescriptor.Format("%d",padBlock->getBlockNumber());
      }
      else
      {
         CStandardAperture standardAperture(padBlock,m_camCadDatabase.getPageUnits());
         apertureDescriptor = standardAperture.getDescriptor();
         descriptor += "_" + apertureDescriptor;
      }

      padsExtent.update(padBlock->getExtent(getCamCadData()));

      topPadCount--;
      topPasteCount--;
   }

   if (bottomPad != NULL)
   {
      BlockStruct*& padBlock = bottomPadBlock;

      DataStruct* pin = bottomPad;
      InsertStruct& pinInsert = *(pin->getInsert());
      padBlock = m_camCadDatabase.getBlock(pinInsert.getBlockNumber());
      rotationRadians = bottomPadRotationRadians = pinInsert.getAngleRadians();

      if (padBlock->getShape() == apertureComplex)
      {
         apertureDescriptor.Format("%d",padBlock->getBlockNumber());
      }
      else
      {
         CStandardAperture standardAperture(padBlock,m_camCadDatabase.getPageUnits());
         apertureDescriptor = standardAperture.getDescriptor();
         descriptor += "_" + apertureDescriptor;
      }

      padsExtent.update(padBlock->getExtent(getCamCadData()));

      bottomPadCount--;
      bottomPasteCount--;
   }

   if (toolCount != 0)
   {
      fhs = padsExtent.getMinSize() / 2.;
      toolBlock = m_camCadDatabase.getDefinedTool(fhs);

      bottomPadRotationRadians -= topPadRotationRadians;
      rotationRadians           = topPadRotationRadians;

      if (!topFlag)
      {
         bottomPadRotationRadians *= -1.;
         rotationRadians          *= -1.;
      }

      bottomPadRotationDegrees  = normalizeDegrees(radiansToDegrees(bottomPadRotationRadians));

      if (!radiansApproximatelyEqual(bottomPadRotationDegrees,0.))
      {
         descriptor += CString("_") + fpfmt(bottomPadRotationDegrees,1);
      }
   }

   int topPadLayerIndex      = getLayer(ccLayerPadTop     )->getLayerIndex();
   int bottomPadLayerIndex   = getLayer(ccLayerPadBottom  )->getLayerIndex();
   int topPasteLayerIndex    = getLayer(ccLayerPasteTop   )->getLayerIndex();
   int bottomPasteLayerIndex = getLayer(ccLayerPasteBottom)->getLayerIndex();

   CString padStackName(descriptor);
   BlockStruct* padStackGeometry = NULL;

   for (int ind = 1;;ind++)
   {
      padStackGeometry = m_camCadDatabase.getBlock(padStackName,m_pcbFile->getFileNumber());

      // block doesn't exist, create a new one
      if (padStackGeometry == NULL)
      { 
         padStackGeometry = m_camCadDatabase.getNewBlock(padStackName,"-%d",blockTypePadstack,m_pcbFile->getFileNumber());

         if (topPadBlock != NULL)
         {
            m_camCadDatabase.referenceBlock(padStackGeometry,topPadBlock,insertTypeUnknown,"",topPadLayerIndex  );
            m_camCadDatabase.referenceBlock(padStackGeometry,topPadBlock,insertTypeUnknown,"",topPasteLayerIndex);
         }

         if (bottomPadBlock != NULL)
         {
            m_camCadDatabase.referenceBlock(padStackGeometry,bottomPadBlock,insertTypeUnknown,"",bottomPadLayerIndex  ,
               0.,0.,bottomPadRotationRadians);
            m_camCadDatabase.referenceBlock(padStackGeometry,bottomPadBlock,insertTypeUnknown,"",bottomPasteLayerIndex,
               0.,0.,bottomPadRotationRadians);
         }

         if (toolBlock != NULL)
         {
            int toolLayerIndex = m_camCadDatabase.getCamCadLayerIndex(ccLayerDrillHoles);
            m_camCadDatabase.referenceBlock(padStackGeometry,toolBlock,insertTypeUnknown,"",toolLayerIndex);
         }

         break;
      }

      // check block numbers match for existing padstack
      bool matchFlag = true;

      for (POSITION padPos = padStackGeometry->getHeadDataPosition();padPos != NULL && matchFlag;)
      {
         DataStruct* pad = padStackGeometry->getNextData(padPos);

         if (topPadBlock != NULL &&
             (pad->getLayerIndex() == topPadLayerIndex || pad->getLayerIndex() == topPasteLayerIndex))
         {
            matchFlag = aperturesEqual(m_camCadDatabase,pad,topPad);

            if (pad->getLayerIndex() == topPadLayerIndex)
            {
               topPadCount++;
            }
            else
            {
               topPasteCount++;
            }
         }
         else if (bottomPadBlock != NULL &&
                  (pad->getLayerIndex() == bottomPadLayerIndex || pad->getLayerIndex() == bottomPasteLayerIndex))
         {
            matchFlag = aperturesEqual(m_camCadDatabase,pad,bottomPad,bottomPadRotationRadians);

            if (pad->getLayerIndex() == bottomPadLayerIndex)
            {
               bottomPadCount++;
            }
            else
            {
               bottomPasteCount++;
            }
         }
         else if (toolBlock != NULL)
         {
            matchFlag = (pad->getInsert()->getBlockNumber() == toolBlock->getBlockNumber());

            if (matchFlag)
            {
               toolCount++;
            }
         }
         else
         {
            matchFlag = false;
         }
      }

      if (matchFlag)
      {
         if (topPadCount   == 0 && bottomPadCount   == 0 &&
             topPasteCount == 0 && bottomPasteCount == 0 &&
             toolCount == 0)
         {
            break;
         }
      }

      padStackName.Format("%s-%d",(const char*)descriptor,ind);
   }

   return padStackGeometry;
}

void CGerberEducator::getPadstackSurfacePads(DataStruct& pin,DataStruct*& topPad,DataStruct*& bottomPad)
{
   int topPadLayerIndex      = getLayer(ccLayerPadTop     )->getLayerIndex();
   int bottomPadLayerIndex   = getLayer(ccLayerPadBottom  )->getLayerIndex();

   topPad = bottomPad = NULL;

   if (pin.getDataType() == dataTypeInsert)
   {
      BlockStruct* padstackGeometry = m_camCadDatabase.getBlock(pin.getInsert()->getBlockNumber());

      for (CDataListIterator padIterator(*padstackGeometry,dataTypeInsert);padIterator.hasNext();)
      {
         DataStruct* pad = padIterator.getNext();

         if (pad->getLayerIndex() == topPadLayerIndex || pad->getLayerIndex() == bottomPadLayerIndex)
         {
            BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

            if (padGeometry->isAperture())
            {
               if (pad->getLayerIndex() == topPadLayerIndex)
               {
                  topPad = pad;
               }
               else
               {
                  bottomPad = pad;
               }
            }
         }
      }
   }
}

DataStruct* CGerberEducator::getPadstackSurfacePad(DataStruct& pin,bool topFlag)
{
   DataStruct* topPad;
   DataStruct* bottomPad;

   getPadstackSurfacePads(pin,topPad,bottomPad);

   DataStruct* surfacePad = (topFlag ? topPad : bottomPad);

   return surfacePad;
}

int CGerberEducator::getLinkedLayerIndex(int layerIndex)
{
   int retval = layerIndex;

   if      (layerIndex == getTopUnmatchedLayerIndex()   ) retval = getTopMatchedLayerIndex();
   else if (layerIndex == getTopMatchedLayerIndex()     ) retval = getTopUnmatchedLayerIndex();
   else if (layerIndex == getBottomUnmatchedLayerIndex()) retval = getBottomMatchedLayerIndex();
   else if (layerIndex == getBottomMatchedLayerIndex()  ) retval = getBottomUnmatchedLayerIndex();

   return retval;
}

int CGerberEducator::getOppositeLayerIndex(int layerIndex)
{
   int retval = layerIndex;

   if      (layerIndex == getTopUnmatchedLayerIndex()   ) retval = getBottomUnmatchedLayerIndex();
   else if (layerIndex == getTopMatchedLayerIndex()     ) retval = getBottomMatchedLayerIndex();
   else if (layerIndex == getBottomUnmatchedLayerIndex()) retval = getTopUnmatchedLayerIndex();
   else if (layerIndex == getBottomMatchedLayerIndex()  ) retval = getTopMatchedLayerIndex();

   return retval;
}

int CGerberEducator::getUnmatchedLayerIndexForPadLayer(int padLayerIndex,bool componentPlacedTopFlag)
{
   int unmatchedLayerIndex = -1;

   if      (padLayerIndex == getCamCadDatabase().getLayer(ccLayerPadTop)->getLayerIndex())
   {
      unmatchedLayerIndex = (componentPlacedTopFlag ? getTopUnmatchedLayerIndex() : getBottomUnmatchedLayerIndex());
   }
   else if (padLayerIndex == getCamCadDatabase().getLayer(ccLayerPadBottom)->getLayerIndex())
   {
      unmatchedLayerIndex = (componentPlacedTopFlag ? getBottomUnmatchedLayerIndex() : getTopUnmatchedLayerIndex());
   }

   return unmatchedLayerIndex;
}

int CGerberEducator::getPadLayerIndexForUnmatchedLayer(int unmatchedLayerIndex,bool componentPlacedTopFlag)
{
   int padLayerIndex = -1;

   if      (unmatchedLayerIndex == getTopUnmatchedLayerIndex())
   {
      padLayerIndex = getCamCadDatabase().getLayer(componentPlacedTopFlag ? ccLayerPadTop : ccLayerPadBottom)->getLayerIndex();
   }
   else if (unmatchedLayerIndex == getBottomUnmatchedLayerIndex())
   {
      padLayerIndex = getCamCadDatabase().getLayer(componentPlacedTopFlag ? ccLayerPadBottom : ccLayerPadTop)->getLayerIndex();
   }

   return padLayerIndex;
}

void CGerberEducator::createComponentGeometry(BlockStruct& constructedComponentGeometry,
   CBasesVector& componentBasesVector,CPinDataTreeList& pinList,
   DataStruct* centroidData,bool topFlag,bool forceThFlag)
{
   constructedComponentGeometry.setFileNumber(m_pcbFile->getFileNumber());
   constructedComponentGeometry.setBlockType(blockTypePcbComponent);
   constructedComponentGeometry.getDataList().empty();

   int pinNumber = 1;
   CString pinNumberString;
   double rotationRadians = 0.;
   CTMatrix topNormalizedMatrix;

   for (POSITION pos = pinList.GetHeadPosition();pos != NULL;pinNumber++)
   {
      CPinData* pinData = pinList.GetNext(pos);
      CPoint2d pinOrigin = pinData->getOrigin();

      if (pinNumber == 1)
      {
         topNormalizedMatrix.translate(-pinOrigin);
         topNormalizedMatrix.mirrorAboutYAxis(!topFlag);
      }

      topNormalizedMatrix.transform(pinOrigin);

      DataStruct* pin = pinData->getPad();
      InsertStruct& pinInsert = *(pin->getInsert());
      pinNumberString.Format("%d",pinNumber);

      double padStackFhs = 0.;
      BlockStruct* padStackGeometry = getPadStackGeometry(*pinData,topFlag,forceThFlag,rotationRadians,padStackFhs);

      DataStruct* pinInsertData = m_camCadDatabase.referenceBlock(&constructedComponentGeometry,
                                       padStackGeometry,insertTypePin,pinNumberString,-1,
                                       pinOrigin.x,pinOrigin.y,rotationRadians);
   }

   BlockStruct* centroidBlock = m_camCadDatabase.getDefinedCentroidGeometry(m_pcbFile->getFileNumber());
   int centroidLayerIndex = getLayer(ccLayerCentroidTop)->getLayerIndex();

   if (centroidData != NULL)
   {
      CBasesVector centroidBasesVector = centroidData->getInsert()->getBasesVector();
      centroidBasesVector.transform(topNormalizedMatrix);

      DataStruct* centroidInsertData = m_camCadDatabase.referenceBlock(&constructedComponentGeometry,
                                          centroidBlock,insertTypeCentroid,"",centroidLayerIndex,
                                          centroidBasesVector.getOrigin().x,centroidBasesVector.getOrigin().y,
                                          centroidBasesVector.getRotationRadians());
   }

   CPcbComponentPinAnalyzer pinAnalyzer(m_camCadDatabase,constructedComponentGeometry);
   CTMatrix oldToNewGeometryMatrix;
   CBasesVector componentBasesVector2;
   pinAnalyzer.updateGeometry(componentBasesVector2,oldToNewGeometryMatrix);
   constructedComponentGeometry.setName(pinAnalyzer.getGeometryDescriptor());

   CGeometryReviewStatusAttribute geometryReviewStatusAttribute(m_camCadDatabase);
   geometryReviewStatusAttribute.set(pinAnalyzer);
   geometryReviewStatusAttribute.storeToAttribute(*(constructedComponentGeometry.getDefinedAttributes()));

   int technologyKeywordIndex = m_camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeTechnology);
   CString technologyValue = (pinAnalyzer.isThruHoleComponent() ? "THRU" : "SMD");
   m_camCadDatabase.addAttribute(constructedComponentGeometry.attributes(),technologyKeywordIndex,technologyValue);

   // The following commented out code was messing up the orientation markers for soic, plcc, etc. - knv 20051202
	//// This is to apply IPC rotations rules "just like the button" in ReviewGeometry
	//// Really seems like we should just fix pinAnalyzer
	//int toIpcRot = setIpcStandardOrientation(&constructedComponentGeometry);
	//CTMatrix toIpcRotMat;
	//if (toIpcRot != 0)
	//	toIpcRotMat.rotateDegrees(toIpcRot);

	//CTMatrix matrix = topNormalizedMatrix * oldToNewGeometryMatrix * toIpcRotMat;

   CTMatrix matrix = topNormalizedMatrix * oldToNewGeometryMatrix;
   matrix.invert();

   componentBasesVector.transform(matrix);

   bool addCentroidFlag = true;

   if (addCentroidFlag && centroidData == NULL)
   {
      DataStruct* centroidData = m_camCadDatabase.referenceBlock(&constructedComponentGeometry,
                                    centroidBlock,insertTypeCentroid,"",centroidLayerIndex);
   }
}

void CGerberEducator::addComponentByCentroid(DataStruct& centroid,CPinDataTreeList& pinList,bool forceThFlag,GerberEducationMethodTag educationMethod)
{
   CString refDes        = getRefDes(centroid);
   CString shape         = getShape(centroid);
   CString partNumber    = getPartNumber(centroid);
   CString machineFeeder = getMachineFeeder(centroid);
   CString machine       = getMachine(centroid);
   CString feeder        = getFeeder(centroid);
   int fhsValue       = -1;
   InsertStruct& centroidInsert = *(centroid.getInsert());
   bool topFlag = (centroid.getLayerIndex() == getTopUnmatchedLayerIndex());

   DataStruct* component = addComponent(refDes,shape,partNumber,fhsValue,
                              topFlag,forceThFlag,pinList,&centroid,educationMethod);

   m_camCadDatabase.addAttribute(getAttributeMap(component),m_machineFeederKeywordIndex  ,machineFeeder  );
   m_camCadDatabase.addAttribute(getAttributeMap(component),m_machineKeywordIndex        ,machine        );
   m_camCadDatabase.addAttribute(getAttributeMap(component),m_feederKeywordIndex         ,feeder         );

   int matchedLayerIndex = (topFlag ? getTopMatchedLayerIndex() : getBottomMatchedLayerIndex());
   centroid.setLayerIndex(matchedLayerIndex);
}

void CGerberEducator::addComponentAtCentroid(const CString& refDes,const CString& shape,
   const CString& partNumber,bool topFlag,bool forceThFlag,double fhs,double rotationRadians,bool preferredCcwOrthoRotationFlag,
   CPinDataTreeList& pinList)
{
   if (pinList.GetCount() > 0)
   {
      CPoint2d componentOrigin = pinList.getCentroid();

      DataStruct* component = addComponent(refDes,shape,partNumber,fhs,topFlag,forceThFlag,
                                 pinList,NULL,gerberEducationMethodByPinPattern);
   }
}

//InsertStruct *CGerberEducator::getFirstPinInsert(BlockStruct *block)
//{
//	// Based on CGEReviewGeometriesDlg::loadPinsGrid()
//	// and CGEReviewGeometriesDlg::getFirstSortedPinInGrid(..)
//
//	CString firstPinRefdes;
//	InsertStruct *firstPinInsert = NULL;
//
//   for (POSITION pos=block->getHeadDataInsertPosition(); pos != NULL;)
//   {
//		DataStruct* pin = block->getNextDataInsert(pos);
//		if (pin == NULL || pin->getDataType() != dataTypeInsert || pin->getInsert() == NULL)
//			continue;
//		
//		InsertStruct* pinInsert = pin->getInsert();
//		if (pinInsert->getInsertType() != insertTypePin && pinInsert->getInsertType() != insertTypeMechanicalPin &&
//			 pinInsert->getInsertType() != insertTypeFiducial)
//			continue;
//
//		BlockStruct* pinPadstack = m_camCadDatabase.getCamCadDoc().getBlockAt(pinInsert->getBlockNumber());
//		if (pinPadstack == NULL)
//			continue;
//		m_camCadDatabase.getCamCadDoc().validateBlockExtents(pinPadstack);
//		
//		if (firstPinInsert == NULL)
//		{
//			firstPinRefdes = pin->getInsert()->getSortableRefDes();
//			firstPinInsert = pin->getInsert();
//		}
//		else
//		{
//			CString candidateRefdes = pin->getInsert()->getSortableRefDes();
//			if (candidateRefdes < firstPinRefdes)
//			{
//				firstPinRefdes = candidateRefdes;
//				firstPinInsert = pin->getInsert();
//			}
//		}
//	}
//
//	return firstPinInsert;
//}
//
//int CGerberEducator::setIpcStandardOrientation(BlockStruct *block)
//{
//	// Returns rotation applied to get from current orientatino to IPC standard orientation
//
//	int rotation = 0;
//
//	/// Mostly snatched from "review geometries dialog", need to consolidate so there
//	/// is a shared utility to do this, not twins.
//;
//	if (block != NULL)
//	{
//		InsertStruct* centroid = (block->GetCentroidData()!=NULL)?block->GetCentroidData()->getInsert():NULL;
//		InsertStruct* pin = getFirstPinInsert(block);
//		if (pin == NULL)
//			return 0;
//
//		double x = pin->getOriginX();
//		double y = pin->getOriginY();
//		if (centroid != NULL)
//		{
//			x -= centroid->getOriginX();
//			y -= centroid->getOriginY();
//		}
//
//		int orientationIndex = outlineIndicatorOrientationUndefined;
//		orientationIndex = block->getComponentOutlineIndicatorOrientation(m_camCadDatabase);
//
//		double oneMils = Units_Factor(pageUnitsMils, m_camCadDoc.getSettings().getPageUnits()) * 0.1;
//		if (block->getPinCount() == 2)
//		{
//			if (x < oneMils && y <= -oneMils)
//			{
//				// pin in Quadrant III
//				rotation = 270;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 6;
//			}
//			else if (x >= oneMils && y < oneMils)
//			{
//				// pin in Quadrant IV
//				rotation = 180;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 4;
//			}
//			else if (x > -oneMils && y >= oneMils)
//			{
//				// pin in Quadrant I
//				rotation = 90;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 2;
//			}
//		}
//		else
//		{
//			if (x < -oneMils && y <= oneMils)
//			{
//				// pin in Quadrant III
//				rotation = 270;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 6;
//			}
//			else if (x >= -oneMils && y < -oneMils)
//			{
//				// pin in Quadrant IV
//				rotation = 180;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 4;
//			}
//			else if (x > oneMils && y >= -oneMils)
//			{
//				// pin in Quadrant I
//				rotation = 90;
//				if (orientationIndex != outlineIndicatorOrientationUndefined)
//					orientationIndex = orientationIndex + 2;
//			}
//		}
//
//		while (orientationIndex > outlineIndicatorOrientationUpperBound)
//      {
//			orientationIndex -= outlineIndicatorOrientationUpperBound + 1;
//      }
//
//		CTMatrix matrix;
//		matrix.rotateDegrees(rotation);
//		
//		///doTransform(matrix);
//		// This is used during block definition, so there can not yet be inserts of this block.
//		// So only need to transform this block.
//		block->transform(matrix, &m_camCadDoc, true);
//
//		DataStruct* blockCentroid = block->GetCentroidData();
//		if (blockCentroid != NULL)
//		{
//			InsertStruct* insert = blockCentroid->getInsert();
//			insert->setAngle(0.0);
//			insert->setOrigin(0.0, 0.0);
//		}
//
//		block->setComponentOutlineIndicatorOrientation(m_camCadDatabase, (OutlineIndicatorOrientationTag)orientationIndex);
//	}
//	return rotation;
//}

void CGerberEducator::erase(DataStruct& data)
{
   int fileMirror = ((m_camCadDoc.getBottomView() ? MIRROR_FLIP : 0) ^ (m_pcbFile->isMirrored()) ? MIRROR_FLIP : 0);

	//SelectStruct* selectStruct = m_camCadDoc.InsertInSelectList(&data, m_pcbFile->getBlock()->getFileNumber(),
	//		m_pcbFile->getInsertX(), m_pcbFile->getInsertY(),  m_pcbFile->getScale(), 
	//		m_pcbFile->getRotation(), fileMirror, &(m_pcbFile->getBlock()->getDataList()), FALSE, TRUE);

   SelectStruct selectStruct;
   selectStruct.setData(&data);
   //selectStruct.p->setSelected(false);
   selectStruct.filenum  = m_pcbFile->getBlock()->getFileNumber();
   selectStruct.insert_x = m_pcbFile->getInsertX();
   selectStruct.insert_y = m_pcbFile->getInsertY();
   selectStruct.scale    = m_pcbFile->getScale();
   selectStruct.rotation = m_pcbFile->getRotation();
   selectStruct.mirror   = fileMirror;
   selectStruct.layer    = -1;
   //selectStruct.DataList = DataList;

	m_camCadDoc.DrawEntity(selectStruct,drawStateErase);
}

DataStruct* CGerberEducator::addComponent(
   const CString& refDes,const CString& shape,const CString& partNumber,double fhs,
   const bool topFlag,bool forceThFlag,
   CPinDataTreeList& pinList,DataStruct* centroidData,GerberEducationMethodTag educationMethod)
{
   int matchedLayerIndex       = (topFlag ? getTopMatchedLayerIndex()       : getBottomMatchedLayerIndex());
   int unmatchedLayerIndex     = (topFlag ? getTopUnmatchedLayerIndex()     : getBottomUnmatchedLayerIndex());
   //int centroidLayerIndex      = getLayer(ccLayerEducatorCentroidTop)->getLayerIndex();
   //int assemblyLayerIndex      = getLayer(ccLayerAssemblyTop        )->getLayerIndex();   
   //int gerberInputLayerIndex   = (topFlag ? m_topPadInputLayerIndex   : m_bottomPadInputLayerIndex);   
   //int centroidInputLayerIndex = (topFlag ? m_topCentroidInputLayerIndex : m_bottomCentroidInputLayerIndex);   

   if (centroidData != NULL && centroidData->getLayerIndex() == unmatchedLayerIndex)
   {
      centroidData->setLayerIndex(matchedLayerIndex);
      m_gerberEducatorProgressSummary.matchCentroid(topFlag);
   }

   bool thFlag = forceThFlag;

   for (POSITION pos = pinList.GetHeadPosition();pos != NULL;/*pinNumber++*/)
   {
      CPinData* pinData = pinList.GetNext(pos);

      if (pinData->getTopPad() != NULL)
      {
         erase(*(pinData->getTopPad()));
         pinData->getTopPad()->setLayerIndex(getLinkedLayerIndex(pinData->getTopPad()->getLayerIndex()));
         m_gerberEducatorProgressSummary.matchPad(true);
      }

      if (pinData->getBottomPad() != NULL)
      {
         erase(*(pinData->getBottomPad()));
         pinData->getBottomPad()->setLayerIndex(getLinkedLayerIndex(pinData->getBottomPad()->getLayerIndex()));
         m_gerberEducatorProgressSummary.matchPad(false);
      }

      thFlag = thFlag || ((pinData->getTopPad()    != NULL) && (pinData->getBottomPad()    != NULL));
   }

   CBasesVector componentBasesVector;
   BlockStruct* componentGeometry = m_componentGeometryDirectory.findMatchingComponentGeometry(
                                       pinList,centroidData,componentBasesVector,topFlag,forceThFlag); 

   if (componentGeometry == NULL)
   {
      BlockStruct constructedComponentGeometry;
      createComponentGeometry(constructedComponentGeometry,componentBasesVector,pinList,centroidData,topFlag,forceThFlag);

      CString geometryBaseName = constructedComponentGeometry.getName();

      if (geometryBaseName.IsEmpty())
      {
         geometryBaseName = "GE_Component";
      }
      else
      {
         geometryBaseName = (thFlag ? "GE_th_" : "GE_smd_") + geometryBaseName;
      }

      CString geometryName = m_camCadDatabase.getNewBlockName(geometryBaseName,"-%d",m_pcbFile->getFileNumber(),false);
      componentGeometry = m_camCadDatabase.copyBlock(geometryName,&constructedComponentGeometry);
      m_componentGeometryDirectory.add(componentGeometry);
   }

   DataStruct* component = NULL;

   if (componentGeometry != NULL)
   {
      component = m_camCadDatabase.referenceBlock(m_pcbFile->getBlock(),componentGeometry,
                     insertTypePcbComponent,refDes,-1);

      component->getInsert()->setBasesVector(componentBasesVector);

      if (centroidData != NULL)
      {
         double centroidAngleRadians = centroidData->getInsert()->getAngleRadians();
         double deltaAngleDegrees = normalizeDegrees(radiansToDegrees(centroidAngleRadians - componentBasesVector.getRotationRadians()),.01);
         int pinSymmetryCount = componentGeometry->getDataList().getSymmetryCount(m_pinInsertTypeMask,symmetryCriteriaPadstack,getPinPitchTolerance());
         CPoint2d pinExtentCenter = componentGeometry->getPinExtent().getCenter();

         if ((pinSymmetryCount == 2) && fpeq(fmod(deltaAngleDegrees,180.),0.) && 
              pinExtentCenter.fpeq(CPoint2d(0.,0.),getCoordinateTolerance()))
         {
            component->getInsert()->setAngleRadians(centroidAngleRadians);
         }

			// Copy all attributes from centroid over to pcb component
			component->getAttributesRef()->CopyAll(*centroidData->getAttributesRef());

      }

      component->getInsert()->setPlacedBottom(!topFlag);
      component->getInsert()->setGraphicsMirrorFlag(!topFlag);
      component->getInsert()->setLayerMirrorFlag(!topFlag);

      double rotationDegrees = normalizeDegrees(component->getInsert()->getAngleDegrees());

      //if (!componentPlacedTopFlag && !topFlag)
      //{
      //   rotationDegrees += 180.;
      //   component->getInsert()->setAngleDegrees(rotationDegrees);
      //}
      
      if (fpnear(fmod(rotationDegrees,90.),0.,.1))
      {
         rotationDegrees = round(rotationDegrees);
         component->getInsert()->setAngleDegrees(rotationDegrees);
      }

      m_camCadDatabase.addAttribute(getAttributeMap(component),m_refDesKeywordIndex         ,refDes         );
      m_camCadDatabase.addAttribute(getAttributeMap(component),m_partNumberKeywordIndex     ,partNumber     );
      m_camCadDatabase.addAttribute(getAttributeMap(component),m_shapeKeywordIndex          ,shape          );
      m_camCadDatabase.addAttribute(getAttributeMap(component),m_educationMethodKeywordIndex,gerberEducationMethodTagToString(educationMethod));

      m_components.SetAt(refDes,component);
      addToRedrawList(component);

      // add comp pins
      addCompPins(*component);

      m_gerberEducatorProgressSummary.addComponent(*component);
   }

   return component;
}

void CGerberEducator::addCompPins(DataStruct& component)
{
   CTMatrix matrix = component.getInsert()->getTMatrix();
   BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
   CString refDes = component.getInsert()->getRefname();
   NetStruct* net = m_camCadDatabase.getDefinedNet(NET_UNUSED_PINS,m_pcbFile);

   for (CDataListIterator pinIterator(*componentGeometry,insertTypePin);pinIterator.hasNext();)
   {
      DataStruct* padStack = pinIterator.getNext();

      CString pinName = padStack->getInsert()->getRefname();

      m_camCadDatabase.addCompPin(net,refDes,pinName);
   }
}

int CGerberEducator::augmentPins(CPinDataTreeList& selectedPins)
{
   int augmentedCount = 0;

   CQfeApertureInsertList apertureList;
   CExtent entireExtent;
   entireExtent.maximize();

   m_apertureInsertTree.search(entireExtent,apertureList);

   for (POSITION padPos = apertureList.GetHeadPosition();padPos != NULL;)
   {
      CQfeApertureInsert* aperture = apertureList.GetNext(padPos);

      if (aperture == NULL)
      {
         continue;
      }

      int pinLayerIndex  = aperture->getAperture()->getLayerIndex();

      if (!isOnUnmatchedLayer(pinLayerIndex))
      {
         continue;
      }

      CPinData* pinData = selectedPins.addToExistingPin(aperture->getAperture(),isOnTopMatchTypeLayer(pinLayerIndex));

      if (pinData != NULL)
      {
         augmentedCount++;
      }
   }

   return augmentedCount;
}

bool CGerberEducator::padShapesEqual(BlockStruct* padGeometry0,BlockStruct* padGeometry1)
{
   bool retval = false;

   if (padGeometry0 != NULL && padGeometry1 != NULL)
   {
      if (padGeometry0->isAperture() && padGeometry1->isAperture())
      {
         if (padGeometry0 == padGeometry1)
         {
            retval = true;
         }
         else if (padGeometry0->getShape() == padGeometry1->getShape())
         {
            if (padGeometry0->isComplexAperture())
            {
               retval = (padGeometry0->getComplexApertureSubBlockNumber() == padGeometry1->getComplexApertureSubBlockNumber());
            }
            else
            {
               double tolerance = getPadShapeTolerance();

               switch (padGeometry0->getShape())
               {
               case apertureRectangle:
               case apertureOblong:
                  {
                     double minSize0 = min(padGeometry0->getSizeA(),padGeometry0->getSizeB());
                     double maxSize0 = max(padGeometry0->getSizeA(),padGeometry0->getSizeB());
                     double minSize1 = min(padGeometry1->getSizeA(),padGeometry1->getSizeB());
                     double maxSize1 = max(padGeometry1->getSizeA(),padGeometry1->getSizeB());

                     retval = (fpnear(minSize0,minSize1,tolerance) && fpnear(maxSize0,maxSize1,tolerance));
                  }

                  break;
               case apertureRound:
               case apertureSquare:
               case apertureOctagon:
               case apertureBlank:
                  retval = fpnear(padGeometry0->getSizeA(),padGeometry1->getSizeA(),tolerance);

                  break;
               case apertureDonut:
                  retval = (fpnear(padGeometry0->getSizeA(),padGeometry1->getSizeA(),tolerance) &&
                            fpnear(padGeometry0->getSizeB(),padGeometry1->getSizeB(),tolerance)     );

                  break;
               }
            }
         }
      }
   }

   return retval;
}

bool CGerberEducator::padStackPadsEqual(BlockStruct& padStack0,BlockStruct& padStack1)
{
   bool retval = (&padStack0 == &padStack1);

   if (!retval)
   {
      int topPadLayerIndex      = getLayer(ccLayerPadTop     )->getLayerIndex();
      int bottomPadLayerIndex   = getLayer(ccLayerPadBottom  )->getLayerIndex();

      BlockStruct* topPad0    = NULL;
      BlockStruct* topPad1    = NULL;
      BlockStruct* bottomPad0 = NULL;
      BlockStruct* bottomPad1 = NULL;
      BlockStruct* tool0      = NULL;
      BlockStruct* tool1      = NULL;
      double topPadRotationRadians0    = 0.;
      double topPadRotationRadians1    = 0.;
      double bottomPadRotationRadians0 = 0.;
      double bottomPadRotationRadians1 = 0.;

      for (CDataListIterator padIterator(padStack0,dataTypeInsert);padIterator.hasNext();)
      {
         DataStruct* pad = padIterator.getNext();
         BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

         if (padGeometry->isAperture())
         {
            if (pad->getLayerIndex() == topPadLayerIndex)
            {
               topPad0 = padGeometry;
               topPadRotationRadians0 = pad->getInsert()->getAngleRadians();
            }
            else if (pad->getLayerIndex() == bottomPadLayerIndex)
            {
               bottomPad0 = padGeometry;
               bottomPadRotationRadians0 = pad->getInsert()->getAngleRadians();
            }
         }
         else if (padGeometry->isTool())
         {
            tool0 = padGeometry;
         }
      }

      for (CDataListIterator padIterator(padStack1,dataTypeInsert);padIterator.hasNext();)
      {
         DataStruct* pad = padIterator.getNext();
         BlockStruct* padGeometry = m_camCadDatabase.getBlock(pad->getInsert()->getBlockNumber());

         if (padGeometry->isAperture())
         {
            if (pad->getLayerIndex() == topPadLayerIndex)
            {
               topPad1 = padGeometry;
               topPadRotationRadians1 = pad->getInsert()->getAngleRadians();
            }
            else if (pad->getLayerIndex() == bottomPadLayerIndex)
            {
               bottomPad1 = padGeometry;
               bottomPadRotationRadians1 = pad->getInsert()->getAngleRadians();
            }
         }
         else if (padGeometry->isDrillHole())
         {
            tool1 = padGeometry;
         }
      }

      retval =  (((topPad0    == NULL) == (topPad1    == NULL)) &&
                 ((bottomPad0 == NULL) == (bottomPad1 == NULL)) &&
                 ((tool0      == NULL) == (tool1      == NULL))    );

      if (retval && topPad0 != NULL && topPad1 != NULL)
      {
         if (!padShapesEqual(topPad0,topPad1) || !radiansApproximatelyEqual(topPadRotationRadians0,topPadRotationRadians1))
         {
            retval = false;
         }
      }

      if (retval && bottomPad0 != NULL && bottomPad1 != NULL)
      {
         if (!padShapesEqual(bottomPad0,bottomPad1) || !radiansApproximatelyEqual(bottomPadRotationRadians0,bottomPadRotationRadians1))
         {
            retval = false;
         }
      }

      if (retval && tool0 != NULL && tool1 != NULL)
      {
         if (tool0 != tool1)
         {
            retval = false;
         }
      }
   }

   return retval;
}

bool CGerberEducator::padStacksEqual(BlockStruct& padStack0,BlockStruct& padStack1,double deltaAngleRadians)
{
   bool retval = padStackPadsEqual(padStack0,padStack1);

   return retval;
}

void CGerberEducator::addToRedrawList(DataStruct* data)
{
   if (data != NULL)
   {
      m_redrawList.AddTail(data);
   }
}

void CGerberEducator::emptyRedrawList()
{
   m_redrawList.empty();
}

void CGerberEducator::drawRedrawList(bool emptyFlag)
{
   for (POSITION pos = m_redrawList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_redrawList.GetNext(pos);

      //int fileMirror = ((m_camCadDoc.Bottom ? MIRROR_FLIP : 0) ^ (m_pcbFile->isMirrored()) ? MIRROR_FLIP | MIRROR_LAYERS : 0);
      int fileMirror = ((m_camCadDoc.getBottomView() ? MIRROR_FLIP : 0) ^ (m_pcbFile->isMirrored()) ? MIRROR_FLIP : 0);

	   SelectStruct* selectStruct = m_camCadDoc.InsertInSelectList(data, m_pcbFile->getBlock()->getFileNumber(),
			   m_pcbFile->getInsertX(), m_pcbFile->getInsertY(),  m_pcbFile->getScale(), 
			   m_pcbFile->getRotation(), fileMirror, &(m_pcbFile->getBlock()->getDataList()), FALSE, TRUE);

	   m_camCadDoc.DrawEntity(*selectStruct,drawStateNormal);
   }

   //m_camCadDoc.OnClearSelected();

   if (emptyFlag)
   {
      m_redrawList.empty();
   }
}

void CGerberEducator::addToEraseList(DataStruct* data)
{
   if (data != NULL)
   {
      m_eraseList.AddTail(data);
   }
}

void CGerberEducator::emptyEraseList()
{
   m_eraseList.empty();
}

void CGerberEducator::drawEraseList(DataStruct* data,bool emptyFlag)
{
   addToEraseList(data);

   for (POSITION pos = m_eraseList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_eraseList.GetNext(pos);

      int fileMirror = ((m_camCadDoc.getBottomView() ? MIRROR_FLIP : 0) ^ (m_pcbFile->isMirrored()) ? MIRROR_FLIP : 0);

	   SelectStruct* selectStruct = m_camCadDoc.InsertInSelectList(data, m_pcbFile->getBlock()->getFileNumber(),
			   m_pcbFile->getInsertX(), m_pcbFile->getInsertY(),  m_pcbFile->getScale(), 
			   m_pcbFile->getRotation(), fileMirror, &(m_pcbFile->getBlock()->getDataList()), FALSE, TRUE);

	   m_camCadDoc.DrawEntity(selectStruct,-1, FALSE);
   }

   //m_camCadDoc.OnClearSelected();

   if (emptyFlag)
   {
      m_eraseList.empty();
   }
}

CPinTreeByPolarCoordinate* CGerberEducator::calcPinTree(CQfeApertureInsert& pin1Aperture,double searchTolerance)
{
   CPinTreeByPolarCoordinate* pinTree = NULL;

   if (isOnUnmatchedLayer(pin1Aperture.getAperture()->getLayerIndex()))
   {
      CPoint2d pin1Origin = pin1Aperture.getAperture()->getInsert()->getOrigin2d();
      CPinTreeByPolarCoordinate* pinTree = new CPinTreeByPolarCoordinate(searchTolerance);

      CQfeApertureInsertList foundList;
      CExtent searchExtent(-searchTolerance,-searchTolerance,searchTolerance,searchTolerance);

      m_apertureInsertTree.search(searchExtent,foundList);

      for (POSITION pinPos = foundList.GetHeadPosition();pinPos != NULL;)
      {
         CQfeApertureInsert* pinAperture = foundList.GetNext(pinPos);

         if (!isOnUnmatchedLayer(pinAperture->getAperture()->getLayerIndex()))
         {
            continue;
         }

         CPoint2d pinOriginRelativeToPin1 = pinAperture->getAperture()->getInsert()->getOrigin2d() - pin1Origin;
         CPolarCoordinate pinPolarCoordinate(pinOriginRelativeToPin1);

         pinTree->setAt(pinPolarCoordinate,pinAperture->getAperture());
      }
   }

   return pinTree;
}

void CGerberEducator::addComponents(
   CString refDes,CString shape,CString partNumber,double fhs,
   DataStruct& component,bool matchCentroidFlag,bool orthogonalFlag,bool forceThFlag,GerberEducationMethodTag educationMethod)
{
   CWaitCursor waitCursor;

   double searchTolerance   = 2. * getSearchTolerance();
   double pinPitchTolerance = getPinPitchTolerance();
   double pinPitch = 0.,pinAngleRadians = 0.;

   BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component.getInsert()->getBlockNumber());
   double componentInsertAngleRadians = component.getInsert()->getAngleRadians();
   bool componentPlacedTopFlag = component.getInsert()->getPlacedTop();

   DataStruct* pin1 = NULL;
   DataStruct* pin2 = NULL;
   CPoint2d geometryPin1Origin;
   CPoint2d geometryPin2Origin;

   // calculate a pin pitch from the first two pins of the component geometry
   for (CDataListIterator pinIterator(*componentGeometry,m_pinInsertTypeMask);pinIterator.hasNext();)
   {
      DataStruct* pin = pinIterator.getNext();
      InsertStruct* padStackInsert = pin->getInsert();

      if (pin1 == NULL)
      {
         pin1 = pin;
         geometryPin1Origin = padStackInsert->getOrigin2d();
      }
      else if (pin2 == NULL)
      {
         pin2 = pin;
         geometryPin2Origin = padStackInsert->getOrigin2d();
         pinPitch   = geometryPin1Origin.distance(geometryPin2Origin);
         CPoint2d pinDelta = geometryPin2Origin - geometryPin1Origin;
         pinAngleRadians = pinDelta.getTheta();

         break;
      }
   }

   if (pinPitch < searchTolerance)
   {
      return;
   }

   DataStruct* topPad1 = NULL;
   DataStruct* bottomPad1 = NULL;
   getPadstackSurfacePads(*pin1,topPad1,bottomPad1);

   bool primaryPadTopFlag = (topPad1 != NULL);

   int pinSymmetryCount = componentGeometry->getDataList().getSymmetryCount(m_pinInsertTypeMask,symmetryCriteriaPadstack,getPinPitchTolerance());

   // pad loop
   searchTolerance += pinPitch;
   CQfeApertureInsertList apertureList;
   CExtent entireExtent;
   entireExtent.maximize();
   int newComponentCount = 0;
   int nullCount = 0;

   m_apertureInsertTree.search(entireExtent,apertureList);

   enum EducatorComponentRotationState
   {
      educatorRotationStateMatch,
      educatorRotationStateOrthogonal,
      educatorRotationStateAny,
      educatorRotationStateUndefined,
   };

   for (int sameSurfacePass = 0;sameSurfacePass < 2;sameSurfacePass++)
   {
      bool sameSurfaceFlag = (sameSurfacePass == 0);

      for (EducatorComponentRotationState componentRotationState = educatorRotationStateMatch;
         componentRotationState != educatorRotationStateUndefined;
         componentRotationState = (EducatorComponentRotationState)(componentRotationState + 1))
      {
         if (orthogonalFlag && (componentRotationState != educatorRotationStateOrthogonal) &&
                               (componentRotationState != educatorRotationStateMatch     )    )
         {
            continue;
         }

         for (POSITION padPos = apertureList.GetHeadPosition();padPos != NULL;)
         {
            CQfeApertureInsert* pin1Aperture = apertureList.GetNext(padPos);

            if (pin1Aperture == NULL)
            {
               nullCount++;
               continue;
            }

            DataStruct* pin1ApertureData = pin1Aperture->getAperture();
            int pinLayerIndex  = pin1ApertureData->getLayerIndex();

            if (!isOnUnmatchedLayer(pinLayerIndex))
            {
               continue;
            }

            bool topFlag = (sameSurfaceFlag ? componentPlacedTopFlag : !componentPlacedTopFlag);

            //if (sameSurfaceFlag != (topFlag == componentPlacedTopFlag))
            //{
            //   continue;
            //}

            CPoint2d pin1Origin = pin1Aperture->getOrigin();

            CQfeApertureInsertList foundList;
            CExtent searchExtent(pin1Origin,searchTolerance);

            m_apertureInsertTree.search(searchExtent,foundList);
            bool addedComponentFlag = false;

            for (POSITION pinPos = foundList.GetHeadPosition();pinPos != NULL && !addedComponentFlag;)
            {
               CQfeApertureInsert* pin2Aperture = apertureList.GetNext(pinPos);
               DataStruct* pin2ApertureData = pin2Aperture->getAperture();
               CPoint2d pin2Origin = pin2Aperture->getOrigin();

               if (fpnear(pin2Origin.distance(pin1Origin),pinPitch,pinPitchTolerance))
               {
                  CPoint2d pinDelta = pin2Origin - pin1Origin;
                  double sideAdjustedPinAngleRadians = (topFlag ? pinAngleRadians : Pi - pinAngleRadians);
                  double deltaAngle = normalizeRadians(pinDelta.getTheta() - sideAdjustedPinAngleRadians);
                  double deltaComponentInsertAngleRadians = deltaAngle - componentInsertAngleRadians;

                  if ((componentRotationState == educatorRotationStateMatch) && !isMultipleOfRadians(deltaComponentInsertAngleRadians,2.*Pi))
                  {
                     continue;
                  }

                  if ((componentRotationState == educatorRotationStateOrthogonal) && !isMultipleOfRadians(deltaComponentInsertAngleRadians,PiOver2))
                  {
                     continue;
                  }

                  for (int pass = 0;pass < 2;pass++)
                  {
                     CTMatrix componentGeometryToPcbMatrix;
                     componentGeometryToPcbMatrix.scale(topFlag ? 1. : -1.,1.);

                     if (pass > 0)
                     {
                        deltaAngle += Pi;

                        if (componentRotationState == educatorRotationStateMatch)
                        {
                           break;
                        }
                     }

                     componentGeometryToPcbMatrix.rotateRadians(deltaAngle);
                     CPoint2d transformedOrigin = componentGeometryToPcbMatrix.transform((const CPoint2d&)geometryPin1Origin);
                     componentGeometryToPcbMatrix.translate(pin1Origin - transformedOrigin);

                     //CPoint2d transformedOrigin1 = componentGeometryToPcbMatrix.transform((const CPoint2d&)geometryPin1Origin);
                     //CPoint2d transformedOrigin2 = componentGeometryToPcbMatrix.transform((const CPoint2d&)geometryPin2Origin);
                     //CPoint2d d1 = transformedOrigin1 - pin1Origin;
                     //CPoint2d d2 = transformedOrigin2 - pin2Origin;

                     CPinDataTreeList pinList(getSearchTolerance());
                     DataStruct* centroidData;
                     bool allPinsFound = findGeometryPattern(pinList,*componentGeometry,&centroidData,componentGeometryToPcbMatrix,topFlag);

                     int pinCount = pinList.GetCount();

                     bool centroidMatchOkFlag = !matchCentroidFlag;

                     if (!centroidMatchOkFlag)
                     {
                        if (centroidData != NULL)
                        {
                           refDes     = getRefDes(*centroidData);
                           shape      = getShape(*centroidData);
                           partNumber = getPartNumber(*centroidData);
                           CExtent searchExtent(centroidData->getInsert()->getOrigin2d(),searchTolerance);

                           centroidMatchOkFlag = (getCentroidTree(topFlag).findFirst(searchExtent) != NULL);
                        }
                     }

                     if (allPinsFound && centroidMatchOkFlag)
                     {
                        newComponentCount++;
                        addedComponentFlag = true;

                        CGerberEducatorRefDes newRefDes(refDes);
                        newRefDes.setRefDes(getRefDesMap().defineNew(newRefDes.getRefDes(),QRefDesSuffixFormat));
                        //CPoint2d componentOrigin(0.,0.);
                        //componentGeometryToPcbMatrix.transform(componentOrigin);

                        DataStruct* component = addComponent(newRefDes.getRefDes(),shape,partNumber,fhs,
                                                   topFlag,forceThFlag,pinList,centroidData,educationMethod);

                        break;  // terminate pass
                     }
                  }  // pass
               }
            }
         }
      }
   }
}

void CGerberEducator::addComponentsByCentroid(DataStruct& centroid,CDataList& selectedCentroidList,
   bool forceThFlag,GerberEducationMethodTag educationMethod)
{
   const double searchTolerance = getSearchTolerance();
   CString refDes = getRefDes(centroid);
   bool topFlag = (centroid.getLayerIndex() == getTopMatchedLayerIndex());
   int sourceLayerIndex = (topFlag ? getTopUnmatchedLayerIndex() : getBottomUnmatchedLayerIndex());

   DataStruct* component = NULL;
   m_components.Lookup(refDes,component);

   if (component != NULL)
   {
      BlockStruct* componentGeometry = m_camCadDatabase.getBlock(component->getInsert()->getBlockNumber());

      for (POSITION centroidPos = selectedCentroidList.GetHeadPosition();centroidPos != NULL;)
      {
         DataStruct* selectedCentroid = selectedCentroidList.GetNext(centroidPos);
         InsertStruct* selectedCentroidInsert = selectedCentroid->getInsert();

         CString selectedRefDes = getRefDes(*selectedCentroid);
         CString selectedShape  = getShape(*selectedCentroid);

         CTMatrix matrix;
         matrix.translateCtm(selectedCentroidInsert->getOriginX(),selectedCentroidInsert->getOriginY());
         matrix.rotateRadiansCtm(selectedCentroidInsert->getAngle());
         matrix.scaleCtm(selectedCentroidInsert->getGraphicMirrored() ? -1. : 1.,1.);

         CPinDataTreeList pinList(getSearchTolerance());
         DataStruct* centroidData;
         bool allPinsFound = findGeometryPattern(pinList,*componentGeometry,&centroidData,matrix,topFlag);

         int pinCount = pinList.GetCount();

         if (allPinsFound)
         {
            addComponentByCentroid(*selectedCentroid,pinList,forceThFlag,educationMethod);
         }
      }
   }
}

void CGerberEducator::addSinglePinComponents(
   CString refDes,CString shape,CString partNumber,double fhs,
   BlockStruct& componentGeometry,bool matchCentroidFlag,double preferredRotation,bool preferredCcwOrthoRotationFlag,
   bool orthogonalFlag,bool forceThFlag,GerberEducationMethodTag educationMethod)
{
   double searchTolerance = 2. * getSearchTolerance();

   DataStruct* pin1 = NULL;
   CPoint2d geometryPin1Origin;

   for (CDataListIterator pinIterator(componentGeometry,m_pinInsertTypeMask);pinIterator.hasNext();)
   {
      DataStruct* pin = pinIterator.getNext();
      InsertStruct* padStackInsert = pin->getInsert();

      if (pin1 == NULL)
      {
         pin1 = pin;
         geometryPin1Origin = padStackInsert->getOrigin2d();
      }
      else
      {
         // more than one pin found in the geometry
         return;
      }
   }

   int pinSymmetryCount = componentGeometry.getDataList().getSymmetryCount(m_pinInsertTypeMask,symmetryCriteriaPadstack,getPinPitchTolerance());

   // pad loop
   CQfeApertureInsertList apertureList;
   CExtent entireExtent;
   entireExtent.maximize();
   int newComponentCount = 0;
   int nullCount = 0;

   m_apertureInsertTree.search(entireExtent,apertureList);

   for (POSITION padPos = apertureList.GetHeadPosition();padPos != NULL;)
   {
      CQfeApertureInsert* pin1Aperture = apertureList.GetNext(padPos);

      if (pin1Aperture == NULL)
      {
         nullCount++;
         continue;
      }

      int pinLayerIndex  = pin1Aperture->getAperture()->getLayerIndex();

      if (!isOnUnmatchedLayer(pinLayerIndex))
      {
         continue;
      }

      bool topFlag = isOnTopMatchTypeLayer(pinLayerIndex);

      CPoint2d pin1Origin = pin1Aperture->getOrigin();

      bool addedComponentFlag = false;

      CTMatrix matrix;

      if (!topFlag)
      {
         matrix.scale(-1.,1.);
      }

      CPoint2d transformedOrigin = matrix.transform((const CPoint2d&)geometryPin1Origin);
      matrix.translate(pin1Origin - transformedOrigin);

      CPinDataTreeList pinList(getSearchTolerance());
      DataStruct* centroidData = NULL;
      bool allPinsFound = findGeometryPattern(pinList,componentGeometry,matchCentroidFlag ? &centroidData : NULL,matrix,topFlag);

      int pinCount = pinList.GetCount();

      bool centroidMatchOkFlag = !matchCentroidFlag;

      if (!centroidMatchOkFlag)
      {
         if (centroidData != NULL)
         {
            refDes     = getRefDes(*centroidData);
            shape      = getShape(*centroidData);
            partNumber = getPartNumber(*centroidData);
            CExtent searchExtent(centroidData->getInsert()->getOrigin2d(),searchTolerance);

            centroidMatchOkFlag = (getCentroidTree(topFlag).findFirst(searchExtent) != NULL);
         }
      }

      if (allPinsFound && centroidMatchOkFlag)
      {
         newComponentCount++;
         addedComponentFlag = true;

         CGerberEducatorRefDes newRefDes(refDes);
         newRefDes.setRefDes(getRefDesMap().defineNew(newRefDes.getRefDes(),QRefDesSuffixFormat));
         CPoint2d componentOrigin(0.,0.);
         matrix.transform(componentOrigin);

         bool supplementaryRotationFlag = false;
         bool preferredRotationFlag = false;
         preferredRotation = normalizeRadians(preferredRotation);

         if (pinSymmetryCount > 0)
         {
            double symmetricRotation = normalizeRadians(pinSymmetryCount*(preferredRotation/* - deltaAngle*/));

            if (fpeq(symmetricRotation,0.))
            {
               preferredRotationFlag = true;
            }
            else if ((pinSymmetryCount % 2) == 0)
            {
               double supplementaryPreferredRotation = normalizeRadians(preferredRotation + Pi);
               double preferredOrthogonalRotation = normalizeRadians(preferredRotation + (preferredCcwOrthoRotationFlag ? 1. : -1.) * Pi/2.);
               double supplementaryPreferredOrthogonalRotation= normalizeRadians(preferredOrthogonalRotation + Pi);
            }
         }

         DataStruct* component = addComponent(newRefDes.getRefDes(),shape,partNumber,fhs,
                                    topFlag,forceThFlag,pinList,centroidData,educationMethod);
         component->getInsert()->setAngle(preferredRotation);
      }
   }
}

DataStruct* CGerberEducator::getSingleComponentCentroid(BlockStruct& componentGeometry)
{
   DataStruct* geometryCentroid = NULL;

   for (CDataListIterator centroidIterator(componentGeometry,insertTypeCentroid);centroidIterator.hasNext();)
   {
      if (geometryCentroid != NULL)
      {
         geometryCentroid = NULL;
         break;
      }

      geometryCentroid = centroidIterator.getNext();
   }

   return geometryCentroid;
}

bool CGerberEducator::findGeometryPattern(CPinDataTreeList& pinList,
   BlockStruct& componentGeometry,DataStruct** centroidData,CTMatrix& componentGeometryToPcbMatrix,bool placedTopFlag)
{
   bool retval = true;

   const double searchTolerance    = getSearchTolerance();
   //bool placedTopFlag              = isOnTopMatchTypeLayer(sourceLayerIndex);
   int topGeometryPadLayerIndex    = getLayer(ccLayerPadTop   )->getLayerIndex();
   int bottomGeometryPadLayerIndex = getLayer(ccLayerPadBottom)->getLayerIndex();
   int sourceLayerIndex            = (placedTopFlag ? getTopUnmatchedLayerIndex() : getBottomUnmatchedLayerIndex());
   int oppositeSourceLayerIndex    = getOppositeLayerIndex(sourceLayerIndex);
   bool comparePadFlag = true;

   CBasesVector basesVector;
   basesVector.transform(componentGeometryToPcbMatrix);
   double componentInsertAngleRadians = -basesVector.getRotationRadians();

   if (centroidData != NULL)
   {
      *centroidData = NULL;
      DataStruct* componentGeometryCentroid = getSingleComponentCentroid(componentGeometry);

      if (componentGeometryCentroid == NULL)
      {
         retval = false;
      }
      else
      {
         CPoint2d componentGeometryCentroidOrigin = componentGeometryCentroid->getInsert()->getOrigin2d();
         CPoint2d componentGeometryCentroidOriginOnPcb(componentGeometryCentroidOrigin);
         componentGeometryToPcbMatrix.transform(componentGeometryCentroidOriginOnPcb);

         CExtent searchExtent(componentGeometryCentroidOriginOnPcb,searchTolerance);
         CDataStructQfe* dataStructQfe = getCentroidTree(placedTopFlag).findFirst(searchExtent);

         if (dataStructQfe != NULL)
         {
            *centroidData = dataStructQfe->getObject();
         }
      }
   }

   for (CDataListIterator pinIterator(componentGeometry,getPinInsertTypeMask());pinIterator.hasNext() && retval;)
   {
      DataStruct* pin = pinIterator.getNext();
      InsertStruct* pinInsert = pin->getInsert();
      CString pinName = pinInsert->getRefname();
      double padStackInsertAngleRadians = pinInsert->getAngleRadians();
      double deltaRadians = padStackInsertAngleRadians + componentInsertAngleRadians;

      BlockStruct* padstackGeometry   = getCamCadDatabase().getBlock(pinInsert->getBlockNumber());
      DataStruct* topGeometryPad      = NULL;
      DataStruct* bottomGeometryPad   = NULL;
      double topPadRotationRadians    = 0.;
      double bottomPadRotationRadians = 0.;

      for (CDataListIterator padIterator(*padstackGeometry,insertTypeUnknown);padIterator.hasNext() && 
              (topGeometryPad == NULL || bottomGeometryPad == NULL);)
      {
         DataStruct* pad = padIterator.getNext();

         if (topGeometryPad == NULL && pad->getLayerIndex() == topGeometryPadLayerIndex)
         {
            topGeometryPad = pad;
            topPadRotationRadians = pad->getInsert()->getAngleRadians();
         }

         if (bottomGeometryPad == NULL && pad->getLayerIndex() == bottomGeometryPadLayerIndex)
         {
            bottomGeometryPad = pad;
            bottomPadRotationRadians = pad->getInsert()->getAngleRadians();
         }
      }

      CPoint2d pinOrigin = pinInsert->getOrigin2d();
      CPoint2d pinOriginOnPcb(pinOrigin);
      componentGeometryToPcbMatrix.transform(pinOriginOnPcb);

      CExtent searchExtent(pinOriginOnPcb,searchTolerance);

      CQfeApertureInsertList foundList;
      m_apertureInsertTree.search(searchExtent,foundList);
      DataStruct* topAperture    = NULL;
      DataStruct* bottomAperture = NULL;

      for (POSITION pos = foundList.GetHeadPosition();pos != NULL && (topAperture == NULL || bottomAperture == NULL);)
      {
         CQfeApertureInsert* qfeAperture = foundList.GetNext(pos);
         DataStruct* aperture = qfeAperture->getAperture();

         if (topAperture == NULL && aperture->getLayerIndex() == sourceLayerIndex)
         {
            topAperture = aperture;
         }

         if (bottomAperture == NULL && aperture->getLayerIndex() == oppositeSourceLayerIndex)
         {
            bottomAperture = aperture;
         }
      }

      retval = false;

      if (topGeometryPad != NULL && bottomGeometryPad != NULL)
      {
         if (topAperture != NULL && bottomAperture != NULL)
         {
            if (comparePadFlag && 
                 ((   topGeometryPad->getInsert()->getBlockNumber() !=    topAperture->getInsert()->getBlockNumber()) ||
                  (bottomGeometryPad->getInsert()->getBlockNumber() != bottomAperture->getInsert()->getBlockNumber())   )  )
            {
               break;
            }

            pinList.add(topAperture   , placedTopFlag);
            pinList.add(bottomAperture,!placedTopFlag);

            retval = true;
         }
      }
      else if (topGeometryPad != NULL && topAperture != NULL)
      {
         if (comparePadFlag && !aperturesEqual(m_camCadDatabase,topGeometryPad,topAperture,deltaRadians))
         {
            break;
         }

         pinList.add(topAperture,placedTopFlag);

         retval = true;
      }
      else if (bottomGeometryPad != NULL && bottomAperture != NULL)
      {
         if (comparePadFlag && !aperturesEqual(m_camCadDatabase,bottomGeometryPad,bottomAperture,deltaRadians))
         {
            break;
         }

         pinList.add(bottomAperture,!placedTopFlag);

         retval = true;
      }
   }

   return retval;
}

void CGerberEducator::addByShape(DataStruct& centroidTemplate,GerberEducationMethodTag educationMethod)
{
   CString shapeTemplate = getShape(centroidTemplate);
   CString shape;

   CDataList selectedCentroidList(false);

   for (POSITION pos = m_centroidList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* centroid = m_centroidList.GetNext(pos);

      if (centroid->getLayerIndex() == getTopUnmatchedLayerIndex()    ||
          centroid->getLayerIndex() == getBottomUnmatchedLayerIndex()    )
      {
         shape = getShape(*centroid);

         if (shape.CompareNoCase(shapeTemplate) == 0)
         {
            selectedCentroidList.AddTail(centroid);
         }
      }
   }

   addComponentsByCentroid(centroidTemplate,selectedCentroidList,false,educationMethod);
}

void CGerberEducator::addByPartNumber(DataStruct& centroidTemplate)
{
   CString partNumberTemplate = getPartNumber(centroidTemplate);
   CString partNumber;

   CDataList selectedCentroidList(false);

   for (POSITION pos = m_centroidList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* centroid = m_centroidList.GetNext(pos);

      if (centroid->getLayerIndex() == getTopUnmatchedLayerIndex()    ||
          centroid->getLayerIndex() == getBottomUnmatchedLayerIndex()    )
      {
         partNumber = getPartNumber(*centroid);

         if (partNumber.CompareNoCase(partNumberTemplate) == 0)
         {
            selectedCentroidList.AddTail(centroid);
         }
      }
   }

   addComponentsByCentroid(centroidTemplate,selectedCentroidList,false,gerberEducationMethodByPartNumber);
}

void CGerberEducator::addByPinPattern(const CString& refDes,const CString& shape,
   const CString& partNumber,bool topFlag,bool forceThFlag,double fhs,double rotationRadians,bool preferredCcwOrthoRotationFlag,
   CPinDataTreeList& pinList,DataStruct* centroidData,bool orthogonalFlag)
{
   if (pinList.GetCount() > 0)
   {
      CPoint2d componentOrigin = pinList.getPinOriginExtentsCenter();

      DataStruct* component = addComponent(refDes,shape,partNumber,fhs,topFlag,forceThFlag,pinList,
                                 centroidData,gerberEducationMethodByPinPattern);

      BlockStruct* componentGeometry = 
         m_camCadDatabase.getBlock(component->getInsert()->getBlockNumber());

      if (pinList.GetCount() > 1)
      {
         addComponents(refDes,shape,partNumber,fhs,*component,centroidData != NULL,
            orthogonalFlag,forceThFlag,gerberEducationMethodByPinPattern);
      }
      else if (pinList.GetCount() == 1)
      {
         addSinglePinComponents(refDes,shape,partNumber,fhs,*componentGeometry,centroidData != NULL,rotationRadians,
            preferredCcwOrthoRotationFlag,orthogonalFlag,forceThFlag,gerberEducationMethodByPinPattern);
      }
   }
}

void CGerberEducator::addAutomatically(CGerberEducatorCentroidList& centroidList)
{
   CByteArray pinMask;
   pinMask.SetSize(0,20);
   CGerberEducatorCentroid* unmatchedCentroid = NULL;

   for (POSITION pos = centroidList.GetHeadPosition();pos != NULL;)
   {
      CGerberEducatorCentroid* centroid = centroidList.GetNext(pos);
      centroid->updatePinMask(pinMask);

      if (unmatchedCentroid == NULL)
      {
         int unmatchedCentroidLayerIndex = centroid->getCentroid().getLayerIndex();

         if (unmatchedCentroidLayerIndex == getTopUnmatchedLayerIndex()    ||
             unmatchedCentroidLayerIndex == getBottomUnmatchedLayerIndex()    )
         {
            unmatchedCentroid = centroid;
         }
      }
   }

   if (unmatchedCentroid != NULL)
   {
      CPinDataTreeList matchedPinList(getSearchTolerance());
      CQfeApertureInsertList& pinList = unmatchedCentroid->getPinList();

      int pinIndex = 0;

      for (POSITION pinPos = pinList.GetHeadPosition();pinPos != NULL;pinIndex++)
      {
         CQfeApertureInsert* pin = pinList.GetNext(pinPos);

         if (pinMask.GetAt(pinIndex) == PinsMatch)
         {
            matchedPinList.add(pin->getAperture());
         }
      }

      if (! matchedPinList.IsEmpty())
      {
         addComponentByCentroid(unmatchedCentroid->getCentroid(),matchedPinList,false,gerberEducationMethodAutomatic);
         addByShape(unmatchedCentroid->getCentroid(),gerberEducationMethodAutomatic);
      }
   }
}

void CGerberEducator::evaluatePinClaims(CGerberEducatorCentroidList& centroidList,GerberEducatorPackageIdentifierTag packageIdentifierTag)
{
   //m_apertureInsertTree.assertValid();
   int centroidCount = centroidList.GetCount();

   const double componentSearchTolerance = m_camCadDatabase.convertToPageUnits(pageUnitsInches,2.0);
   const double pinSearchTolerance = m_camCadDatabase.convertToPageUnits(pageUnitsInches,.001);

   // find first unmatched centroid in list
   CGerberEducatorCentroid* unmatchedCentroid = NULL;
   int unmatchedCentroidLayerIndex;

   for (POSITION pos = centroidList.GetHeadPosition();pos != NULL;)
   {
      unmatchedCentroid = centroidList.GetNext(pos);
      unmatchedCentroidLayerIndex = unmatchedCentroid->getCentroid().getLayerIndex();

      if (unmatchedCentroidLayerIndex == getTopUnmatchedLayerIndex()    ||
          unmatchedCentroidLayerIndex == getBottomUnmatchedLayerIndex()    )
      {
         break;
      }

      unmatchedCentroid = NULL;
   }

   if (unmatchedCentroid != NULL)
   {
      CString refDes            = getRefDes(unmatchedCentroid->getCentroid());
      CString packageIdentifier = getPackageIdentifier(unmatchedCentroid->getCentroid(),packageIdentifierTag);

      CDataList pinList(false);

      InsertStruct* unmatchedCentroidInsert = unmatchedCentroid->getCentroid().getInsert();

      CExtent searchExtent(unmatchedCentroidInsert->getOriginX() - componentSearchTolerance,
                           unmatchedCentroidInsert->getOriginY() - componentSearchTolerance,
                           unmatchedCentroidInsert->getOriginX() + componentSearchTolerance,
                           unmatchedCentroidInsert->getOriginY() + componentSearchTolerance);

      CQfeApertureInsertList foundList;
      m_apertureInsertTree.search(searchExtent,foundList);
      DataStruct* pin = NULL;
      int matchingPinCount = 0;

      for (POSITION aperturePos = foundList.GetHeadPosition();aperturePos != NULL;)
      {
         CQfeApertureInsert* apertureInsert = foundList.GetNext(aperturePos);
         DataStruct* aperture = apertureInsert->getAperture();

         if (aperture->getLayerIndex() == unmatchedCentroidLayerIndex)
         {
            bool missingPinFlag = false;
            CPoint2d apertureOrigin = aperture->getInsert()->getOrigin2d();
            CPoint2d localApertureOrigin = unmatchedCentroid->transformPcbToLocal(apertureOrigin);
            CQfeApertureInsertList matchingPinList;

            for (POSITION centroidPos = centroidList.GetHeadPosition();centroidPos != NULL;)
            {
               CGerberEducatorCentroid* centroid = centroidList.GetNext(centroidPos);
               CPoint2d pcbApertureOrigin = centroid->transformLocalToPcb(localApertureOrigin);

               int centroidLayerIndex = centroid->getCentroid().getLayerIndex();

               CExtent searchExtent(pcbApertureOrigin.x - pinSearchTolerance,
                                    pcbApertureOrigin.y - pinSearchTolerance,
                                    pcbApertureOrigin.x + pinSearchTolerance,
                                    pcbApertureOrigin.y + pinSearchTolerance);

               CQfeApertureInsertList foundPinList;

               m_apertureInsertTree.search(searchExtent,foundPinList);

               //foundPinList.assertValid();

               for (POSITION pinPos = foundPinList.GetHeadPosition();;)
               {
                  if (pinPos == NULL)
                  {
                     missingPinFlag = true;
                     break;
                  }

                  CQfeApertureInsert* pinApertureInsert = foundPinList.GetNext(pinPos);

                  //pinApertureInsert->assertValid();

                  if (pinApertureInsert->getAperture()->getLayerIndex() == centroidLayerIndex)
                  {
                     matchingPinCount++;
                     matchingPinList.AddTail(pinApertureInsert);
                     break;
                  }
               }
            }

            if (!missingPinFlag && centroidList.GetCount() == matchingPinList.GetCount())
            {
               POSITION centroidPos = centroidList.GetHeadPosition();
               POSITION pinPos      = matchingPinList.GetHeadPosition();

               while (centroidPos != NULL && pinPos != NULL)
               {
                  CGerberEducatorCentroid* centroid = centroidList.GetNext(centroidPos);
                  CQfeApertureInsert* pinApertureInsert = matchingPinList.GetNext(pinPos);

                  centroid->addPin(pinApertureInsert);
               }

               //pinList.AddTail(apertureInsert->getAperture());
            }
         }
      }

      if (matchingPinCount > 0)
      {
         for (POSITION centroidPos = centroidList.GetHeadPosition();centroidPos != NULL;)
         {
            CGerberEducatorCentroid* centroid = centroidList.GetNext(centroidPos);
            centroid->evaluatePinClaims(*this,centroidCount);
         }
      }

      //if (! pinList.IsEmpty())
      //{
      //   addComponent(unmatchedCentroid->getCentroid(),pinList);
      //   addByShape(unmatchedCentroid->getCentroid());
      //}
   }
}

void CGerberEducator::addAutomatically(GerberEducatorPackageIdentifierTag packageIdentifierTag)
{
   //m_apertureInsertTree.assertValid();

   CString packageIdentifier;

   // map centroids by shape
   CGerberEducatorCentroidListMap centroidListMap;

   for (POSITION pos = m_centroidList.GetHeadPosition();pos != NULL;)
   {
      DataStruct* centroidData = m_centroidList.GetNext(pos);
      bool mirrorFlag = (centroidData->getLayerIndex() == getBottomUnmatchedLayerIndex() ||
                         centroidData->getLayerIndex() == getBottomMatchedLayerIndex()      );
      CGerberEducatorCentroid* centroid = new CGerberEducatorCentroid(*centroidData,mirrorFlag);

      packageIdentifier = getPackageIdentifier(*centroidData,packageIdentifierTag);

      CGerberEducatorCentroidList* centroidList = NULL;
      centroidListMap.Lookup(packageIdentifier,centroidList);

      if (centroidList == NULL)
      {
         centroidList = new CGerberEducatorCentroidList;
         centroidListMap.SetAt(packageIdentifier,centroidList);
      }

      centroidList->AddTail(centroid);
   }

   // sort shape list by count
   CGerberEducatorCentroidListArrayArray centroidListArrayArray;

   for (POSITION pos = centroidListMap.GetStartPosition();pos != NULL;)
   {
      CGerberEducatorCentroidList* centroidList;
      centroidListMap.GetNextAssoc(pos,packageIdentifier,centroidList);

      int count = centroidList->GetCount();
      CGerberEducatorCentroidListArray* centroidListArray = NULL;

      if (count < centroidListArrayArray.GetCount())
      {
         centroidListArray = centroidListArrayArray.GetAt(count);
      }

      if (centroidListArray == NULL)
      {
         centroidListArray = new CGerberEducatorCentroidListArray(false);
         centroidListArrayArray.SetAtGrow(count,centroidListArray);
      }

      centroidListArray->Add(centroidList);      
   }

   //m_apertureInsertTree.assertValid();

   // evaluate pin claims in shape order by decreasing frequency
   for (int index = centroidListArrayArray.GetSize() - 1;index > 0;index--)
   {
      CGerberEducatorCentroidListArray* centroidListArray = centroidListArrayArray.GetAt(index);

      if (centroidListArray != NULL)
      {
         for (int index = 0;index < centroidListArray->GetSize();index++)
         {
            CGerberEducatorCentroidList* centroidList = centroidListArray->GetAt(index);
            evaluatePinClaims(*centroidList,packageIdentifierTag);
         }
      }
   }

   // grant pin claims in shape order by decreasing frequency
   for (int index = centroidListArrayArray.GetSize() - 1;index > 0;index--)
   {
      CGerberEducatorCentroidListArray* centroidListArray = centroidListArrayArray.GetAt(index);

      if (centroidListArray != NULL)
      {
         for (int index = 0;index < centroidListArray->GetSize();index++)
         {
            CGerberEducatorCentroidList* centroidList = centroidListArray->GetAt(index);
            addAutomatically(*centroidList);
         }
      }
   }

   getCamCadDoc().OnRedraw();
}

void CGerberEducator::deleteComponent(DataStruct* component,POSITION componentPos)
{
   const double searchTolerance = 2. * getSearchTolerance();
   InsertStruct& componentInsert = *(component->getInsert());
   bool componentPlacedTopFlag = ! component->getInsert()->getPlacedBottom();
   BlockStruct& geometry = *(getCamCadDoc().getBlockAt(componentInsert.getBlockNumber()));

   CString refDes = componentInsert.getRefname();

   CTMatrix componentGeometryToPcbMatrix;
   componentGeometryToPcbMatrix.translateCtm(componentInsert.getOriginX(),componentInsert.getOriginY());
   componentGeometryToPcbMatrix.rotateRadiansCtm(componentInsert.getAngle());
   componentGeometryToPcbMatrix.scaleCtm(componentInsert.getGraphicMirrored() ? -1. : 1.,1.);

   DataStruct* componentGeometryCentroid = getSingleComponentCentroid(geometry);

   if (componentGeometryCentroid != NULL)
   {
      CPoint2d componentGeometryCentroidOrigin = componentGeometryCentroid->getInsert()->getOrigin2d();
      CPoint2d componentGeometryCentroidOriginOnPcb(componentGeometryCentroidOrigin);
      componentGeometryToPcbMatrix.transform(componentGeometryCentroidOriginOnPcb);

      CExtent searchExtent(componentGeometryCentroidOriginOnPcb,searchTolerance);
      CDataStructQfe* qfeCentroid = getCentroidTree(componentPlacedTopFlag).findFirst(searchExtent);

      if (qfeCentroid != NULL)
      {
         DataStruct* centroid = qfeCentroid->getObject();
         int matchedLayerIndex   = (componentPlacedTopFlag ? getTopMatchedLayerIndex()   : getBottomMatchedLayerIndex());
         int unmatchedLayerIndex = (componentPlacedTopFlag ? getTopUnmatchedLayerIndex() : getBottomUnmatchedLayerIndex());

         if (centroid->getLayerIndex() == matchedLayerIndex)
         {
            centroid->setLayerIndex(unmatchedLayerIndex);

            m_gerberEducatorProgressSummary.unmatchCentroid(componentPlacedTopFlag);

            if (getDialog().isSurfaceDisplayed(componentPlacedTopFlag))
            {
               addToRedrawList(centroid);
            }
         }
      }
   }

   for (CDataListIterator pinIterator(geometry,m_pinInsertTypeMask);pinIterator.hasNext();)
   {
      DataStruct* padStack = pinIterator.getNext();
      InsertStruct* padStackInsert = padStack->getInsert();
      DataStruct* topSurfacePad;
      DataStruct* bottomSurfacePad;

      if (componentPlacedTopFlag)
      {
         getPadstackSurfacePads(*padStack,topSurfacePad,bottomSurfacePad);
      }
      else
      {
         getPadstackSurfacePads(*padStack,bottomSurfacePad,topSurfacePad);
      }

      DataStruct* topAperture = NULL;
      DataStruct* bottomAperture = NULL;
      CPoint2d padStackOrigin = padStackInsert->getOrigin2d();

      componentGeometryToPcbMatrix.transform(padStackOrigin);

      CExtent searchExtent(padStackOrigin,searchTolerance);
      CQfeApertureInsertList foundList;
      m_apertureInsertTree.search(searchExtent,foundList);

      for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
      {
         CQfeApertureInsert* qfeAperture = foundList.GetNext(foundPos);

         if (qfeAperture != NULL)
         {
            DataStruct* aperture = qfeAperture->getAperture();

            if (aperture->getLayerIndex() == getTopMatchedLayerIndex() && topSurfacePad != NULL && topAperture == NULL)
            {
               aperture->setLayerIndex(getTopUnmatchedLayerIndex());

               m_gerberEducatorProgressSummary.unmatchPad(true);
               topAperture = aperture;
            }
            else if (aperture->getLayerIndex() == getBottomMatchedLayerIndex() && bottomSurfacePad != NULL && bottomAperture == NULL)
            {
               aperture->setLayerIndex(getBottomUnmatchedLayerIndex());

               m_gerberEducatorProgressSummary.unmatchPad(false);
               bottomAperture = aperture;
            }
         }
      }

      if (topAperture != NULL && bottomAperture != NULL)
      {
         addToRedrawList(getDialog().isWorkingSurfaceTop() ? topAperture : bottomAperture);
      }
      else if (topAperture != NULL && getDialog().isSurfaceDisplayed(true))
      {
         addToRedrawList(topAperture);
      }
      else if (bottomAperture != NULL && getDialog().isSurfaceDisplayed(false))
      {
         addToRedrawList(bottomAperture);
      }
   }

   DataStruct* centroid;

   if (m_centroidMap.Lookup(refDes,centroid))
   {
      if (centroid->getLayerIndex() == getTopMatchedLayerIndex())
      {
         centroid->setLayerIndex(getTopUnmatchedLayerIndex());

         if (getDialog().isSurfaceDisplayed(true))
         {
            addToRedrawList(centroid);
         }

         m_gerberEducatorProgressSummary.unmatchCentroid(true);
      }
      else if (centroid->getLayerIndex() == getBottomMatchedLayerIndex())
      {
         centroid->setLayerIndex(getBottomUnmatchedLayerIndex());

         if (getDialog().isSurfaceDisplayed(false))
         {
            addToRedrawList(centroid);
         }

         m_gerberEducatorProgressSummary.unmatchCentroid(false);
      }
   }

   m_gerberEducatorProgressSummary.removeComponent(*component);

   BlockStruct* pcbBlock = m_pcbFile->getBlock();

   DataStruct* componentCopy = getCamCadData().getNewDataStruct(*component);
   componentCopy->setColorOverride(true);
   componentCopy->setOverrideColor(getCamCadDoc().getSettings().Foregrnd);
   //addToEraseList(component);

   RemoveOneEntityFromDataList(&(getCamCadDoc()),&(pcbBlock->getDataList()),component,componentPos);
   //if (pcbBlock->getDataList().GetAt(componentPos) == component)
   //{
   //   pcbBlock->getDataList().RemoveAt(componentPos);
   //   delete component;
   //}

   drawEraseList(componentCopy);

   m_camCadDatabase.deletePins(m_pcbFile,refDes);
   m_components.RemoveKey(refDes);

   delete componentCopy;
}

void CGerberEducator::deleteComponents(bool byRefDes,bool byShape,bool byPartNumber,bool byGeometry)
{
   CWaitCursor waitCursor;

   CSelectList& selectList = getCamCadDoc().SelectList;
   CMapStringToString refDesMap;
   CMapStringToString shapeMap;
   CMapStringToString partNumberMap;
   CMapStringToString geometryNameMap;

   refDesMap.InitHashTable(nextPrime2n(400));
   shapeMap.InitHashTable(nextPrime2n(100));
   partNumberMap.InitHashTable(nextPrime2n(100));
   geometryNameMap.InitHashTable(nextPrime2n(100));

   for (POSITION pos = selectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* selectStruct = selectList.GetNext(pos);
      DataStruct* data = selectStruct->getData();

      if (data->getDataType() != dataTypeInsert)
      {
         continue;
      }

      InsertStruct* insert = data->getInsert();
      BlockStruct* block = getCamCadDoc().getBlockAt(insert->getBlockNumber());
      int layerIndex = data->getLayerIndex();
      DataStruct* componentData = NULL;
      CString refDes,shape,partNumber,machineFeeder,machine,feeder,geometryName;
      bool foundComponentFlag = false;

      if (block->getBlockType() == blockTypeCentroid)
      {
         if (layerIndex == getTopMatchedLayerIndex()    ||
             layerIndex == getBottomMatchedLayerIndex()    )
         {
            m_camCadDatabase.getAttributeStringValue(refDes       ,getAttributeMap(data),m_refDesKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(shape        ,getAttributeMap(data),m_shapeKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(partNumber   ,getAttributeMap(data),m_partNumberKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(machineFeeder,getAttributeMap(data),m_machineFeederKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(machine      ,getAttributeMap(data),m_machineKeywordIndex);
            m_camCadDatabase.getAttributeStringValue(feeder       ,getAttributeMap(data),m_feederKeywordIndex);
            geometryName.Empty();

            foundComponentFlag = true;
         }
      }
      else if (insert->getInsertType() == insertTypePcbComponent        ||
               insert->getInsertType() == insertTypeFiducial            ||
               insert->getInsertType() == insertTypeMechanicalComponent ||
               insert->getInsertType() == insertTypeDrillTool             )
      {
         refDes = insert->getRefname();
         geometryName = block->getName();
         shape = geometryName;
         m_camCadDatabase.getAttributeStringValue(partNumber,getAttributeMap(data),m_partNumberKeywordIndex);

         foundComponentFlag = true;
      }

      if (foundComponentFlag)
      {
         if (byRefDes)
         {
            refDesMap.SetAt(refDes,"");
         }

         if (byShape)
         {
            shapeMap.SetAt(shape,"");
         }

         if (byPartNumber)
         {
            partNumberMap.SetAt(partNumber,"");
         }

         if (byGeometry)
         {
            geometryNameMap.SetAt(geometryName,"");
         }
      }
   }

   BlockStruct* pcbBlock = m_pcbFile->getBlock();

   if (byShape || byPartNumber || byGeometry)
   {
      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         InsertStruct* insert = data->getInsert();
         BlockStruct* block = getCamCadDoc().getBlockAt(insert->getBlockNumber());

         if (insert->getInsertType() == insertTypePcbComponent        ||
             insert->getInsertType() == insertTypeFiducial            ||
             insert->getInsertType() == insertTypeMechanicalComponent ||
             insert->getInsertType() == insertTypeDrillTool             )
         {
            CString refDes,shape,partNumber,geometryName,value;

            refDes = insert->getRefname();
            geometryName = block->getName();
            shape = geometryName;
            m_camCadDatabase.getAttributeStringValue(partNumber,getAttributeMap(data),m_partNumberKeywordIndex);

            if (shapeMap.Lookup(shape,value))
            {
               refDesMap.SetAt(refDes,"");
            }

            if (partNumberMap.Lookup(partNumber,value))
            {
               refDesMap.SetAt(refDes,"");
            }

            if (geometryNameMap.Lookup(geometryName,value))
            {
               refDesMap.SetAt(refDes,"");
            }
         }
      }
   }

   for (POSITION pos = pcbBlock->getHeadDataPosition();pos != NULL;)
   {
      POSITION componentPos = pos;
      DataStruct* component = pcbBlock->getNextData(pos);

      if (component->getDataType() != dataTypeInsert)
      {
         continue;
      }

      InsertStruct* insert = component->getInsert();

      if (insert->getInsertType() == insertTypePcbComponent        ||
          insert->getInsertType() == insertTypeFiducial            ||
          insert->getInsertType() == insertTypeMechanicalComponent ||
          insert->getInsertType() == insertTypeDrillTool             )
      {
         CString refDes,value;

         refDes = insert->getRefname();

         if (refDesMap.Lookup(refDes,value))
         {
            deleteComponent(component,componentPos);
         }
      }
   }

   //getCamCadDoc().OnClearSelected();
   drawRedrawList();
   //getCamCadDoc().OnRedraw();
}

CString CGerberEducator::getToolName()
{
   return "Gerber Educator";
}

CString CGerberEducator::getEducationMethodAttributeName()
{
   return "EducationMethod";
}

CString CGerberEducator::getEducationSurfaceAttributeName()
{
   return ATT_GerberEducatorSurface;
}

LayerStruct* CGerberEducator::getLayer(CamCadLayerTag layerTag)
{
   return getCamCadDatabase().getLayer(layerTag);
}

//_____________________________________________________________________________
CGerberCentroidMergeTool::CGerberCentroidMergeTool(CCEtoODBDoc& camCadDoc) :
   CGerberEducator(camCadDoc)
{
}

CGerberCentroidMergeTool::~CGerberCentroidMergeTool()
{
}

EducatorFunctionalityTag CGerberCentroidMergeTool::getEducatorFunctionality()
{
   return educatorFunctionalityBasic;
}

bool CGerberCentroidMergeTool::getEnableCentroidEditingFlag()
{
   return false;
}

CString CGerberCentroidMergeTool::getToolName()
{
   return "Gerber/Centroid Merge Tool";
}

CString CGerberCentroidMergeTool::getEducationMethodAttributeName()
{
   return "MergeToolMethod";
}

CString CGerberCentroidMergeTool::getEducationSurfaceAttributeName()
{
   return ATT_MergeToolSurface;
}

LayerStruct* CGerberCentroidMergeTool::getLayer(CamCadLayerTag layerTag)
{
   LayerStruct* layer = NULL;

   if (!getCamCadDatabase().isLayerDefined(layerTag))
   {
      LayerStruct* topLayer    = NULL;
      LayerStruct* bottomLayer = NULL;

      switch (layerTag)
      {
         case ccLayerEducatorCentroidTop:   
         case ccLayerEducatorCentroidBottom:
            topLayer    = getCamCadDatabase().getLayer(ccLayerEducatorCentroidTop);
            bottomLayer = getCamCadDatabase().getLayer(ccLayerEducatorCentroidBottom);

            topLayer->setName("Merge Tool Centroid Top");
            bottomLayer->setName("Merge Tool Centroid Bottom");

            break;
         case ccLayerEducatorMatchedTop:   
         case ccLayerEducatorMatchedBottom:
            topLayer    = getCamCadDatabase().getLayer(ccLayerEducatorMatchedTop);
            bottomLayer = getCamCadDatabase().getLayer(ccLayerEducatorMatchedBottom);

            topLayer->setName("Merge Tool Matched Top");
            bottomLayer->setName("Merge Tool Matched Bottom");

            break;
         case ccLayerEducatorUnmatchedTop:   
         case ccLayerEducatorUnmatchedBottom:
            topLayer    = getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedTop);
            bottomLayer = getCamCadDatabase().getLayer(ccLayerEducatorUnmatchedBottom);

            topLayer->setName("Merge Tool Unmatched Top");
            bottomLayer->setName("Merge Tool Unmatched Bottom");

            break;
      }

      switch (layerTag)
      {
         case ccLayerEducatorCentroidTop:      layer = topLayer;     break;
         case ccLayerEducatorCentroidBottom:   layer = bottomLayer;  break;
         case ccLayerEducatorMatchedTop:       layer = topLayer;     break;  
         case ccLayerEducatorMatchedBottom:    layer = bottomLayer;  break;
         case ccLayerEducatorUnmatchedTop:     layer = topLayer;     break;
         case ccLayerEducatorUnmatchedBottom:  layer = bottomLayer;  break;
      }
   }

   if (layer == NULL)
   {
      layer = getCamCadDatabase().getLayer(layerTag);
   }

   return getCamCadDatabase().getLayer(layerTag);
}

//_____________________________________________________________________________
CGerberEducatorGeometryCreator::CGerberEducatorGeometryCreator(CCEtoODBDoc& camCadDoc)
: CGerberEducator(camCadDoc)
{
}

CGerberEducatorGeometryCreator::CGerberEducatorGeometryCreator(CGerberEducatorUi& gerberEducatorUi)
: CGerberEducator(gerberEducatorUi.getCamCadDoc())
{
}

CGerberEducatorGeometryCreator::~CGerberEducatorGeometryCreator()
{
}

CGerberEducatorBaseDialog& CGerberEducatorGeometryCreator::getDialog()
{
   if (m_dialog == NULL)
   {
      m_dialog = new CGerberEducatorCreateGeometryDialog(*this);
      m_dialog->setGerberEducatorUi(getGerberEducatorUi());
      m_dialog->Create(IDD_GerberEducatorCreateGeometryDialog);
   }

   return *m_dialog;
}

EducatorFunctionalityTag CGerberEducatorGeometryCreator::getEducatorFunctionality()
{
   return educatorFunctionalityCreateGeometry;
}

bool CGerberEducatorGeometryCreator::getEnableCentroidEditingFlag()
{
   return false;
}

CString CGerberEducatorGeometryCreator::getToolName()
{
   return "Gerber Educator Create Geometries";
}

void CGerberEducatorGeometryCreator::getDataSourceAndDataType(LayerStruct& layer,CString& dataSource,CString& dataType)
{
   m_camCadDatabase.getAttributeStringValue(dataSource,layer.attributes(),
      getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource));

   m_camCadDatabase.getAttributeStringValue(dataType,layer.attributes(),
      getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType));
}

bool CGerberEducatorGeometryCreator::isTopGerberLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Gerber") == 0 && dataType.CompareNoCase(QDataTypeTopPad) == 0);

   return retval;
}

bool CGerberEducatorGeometryCreator::isBottomGerberLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Gerber") == 0 && dataType.CompareNoCase(QDataTypeBottomPad) == 0);

   return retval;
}

bool CGerberEducatorGeometryCreator::isTopSilkScreenLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Gerber") == 0 && dataType.CompareNoCase(QDataTypeTopSilkscreen) == 0);

   return retval;
}

bool CGerberEducatorGeometryCreator::isBottomSilkScreenLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Gerber") == 0 && dataType.CompareNoCase(QDataTypeBottomSilkscreen) == 0);

   return retval;
}

bool CGerberEducatorGeometryCreator::isTopCentroidLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Centroid") == 0 && dataType.CompareNoCase("Top") == 0);

   return retval;
}

bool CGerberEducatorGeometryCreator::isBottomCentroidLayer(LayerStruct& layer)
{
   CString dataSource,dataType;
   getDataSourceAndDataType(layer,dataSource,dataType);

   bool retval = (dataSource.CompareNoCase("Centroid") == 0 && dataType.CompareNoCase("Bottom") == 0);

   return retval;
}

#endif  // EnableGerberEducator
