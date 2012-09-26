// $Header: /CAMCAD/DcaLib/DcaInsert.h 6     6/05/07 4:57p Kurt Van Ness $

#if !defined(__DcaInsert_h__)
#define __DcaInsert_h__

#pragma once

#include "DcaPoint.h"
#include "DcaFillStyle.h"

class CCamCadData;
class CBasesVector;

enum InsertTypeTag;
enum BlockTypeTag;

InsertTypeTag getDefaultInsertTypeForBlockType(BlockTypeTag blockType);

//_____________________________________________________________________________
struct ColorStruct
{
   BYTE red;
   BYTE green;
   BYTE blue;
   BYTE On:1;
   BYTE Before:1;       // draw the rectangle color before drawing the contents (geom).
   BYTE Transparent:1;
   BYTE Hatched:1;
   BYTE dummy:4;

   ColorStruct();
   ColorStruct(const ColorStruct& other);
};

//_____________________________________________________________________________
class InsertStruct
{
private:
   PointStruct   m_origin;
   DbUnit        m_angleRadians;
   char          m_mirrorFlags;  // 1 = mirror flip geom.
                               // 2 = layer mirrrored
   char          m_placedBottom;
   DbUnit        m_scale;  
   int           m_blockNumber;   // m_blockNumber - is a number of the blockname in blockname-Structure
   InsertTypeTag m_insertType;

   short         m_fillStyle;

   mutable CString* m_refname;   // for PCB_COMPONENTS the insert.refname must be the component name.

   ColorStruct m_shading;

public: 
   InsertStruct();
   InsertStruct(const InsertStruct& other);
   ~InsertStruct();
   InsertStruct& operator=(const InsertStruct& other);

   // accessors
   PointStruct getOrigin() const                { return m_origin; }
   CPoint2d getOrigin2d() const;
   DbUnit getOriginX() const                    { return m_origin.x; }
   DbUnit getOriginY() const                    { return m_origin.y; }
   void setOrigin(const CPoint2d& origin);
   void setOrigin(const PointStruct& origin)    { m_origin.x = origin.x;  m_origin.y = origin.y; }
   void setOrigin(double x,double y)            { m_origin.x = (DbUnit)x;  m_origin.y = (DbUnit)y; }
   void setOrigin(DbUnit x,DbUnit y)            { m_origin.x = x;  m_origin.y = y; }
   void setOriginX(double x)                    { m_origin.x = (DbUnit)x;  }
   void setOriginY(double y)                    { m_origin.y = (DbUnit)y;  }
   void setOriginX(DbUnit x)                    { m_origin.x = x;  }
   void setOriginY(DbUnit y)                    { m_origin.y = y;  }
   void incOriginX(double x)                    { m_origin.x += (DbUnit)x;  }
   void incOriginY(double y)                    { m_origin.y += (DbUnit)y;  }

   DbUnit getAngle() const                      { return m_angleRadians; }
   double getAngleRadians() const               { return m_angleRadians; }
   double getAngleDegrees() const;
   void setAngle(DbUnit angle)                  { this->m_angleRadians = angle; }
   void setAngle(double angle)                  { this->m_angleRadians = (DbUnit)angle; }
   void setAngleRadians(double radians)         { this->m_angleRadians = (DbUnit)radians; }
   void setAngleDegrees(double degrees);
   void incAngle(double angle)                  { this->m_angleRadians += (DbUnit)angle; }

   int getMirrorFlags() const                   { return m_mirrorFlags; }
   bool getGraphicMirrored() const              { return ((m_mirrorFlags & MIRROR_FLIP  ) != 0); }
   bool getLayerMirrored() const                { return ((m_mirrorFlags & MIRROR_LAYERS) != 0); }
   void setMirrorFlags(char mirrorFlags)        { m_mirrorFlags = mirrorFlags; }
   void setMirrorFlagBits(unsigned char mask)   { m_mirrorFlags |= mask; }
   void clearMirrorFlagBits(unsigned char mask) { m_mirrorFlags &= ~mask; }
   void setLayerMirrorFlag(bool flag);
   void setGraphicsMirrorFlag(bool flag);

   bool getPlacedBottom() const                 { return (m_placedBottom != 0); }
   bool getPlacedTop() const                    { return (m_placedBottom == 0); }
   void setPlacedBottom(bool flag)              { m_placedBottom = flag;        }
   void setPlacedBottom(BOOL flag)              { m_placedBottom = (flag != 0); }

   DbUnit getScale() const                      { return m_scale; }
   void setScale(DbUnit scale)                  { this->m_scale = scale; }
   void setScale(double scale)                  { this->m_scale = (DbUnit)scale; }

   int getBlockNumber() const                   { return m_blockNumber; }
   void setBlockNumber(int blockNumber)         { m_blockNumber= blockNumber; }

   InsertTypeTag getInsertType() const          { return m_insertType; }
   void setInsertType(InsertTypeTag insertType) { m_insertType = insertType; }
   void setInsertType(int insertType);

   void setFillStyle(short fillstyle)            { m_fillStyle = fillstyle; }
   void setFillStyleValidated(short fillstyle)   { setFillStyle(ValidatedFillStyle(fillstyle)); }
   short getFillStyle() const                    { return m_fillStyle; }

   const ColorStruct& getShading() const        { return m_shading; }
   ColorStruct& getShadingRef()                 { return m_shading; }
// void setShading(const ColorStruct& shading)  { this->shading = shading; }

   const CString& getRefname() const;
   CString& getRefnameRef();
   static CString getSortableRefDes(const CString& refDes);
   CString getSortableRefDes() const;
   void setRefname(char* refname);
   void setRefname(const CString& refname);
   bool getCentroidLocation(CCamCadData& camCadData, CPoint2d &location) const;
   bool getCentroidRotation(CCamCadData& camCadData, double &rotDegrees) const;

   // query properties functions
   bool hasRefnameData() const                  { return (m_refname != NULL); }
   bool insertGeometryFpeq(const InsertStruct& other) const;

   // operations
   void transform(const CTMatrix& transformationMatrix);
   void initializePosition();
   CBasesVector getBasesVector() const;
   void setBasesVector(const CBasesVector& basesVector);
   CTMatrix getTMatrix() const;

   bool isValid() const;
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

////_____________________________________________________________________________
//class CThetaSortedInsertStructArrayContainer : protected CTypedPtrArrayContainer<InsertStruct*>
//{
//private:
//   CTMatrix m_matrix;
//
//public:
//   CThetaSortedInsertStructArrayContainer(int growBySize=10,bool isContainer=true) :
//      CTypedPtrArrayContainer<InsertStruct*>(growBySize,isContainer) {}
//   virtual ~CThetaSortedInsertStructArrayContainer(){}
//
//   virtual void add(InsertStruct* insert);
//   virtual INT_PTR getCount() { return GetCount(); }
//   virtual InsertStruct* getAt(int index) { return GetAt(index); }
//   virtual void setMatrix(const CTMatrix& matrix) { m_matrix = matrix; }
//};

#endif
