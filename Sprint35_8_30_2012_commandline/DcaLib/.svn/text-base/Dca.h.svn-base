// $Header: /CAMCAD/DcaLib/Dca.h 5     3/23/07 5:19a Kurt Van Ness $

#if !defined(__Dca_h__)
#define __Dca_h__

#pragma once

class CPnt;
class CTMatrix;

// this does not indicate if a component is placed on the bottom.
#define  MIRROR_FLIP                1
#define  MIRROR_LAYERS              2
#define  MIRROR_ALL                 (MIRROR_FLIP | MIRROR_LAYERS)

// used by propagateLayer
#define FloatingLayer (-3)

void MemErrorMessage(const char *file, int line);
int  ErrorMessage(const char *text, const char *caption = "ERROR", UINT type = MB_OK);

//_____________________________________________________________________________
typedef float DbUnit;
typedef unsigned long DbFlag;

//_____________________________________________________________________________
enum Bool3Tag
{
   boolFalse,
   boolTrue,
   boolUnknown
};

//_____________________________________________________________________________
class CDoubleArray : public CArray<double,double>
{
};

#endif
