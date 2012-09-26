/* -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2DT.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2cdefs.h> 
#include <dmldefs.h> 
#include <t2cdefs.h> 
#include <t2ddefs.h> 
#include <t2dpriv.h> 
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2d_rotate_region ( region0, origin, angle )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region0 ;
PT2 origin ;
REAL angle ;
{
    T2_REGION region = t2d_copy_region ( region0 ) ;

    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( NULL ) ;
    t2c_rotate_region ( region, origin, angle ) ;
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2d_translate_edge ( edge, shift )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 shift ;
{
    RETURN ( edge == NULL ? NULL : 
        t2c_translate_edge ( t2d_copy_edge ( edge ), shift ) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

