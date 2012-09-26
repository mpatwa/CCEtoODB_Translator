// $Header: /CAMCAD/DcaLib/DcaInsert.cpp 6     6/05/07 4:57p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaInsert.h"
#include "DcaInsertType.h"
#include "DcaPoint2d.h"
#include "DcaBlock.h"
#include "DcaCamCadData.h"
#include "DcaTMatrix.h"
#include "DcaLib.h"
#include "DcaBasesVector.h"
#include "DcaWriteFormat.h"

InsertTypeTag getDefaultInsertTypeForBlockType(BlockTypeTag blockType)
{
   InsertTypeTag insertType = insertTypeUnknown;

   switch (blockType)
   {
      case blockTypePcb:                insertType = insertTypePcb;                  break;
      case blockTypePcbComponent:       insertType = insertTypePcbComponent;         break;
      case blockTypeMechComponent:      insertType = insertTypeMechanicalComponent;  break;
      case blockTypeGenericComponent:   insertType = insertTypeGenericComponent;     break;
      case blockTypeFiducial:           insertType = insertTypeFiducial;             break;
      case blockTypeTooling:            insertType = insertTypeDrillTool;            break;
      case blockTypeTestPoint:          insertType = insertTypeTestPoint;            break;
      case blockTypeSymbol:             insertType = insertTypeSymbol;               break;
      case blockTypeGatePort:           insertType = insertTypePortInstance;         break;
      case blockTypeDrillHole:          insertType = insertTypeDrillHole;            break;
      case blockTypeTestProbe:          insertType = insertTypeTestProbe;            break;
      case blockTypeCentroid:           insertType = insertTypeCentroid;             break;
      case blockTypeGluePoint:          insertType = insertTypeGluePoint;            break;
      case blockTypeAccessPoint:        insertType = insertTypeTestAccessPoint;      break;
      case blockTypeDrcMarker:          insertType = insertTypeDrcMarker;            break;
      case blockTypeTestPad:            insertType = insertTypeTestPad;              break;
      case blockTypeSchemJunction:      insertType = insertTypeSchematicJunction;    break;
      case blockTypeRejectMark:         insertType = insertTypeRejectMark;           break;
      case blockTypeDie:                insertType = insertTypeDie;                  break;
      case blockTypeDiePin:             insertType = insertTypeDiePin;               break;
      case blockTypeBondWire:           insertType = insertTypeBondWire;             break;
      case blockTypeBondPad:            insertType = insertTypeBondPad;              break;
   }

   return insertType;
}

//_____________________________________________________________________________
ColorStruct::ColorStruct()
{  
   red         = 0;
   green       = 0;
   blue        = 0;
   On          = 0;
   Before      = 0;
   Transparent = 0;
   Hatched     = 0;
   dummy       = 0;
}

ColorStruct::ColorStruct(const ColorStruct& other)
{
   red         = other.red;
   green       = other.green;
   blue        = other.blue;
   On          = other.On;
   Before      = other.Before;
   Transparent = other.Transparent;
   Hatched     = other.Hatched;
   dummy       = other.dummy;
}

//_____________________________________________________________________________
InsertStruct::InsertStruct()
{
   initializePosition();

   m_placedBottom = 0;
   m_blockNumber  = 0;
   m_insertType   = insertTypeUnknown;
   m_refname      = NULL;
   m_fillStyle    = HS_NOT_SET;
}

InsertStruct::InsertStruct(const InsertStruct& other)
{
   m_refname = NULL;

   *this = other;
}

InsertStruct& InsertStruct::operator=(const InsertStruct& other)
{
   if (&other != this)
   {
      m_origin       = other.m_origin;
      m_shading      = other.m_shading;
      m_angleRadians = other.m_angleRadians;
      m_mirrorFlags  = other.m_mirrorFlags;
      m_placedBottom = other.m_placedBottom;
      m_scale        = other.m_scale;
      m_blockNumber  = other.m_blockNumber;
      m_insertType   = other.m_insertType;
      m_fillStyle    = other.m_fillStyle;

      delete m_refname;
      m_refname = NULL;

      if (other.m_refname != NULL)
      {
         m_refname = new CString(*(other.m_refname));
      }
   }

   return *this;
}

InsertStruct::~InsertStruct()
{
   delete m_refname;
}

double InsertStruct::getAngleDegrees() const
{
   return radiansToDegrees(m_angleRadians);
}

void InsertStruct::setAngleDegrees(double degrees)
{
   m_angleRadians = (DbUnit)degreesToRadians(degrees);
}

CPoint2d InsertStruct::getOrigin2d() const
{
   return CPoint2d(m_origin.x,m_origin.y);
}

void InsertStruct::setOrigin(const CPoint2d& origin)
{
   m_origin.x = (DbUnit)origin.x;
   m_origin.y = (DbUnit)origin.y;
}

