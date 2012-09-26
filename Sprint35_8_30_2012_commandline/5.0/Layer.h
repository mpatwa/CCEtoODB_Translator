// $Header: /CAMCAD/5.0/Layer.h 52    12/10/06 3:56p Kurt Van Ness $

/*****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-97. All Rights Reserved.
*/   

#if !defined(__Layer_h__)
#define __Layer_h__

#pragma once

#include <afxtempl.h>
#include "data.h"
#include "DcaLayer.h"

class CCEtoODBDoc;
class CAttributes;
class CCamCadFileWriteProgress;
class LayerStruct;
class CLayerArray;

int GetMaxElectricalStackNum(CCEtoODBDoc *doc);
// end LAYER.H

#endif
