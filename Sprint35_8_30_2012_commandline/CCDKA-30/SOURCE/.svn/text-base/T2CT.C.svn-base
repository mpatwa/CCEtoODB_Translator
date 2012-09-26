/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2CT.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2cdefs.h> 
#include <dmldefs.h> 
#include <t2cdefs.h> 
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC void t2c_rotate_cs_edge __(( T2_EDGE, PT2, REAL, REAL )) ;
/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2c_translate_region ( region, shift )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 shift ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_translate_loop ( DML_RECORD(item), shift ) ;
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_translate_loop ( loop, shift )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 shift ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) 
        t2c_translate_edge ( DML_RECORD(item), shift ) ;
    t2c_loop_box ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_translate_edge ( edge, shift )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 shift ;
{
    c2c_translate ( T2_EDGE_CURVE(edge), shift ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_mirror_dir_loop ( loop, origin, normal )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 origin, normal ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) 
        t2c_mirror_dir_edge ( DML_RECORD(item), origin, normal ) ;
    t2c_loop_box ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_mirror_dir_edge ( edge, origin, normal )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 origin, normal ;
{
    c2c_mirror_dir ( T2_EDGE_CURVE(edge), origin, normal ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2c_rotate_region ( region, origin, angle )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 origin ;
REAL angle ;
{
    REAL c = cos(angle), s = sin(angle) ;
    DML_ITEM item ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_rotate_cs_loop ( DML_RECORD(item), origin, c, s ) ;
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2c_rotate_cs_region ( region, origin, c, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 origin ;
REAL c, s ;
{
    DML_ITEM item ;
    if ( region == NULL || T2_REGION_LOOP_LIST(region) == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_rotate_cs_loop ( DML_RECORD(item), origin, c, s ) ;
    RETURN ( region ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2c_rotate_looplist ( looplist, origin, angle )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST looplist ;
PT2 origin ;
REAL angle ;
{
    REAL c, s ;
    DML_ITEM item ;

    if ( looplist == NULL ) 
        RETURN ;
    c = cos ( angle ) ;
    s = sin ( angle ) ;

    DML_WALK_LIST ( looplist, item ) 
        t2c_rotate_cs_loop ( DML_RECORD(item), origin, c, s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_rotate_loop ( loop, origin, angle )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 origin ;
REAL angle ;
{
    RETURN ( t2c_rotate_cs_loop ( loop, origin, cos(angle), sin(angle) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_rotate_cs_loop ( loop, origin, c, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 origin ;
REAL c, s ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) 
        t2c_rotate_cs_edge ( DML_RECORD(item), origin, c, s ) ;
    t2c_loop_box ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2c_rotate_cs_edge ( edge, origin, c, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 origin ;
REAL c, s ;
{
    c2c_rotate_cs ( T2_EDGE_CURVE(edge), origin, c, s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_EDGE t2c_transform_edge ( edge, xform )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
C2_TRANSFORM xform ;
{
    c2c_transform ( T2_EDGE_CURVE(edge), xform ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_LOOP t2c_transform_loop ( loop, xform )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_TRANSFORM xform ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) 
        t2c_transform_edge ( (T2_EDGE)DML_RECORD(item), xform ) ;
    t2c_loop_box ( loop ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_REGION t2c_transform_region ( region, xform )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
C2_TRANSFORM xform ;
{
    DML_ITEM item ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        t2c_transform_loop ( (T2_LOOP)DML_RECORD(item), xform ) ;
    RETURN ( region ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