void InsertStruct::setInsertType(int insertType)
{
   m_insertType = ((insertType >= insertTypeLowerBound && insertType <= insertTypeUpperBound) ? (InsertTypeTag)insertType : insertTypeUndefined);
}

void InsertStruct::setLayerMirrorFlag(bool flag)
{
   if (flag)
   {
      m_mirrorFlags |= MIRROR_LAYERS;
   }
   else
   {
      m_mirrorFlags &= ~MIRROR_LAYERS;
   }
}

void InsertStruct::setGraphicsMirrorFlag(bool flag)
{
   if (flag)
   {
      m_mirrorFlags |= MIRROR_FLIP;
   }
   else
   {
      m_mirrorFlags &= ~MIRROR_FLIP;
   }
}

void InsertStruct::initializePosition()
{
   m_origin.x     = 0.f;
   m_origin.y     = 0.f;
   m_angleRadians = 0.f;
   m_mirrorFlags  = 0;
   m_scale        = 1.f;
}

const CString& InsertStruct::getRefname() const
{ 
   if (m_refname == NULL)
   {
      m_refname = new CString();
   }

   return *m_refname; 
}

CString& InsertStruct::getRefnameRef() 
{ 
   if (m_refname == NULL)
   {
      m_refname = new CString();
   }

   return *m_refname; 
}

CString InsertStruct::getSortableRefDes(const CString& refDes)
{
   const char* p = refDes;
   CString refAlpha;
   CString refNumeric;
   CString refSuffix;

   while (*p != '\0')
   {
      if (isdigit(*p))
      {
         break;
      }

      refAlpha += *(p++);
   }

   while (*p != '\0')
   {
      if (!isdigit(*p))
      {
         break;
      }

      refNumeric += *(p++);
   }

   refSuffix = p;

   CString leader("00000000");
   CString trailer("        ");

   refAlpha   += trailer;
   refNumeric  = leader + refNumeric;
   refSuffix  += trailer;

   CString sortableRefDes = refAlpha.Left(8) + refNumeric.Right(8) + refSuffix.Left(8);

   // Case dts0100408077 - The general construction of sortableRefDes as above causes 
   // collisions for refnames like C3 and C03, they both result in the same value.
   // Some features build component maps based on the sortableRefDes, and they are dropping entities when
   // such name collisions occur. So here we tack the original refname onto the end, thus making them
   // unique again. This keeps intact the basic overall "numerical" sorting portion of the game.
   sortableRefDes += "|" + refDes;

   return sortableRefDes;
}

CString InsertStruct::getSortableRefDes() const
{
   CString sortableRefDes;

   if (m_refname != NULL)
   {
      sortableRefDes = getSortableRefDes(*m_refname);
   }

   return sortableRefDes;
}

//
//char* InsertStruct::getRefnamePointer() 
//{ 
//   char* retval = NULL;
//
//   if (m_refname != NULL)
//   {
//      retval = m_refname->GetBuffer();
//   }
//
//   return retval; 
//}

void InsertStruct::setRefname(char* refname)
{
   if (refname == NULL)
   {
      delete m_refname;
      m_refname = NULL;
   }
   else if (m_refname == NULL)
   {
      m_refname = new CString(refname);
   }
   else
   {
      *m_refname = refname;
   }

   free(refname);
}

void InsertStruct::setRefname(const CString& refname)
{
   if (m_refname == NULL)
   {
      m_refname = new CString(refname);
   }
   else
   {
      *m_refname = refname;
   }
}

bool InsertStruct::getCentroidLocation(CCamCadData& camCadData, CPoint2d &location) const
{
   BlockStruct* block = camCadData.getBlock(m_blockNumber); 
   DataStruct* centroidData = block->GetCentroidData();

   if (centroidData != NULL)
   {
      //DTransform xform(m_origin.x, m_origin.y, m_scale, m_angleRadians, m_mirrorFlags);
      //Point2 p;
      //p.x = centroidData->getInsert()->getOriginX();
      //p.y = centroidData->getInsert()->getOriginY();

      //xform.TransformPoint(&p);

      //location.x = p.x;
      //location.y = p.y;

      CTMatrix matrix = getTMatrix();

      location = centroidData->getInsert()->getOrigin2d();
      location.transform(matrix);

      return true;
   } 

   return false;
}

bool InsertStruct::getCentroidRotation(CCamCadData& camCadData, double &rotDegrees) const
{
   BlockStruct* block = camCadData.getBlock(m_blockNumber); 
   DataStruct* centroidData = block->GetCentroidData();

   if (centroidData != NULL)
   {
      if (this->getPlacedTop())
         rotDegrees = this->getAngleDegrees() + centroidData->getInsert()->getAngleDegrees();
      else
         rotDegrees = this->getAngleDegrees() - centroidData->getInsert()->getAngleDegrees();

      rotDegrees = normalizeDegrees(rotDegrees);

      return true;
   } 

   return false;
}

