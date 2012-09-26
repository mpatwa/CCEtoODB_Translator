// $Header: /CAMCAD/DcaLib/DcaDblRect.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaDblRect.h"

//_____________________________________________________________________________
CDblRect::CDblRect(BOOL initToGrow)
{
   if (initToGrow)
      Init();
   else
      ReverseInit();
}  

CDblRect::CDblRect(const CDblRect &rect)
{
	if (this != &rect)
	{
		xMin = rect.xMin;
		xMax = rect.xMax;
		yMin = rect.yMin;
		yMax = rect.yMax;
	}
}

CDblRect::CDblRect(double xmin, double xmax, double ymin, double ymax)
{
   xMin = xmin;
   xMax = xmax;
   yMin = ymin;
   yMax = ymax;
}

void CDblRect::Init()
{
   xMin = yMin = DBL_MAX;
   xMax = yMax = -DBL_MAX;
}

void CDblRect::ReverseInit()
{
   xMin = yMin = -DBL_MAX;
   xMax = yMax = DBL_MAX;
}
