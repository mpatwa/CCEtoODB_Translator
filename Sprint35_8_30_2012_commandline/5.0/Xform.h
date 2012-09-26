// $Header: /CAMCAD/4.5/Xform.h 13    4/12/05 8:28a Devin Dow $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#pragma once

#include "geomlib.h"
#include <math.h>

/******************************************************************************
* CTransform
*/
class DTransform : public CObject
{
public:
   DTransform() 
   {
      x = y = 0.0;
      scale = 1.0;
      mirror = 0;

      SetRotation(0.0);
   };


   DTransform(double _x, double _y, double _scale, double _rotation, int _mirror) 
   {
      x = _x;
      y = _y;
      scale = _scale;
      mirror = _mirror;

      SetRotation(_rotation);

      ASSERT (fabs(scale) > SMALLNUMBER);
   };

   DTransform(DTransform &_xform) 
   {
      x = _xform.x;
      y = _xform.y;
      scale = _xform.scale;
      mirror = _xform.mirror;

      SetRotation(_xform.rotation);

      ASSERT (fabs(scale) > SMALLNUMBER);
   };

   double x;
   double y;
   double scale;
   double rotation;
   int mirror;

   Mat2x2 m_positive;
   Mat2x2 m_negative;


   // Functions
   void TransformPoint(Point2 *point) const
   {
      point->x *= scale;
      if (mirror & MIRROR_FLIP)
      {
         point->x = -point->x;
         point->bulge = -point->bulge;
      }
      point->y *= scale;
      TransPoint2(point, 1, &m_positive, x, y);
   };

   void UntransformPoint(Point2 *point) const
   {
      // insertion point
      point->x -= x;
      point->y -= y;

      if (mirror & MIRROR_FLIP)
      {
         point->x = -point->x;
         //point->bulge = -point->bulge;
	      TransPoint2(point, 1, &m_positive, 0.0, 0.0); // rot = -rot; TransforPoint(-rot); 
      }
		else
			TransPoint2(point, 1, &m_negative, 0.0, 0.0); // TransforPoint(-rot);

      point->x /= scale;
      point->y /= scale;
   };

   void SetRotation(double _rotation)
   {
      rotation = _rotation;
      RotMat2(&m_positive, rotation);
      RotMat2(&m_negative, -rotation);
   };
};


