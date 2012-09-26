// $Header: /CAMCAD/4.4/Text.h 1     3/08/04 7:05p Kurt Van Ness $

/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/ 

#if !defined(__Text_h__)
#define __Text_h__

#pragma once

#include "Point2d.h"

//_____________________________________________________________________________
class CText
{
private:
   unsigned char m_fontNumber;

   unsigned char m_mirrored:1;
   unsigned char m_proportional:1;
   unsigned char m_neverDrawMirrored:1;
   unsigned char m_horizontalPosition:2; // horizontal position relative to origin
   unsigned char m_verticalPosition:2;   // vertical position relative to origin

   CPoint2d      m_origin;

public:
   CText();
   ~CText();

};

#endif
