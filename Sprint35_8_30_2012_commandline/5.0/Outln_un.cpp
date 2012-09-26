// $Header: /CAMCAD/4.3/Outln_un.cpp 7     8/12/03 5:07p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

/******************************************************************************
* Region Union Algorithm 
*/

#include "stdafx.h"
#include <math.h>
#include "data.h"
#include "polylib.h"
#include "bb_lib.h"

// Building Block includes
// needed for BuildingBlock software
extern "C"
{
#include "t2defs.h"
#include "c2ddefs.h"
#include "t2ddefs.h"
#include "c2vdefs.h"
#include "c2cdefs.h"
#include "t2idefs.h"
}



