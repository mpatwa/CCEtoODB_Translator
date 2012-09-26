// $Header: /CAMCAD/DcaLib/DcaFillStyle.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaFillStyle_h__)
#define __DcaFillStyle_h__

#pragma once

//_____________________________________________________________________________
// Extends HS_xxxxx symbols, found in wingdi.h
// These values must not conflict with HS_xxxxxx values found in wingdi.h
#define HS_SOLID     -1
#define HS_NOT_SET   -2

//_____________________________________________________________________________
short ValidatedFillStyle(short proposedFillStyle);

#endif
