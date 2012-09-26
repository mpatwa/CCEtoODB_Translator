// $Header: /CAMCAD/DcaLib/DcaPoint.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaPoint_h__)
#define __DcaPoint_h__

#pragma once

#include "Dca.h"

class CPoint2d;
class CWriteFormat;

//_____________________________________________________________________________
class PointStruct
{ 
public:
   DbUnit   x;
   DbUnit   y; 

   PointStruct();
   PointStruct(DbUnit x,DbUnit y);
   PointStruct(const PointStruct& other);

   void transform(const CTMatrix& transformationMatrix);

   CPoint2d getPoint2d() const;
   void setOrigin(const CPoint2d& point);

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

#endif
