// $Header: /CAMCAD/4.3/Api_Dfm.cpp 10    8/12/03 2:56p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "API.h"

short API::dfmSetMaxErrorsForTest(short maxErrors) 
{  return RC_NO_LICENSE;   }

short API::dfmHaltTest() 
{  return RC_NO_LICENSE;   }

short API::dfmCreateFlattenGeometry(long filePosition, short FAR* geometryNumber) 
{  return RC_NO_LICENSE;   }

short API::dfmFlatten(short geometryNumber, short layerNumber, short textMode) 
{  return RC_NO_LICENSE;   }

short API::dfmFlattenByInserttypes(short geometryNumber, short layerNumber, short inserttype1, short inserttype2) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheck(short geometryNumber, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckDiffNets(short geometryNumber, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckTwoNets(short geometryNumber, LPCTSTR netname1, LPCTSTR netname2, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckByClass(short geometryNumber, short class1, short class2, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckByPackage(short geometryNumber, 
      LPCTSTR packageKeyword, LPCTSTR package1, LPCTSTR package2,  
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckByInserttype(short geometryNumber, short inserttype1, short inserttype2, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmDistanceCheckClassInserttype(short geometryNumber, short class1, short inserttype2, 
      double acceptableDistance, double marginalDistance, double criticalDistance, 
      LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch) 
{  return RC_NO_LICENSE;   }

short API::dfmComponentShadow(long filePosition, 
      double accLeftMargin, double accRightMargin, double accTopMargin, double accBottomMargin, 
      double marLeftMargin, double marRightMargin, double marTopMargin, double marBottomMargin, 
      double crtLeftMargin, double crtRightMargin, double crtTopMargin, double crtBottomMargin) 
{  return RC_NO_LICENSE;   }

short API::dfmAcidTraps(short geometryNumber, double acceptableAngle, double marginalAngle, double criticalAngle, LPCTSTR layerSuffix) 
{  return RC_NO_LICENSE;   }
