// $Header: /CAMCAD/DcaLib/DcaFpeq.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaFpeq_h__)
#define __DcaFpeq_h__

////#pragma once

#define  SMALLNUMBER    (1.0E-6) // this is for smallest distance. This is according to MS
                                 // the accuracy of floating point. because we use floating
                                 // point as our datastruct, thats what it is.

#define  ALMOSTZERO     (0.001)  // only used for finding numbers close to zero (especially slopes)
                                 // (use SMALLNUMBER to compare floating point numbers)
                                 // (use ALMOSTZERO to compare a number to zero) (still use fabs())

#define  SMALLANGLE        0.05  // 0.05 radians = 3 degrees
                                 // used to compare angles

#define  BULGE_THRESHOLD   0.01  // 0.01 bulge number = 2 degrees
                                 // used to compare bulge numbers

bool fpeq(double a,double b);
bool fpeq(double a,double b,double smallNumber);
bool fpnear(double a,double b,double tolerance);

#endif