CTMatrix InsertStruct::getTMatrix() const
{
   CTMatrix matrix;

   matrix.translateCtm(m_origin.x,m_origin.y);
   matrix.rotateRadiansCtm(m_angleRadians);
   matrix.scaleCtm(((m_mirrorFlags & MIRROR_FLIP) != 0) ? -m_scale : m_scale,m_scale);

   return matrix;
}

CBasesVector InsertStruct::getBasesVector() const
{
   return CBasesVector(m_origin.x,m_origin.y,m_scale,radiansToDegrees(m_angleRadians),(m_mirrorFlags & MIRROR_FLIP) != 0);
}

void InsertStruct::setBasesVector(const CBasesVector& basesVector)
{
   m_origin.x     = (DbUnit)basesVector.getOrigin().x;
   m_origin.y     = (DbUnit)basesVector.getOrigin().y;
   m_scale        = (DbUnit)basesVector.getScale();
   m_angleRadians = (DbUnit)(basesVector.getRotationRadians() * (basesVector.getMirror() ? -1. : 1.));
   setGraphicsMirrorFlag(basesVector.getMirror());
}

void InsertStruct::transform(const CTMatrix& transformationMatrix)
{
   CBasesVector basesVector(m_origin.x,m_origin.y,m_scale,radiansToDegrees(m_angleRadians),(m_mirrorFlags & MIRROR_FLIP) != 0);
   basesVector.transform(transformationMatrix);

   m_origin.x  = (DbUnit)basesVector.getOrigin().x;
   m_origin.y  = (DbUnit)basesVector.getOrigin().y;

   //scale  = (DbUnit)basesVector.getScale();
   m_angleRadians = (DbUnit)degreesToRadians(basesVector.getRotation());

   // the layer mirroring should never be changed due to the transformation
   setGraphicsMirrorFlag(basesVector.getMirror());
}

bool InsertStruct::insertGeometryFpeq(const InsertStruct& other) const
{
   bool retval = (fpeq(m_origin.x,other.m_origin.x) &&
                  fpeq(m_origin.y,other.m_origin.y) &&
                  fpeq(m_angleRadians,other.m_angleRadians) &&
                  fpeq(m_scale,other.m_scale) &&
                  m_mirrorFlags  == other.m_mirrorFlags  &&
                  m_placedBottom == other.m_placedBottom);
   
   return retval;
}

bool InsertStruct::isValid() const
{
   bool retval = true;

   if (m_refname != NULL)
   {
      void* pString = (void*)(m_refname);
      unsigned int* i = (unsigned int*)pString;

      unsigned int address = (unsigned int)(pString);

      if (AfxIsValidAddress(m_refname,sizeof(CString),true) == 0)
      {
         retval = false;
      }

      if ((address & 0xfeee0000) == 0xfeee0000)
      {
         retval = false;
      }

      if ((*i & 0xfeee0000) == 0xfeee0000)
      {
         retval = false;
      }

      if ((*i & 0xeefe0000) == 0xeefe0000)
      {
         retval = false;
      }
   }

   return retval;
}

void InsertStruct::dump(CWriteFormat& writeFormat,int depth) const
{
   writeFormat.writef(
"InsertStruct\n"
"{\n"
"   m_refname='%s'\n"
"   m_origin=(%.3f,%.3f)\n"
"   m_angleRadians=%.3f (%.3f degrees)\n"
"   m_mirrorFlags=%d\n"
"   m_placedBottom=%d\n"
"   m_scale=%.3f\n"
"   m_blockNumber=%d\n"
"   m_shading=0x%08x\n"
"   m_insertType=%d (%s)\n"
"}\n",
(const char*)((m_refname == NULL) ? "" : *m_refname),
(double)m_origin.x,(double)m_origin.y,
(double)m_angleRadians,radiansToDegrees(m_angleRadians),
(int)m_mirrorFlags,
(int)m_placedBottom,
(double)m_scale,
m_blockNumber,
m_shading,
m_insertType,(const char*)insertTypeToString(m_insertType));
}

////_____________________________________________________________________________
//void CThetaSortedInsertStructArrayContainer::add(InsertStruct* insert)
//{
//   CPoint2d origin = insert->getOrigin2d();
//   m_matrix.transform(origin);
//   double radius   = origin.getRadius();
//   double theta    = origin.getTheta();
//
//   int index;
//
//   for (index = 0;index < getCount();index++)
//   {
//      InsertStruct* existingInsert = getAt(index);
//      CPoint2d existingOrigin = existingInsert->getOrigin2d();
//      m_matrix.transform(existingOrigin);
//      double existingTheta = existingOrigin.getTheta();
//
//      if (fpeq(existingTheta,theta))
//      {
//         if (existingOrigin.getRadius() > radius)
//         {
//            break;
//         }
//      }
//      else if (existingTheta > theta)
//      {
//         break;
//      }
//   }
//
//   InsertAt(index,insert);
//}
