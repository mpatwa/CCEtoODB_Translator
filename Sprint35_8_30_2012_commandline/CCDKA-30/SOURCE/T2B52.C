/* -S -Z -L -T __BBS_MILL__=2 __BBS_TURN__=2 */
/********************************* T2B52 .C *********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <c2ddefs.h>
#include <m2cdefs.h>
#include <t2bpriv.h>

#if ( __BBS_MILL__>=2 || __BBS_TURN__>=2 )
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2b_offset2_curve ( curve, desc, l, curvelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
T2_OFFSET_DESC desc ;
INT l ;
DML_LIST curvelist ;
{
    DML_ITEM item ;
    INT i, n ;
    PT2 a[4] ;
    C2_CURVE arc ;

    if ( l == 0 || !C2_CURVE_IS_ZERO_ARC(curve) || ( desc == NULL ) ||
        ( T2_OFFSET_DESC_MODE(desc) == T2_FILLET ) )
        RETURN ( l ) ;

    item = DML_LAST(curvelist) ;
    arc = (C2_CURVE)DML_RECORD(item) ;
    n = ( T2_OFFSET_DESC_MODE(desc) == T2_SHARP ) ? 3 : 4 ;
    n = m2c_arc_polygon ( arc, n, a ) ;
    c2d_free_curve ( arc ) ;

    DML_RECORD(item) = c2d_line ( a[0], a[1] ) ;
    for ( i = 2 ; i < n ; i++ ) 
        dml_append_data ( curvelist, c2d_line ( a[i-1], a[i] ) ) ;
    RETURN ( 1 ) ;  /* or n-1 ? */ 
}
#endif /* __BBS_MILL__>=2 || __BBS_TURN__>=2 */

