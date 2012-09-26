// $Header: /CAMCAD/DcaLib/DcaPoint2d.cpp 4     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaPoint2d.h"
#include "DcaTMatrix.h"
#include "DcaLib.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
CPoint2d CPoint2d::operator+(const CSize2d& size) const
{ 
   return CPoint2d(x + size.cx,y + size.cy); 
}

CPoint2d CPoint2d::operator-(const CSize2d& size) const
{ 
   return CPoint2d(x - size.cx,y - size.cy); 
}

void CPoint2d::transform(const CTMatrix& matrix)
{
   matrix.transform(x,y);
}

CString CPoint2d::getString() const
{
   CString retval;

   retval.Format("(%s,%s)",fpfmt(x,6),fpfmt(y,6));

   return retval;
}

void CPoint2d::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CPoint2d\n"
"{\n"
"   x     = %f\n"
"   y     = %f\n"
"}\n",
x,y);
}
