/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** T2I1.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <t2idefs.h>
#include <t2ipriv.h>
#include <c2vmcrs.h>
#include <c2mem.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC BOOLEAN t2i_closest __(( DML_LIST, BOOLEAN, PT2, T2_PARM )) ;
STATIC BOOLEAN t2i_closest_valid __(( BOOLEAN, T2_INT_REC )) ;
STATIC BOOLEAN t2i_first_last_ti __(( DML_LIST, PT2, T2_PARM, PT2, T2_PARM )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_reg_hor_ray_int_no ( region, pt, pt_on_bndry )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
BOOLEAN pt_on_bndry ;
{
    DML_ITEM item ;
    INT int_no = 0, current ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        current = t2i_loop_hor_ray_int_no ( (T2_LOOP)DML_RECORD(item), 
            pt, pt_on_bndry ) ;
        if ( current < 0 )
            RETURN ( current ) ;
        else 
            int_no += current ;
    }
    RETURN ( int_no ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_reg_ray_int_no ( region, pt, c, s, pt_on_bndry )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_bndry ;
{
    DML_ITEM item ;
    INT int_no = 0, current ;

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) {
        current = t2i_loop_ray_int_no ( (T2_LOOP)DML_RECORD(item), 
            pt, c, s, pt_on_bndry ) ;
        if ( current < 0 )
            RETURN ( current ) ;
        else 
            int_no += current ;
    }
    RETURN ( int_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_loop_hor_ray_int_no ( loop, pt, pt_on_loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
BOOLEAN pt_on_loop ;
{
    DML_ITEM item ;
    INT int_no = 0, current ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        current = t2i_edge_hor_ray_int_no ( (T2_EDGE)DML_RECORD(item), 
            pt, pt_on_loop ) ;

        if ( current < 0 )
            RETURN ( current ) ;
        int_no += current ;
    }
    RETURN ( int_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_loop_ray_int_no ( loop, pt, c, s, pt_on_loop )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_loop ;
{
    DML_ITEM item ;
    INT int_no = 0, current ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        current = t2i_edge_ray_int_no ( (T2_EDGE)DML_RECORD(item), 
            pt, c, s, pt_on_loop ) ;
        if ( current < 0 )
            RETURN ( current ) ;
        else 
            int_no += current ;
    }
    RETURN ( int_no ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_edge_hor_ray_int_no ( edge, pt, pt_on_edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
BOOLEAN pt_on_edge ;
{
    INT m ;
    m = m2c_hor_ray_int_no ( T2_EDGE_CURVE(edge), pt, pt_on_edge ) ;
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT t2i_edge_ray_int_no ( edge, pt, c, s, pt_on_edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
REAL c, s ;
BOOLEAN pt_on_edge ;
{
    RETURN ( m2c_ray_int_no ( T2_EDGE_CURVE(edge), pt, c, s, pt_on_edge ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_region_ray ( region, pt, angle, int_pt, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt, int_pt ;
REAL angle ;
T2_PARM int_parm ;
{
    C2_CURVE ray = c2d_ray ( pt, angle ) ;
    DML_LIST intlist = dml_create_list();
    BOOLEAN status ;

    t2i_intersect_ray_region ( ray, region, TRUE, FALSE, intlist ) ;
    c2d_free_curve ( ray ) ;
    ray = NULL ;
    status = t2i_closest ( intlist, TRUE, int_pt, int_parm ) ;
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    intlist = NULL ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_loop_ray ( loop, pt, angle, int_pt, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, int_pt ;
REAL angle ;
T2_PARM int_parm ;
{
    RETURN ( t2i_edgelist_ray ( T2_LOOP_EDGE_LIST(loop), pt, angle, 
        int_pt, int_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2i_edgelist_ray ( edgelist, pt, angle, int_pt, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST edgelist ;
PT2 pt, int_pt ;
REAL angle ;
T2_PARM int_parm ;
{
    C2_CURVE ray = c2d_ray ( pt, angle ) ;
    DML_LIST intlist = dml_create_list();
    BOOLEAN status ;

    t2i_intersect_ray_edgelist ( ray, edgelist, TRUE, FALSE, TRUE, intlist ) ;
    c2d_free_curve ( ray ) ;
    ray = NULL ;
    status = t2i_closest ( intlist, TRUE, int_pt, int_parm ) ;
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    intlist = NULL ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_region_infline ( region, pt, angle, 
        int_pt_r, int_parm_r, int_pt_l, int_parm_l ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt, int_pt_r, int_pt_l ;
REAL angle ;
T2_PARM int_parm_r, int_parm_l ;
{
    C2_CURVE infline = c2d_ray ( pt, angle ) ;
    DML_LIST intlist = dml_create_list();
    BOOLEAN status ;

    t2i_region_curve ( region, infline, TRUE, intlist ) ;
    c2d_free_curve ( infline ) ;
    infline = NULL ;
    status = t2i_closest ( intlist, TRUE, int_pt_r, int_parm_r ) && 
        t2i_closest ( intlist, FALSE, int_pt_l, int_parm_l ) ;
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    intlist = NULL ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_loop_infline ( loop, pt, angle, int_pt_r, int_parm_r, 
            int_pt_l, int_parm_l ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt, int_pt_r, int_pt_l ;
REAL angle ;
T2_PARM int_parm_r, int_parm_l ;
{
    C2_CURVE infline = c2d_ray ( pt, angle ) ;
    DML_LIST intlist = dml_create_list();
    BOOLEAN status ;

    t2i_loop_curve ( loop, infline, TRUE, intlist ) ;
    c2d_free_curve ( infline ) ;
    infline = NULL ;
    status = t2i_closest ( intlist, TRUE, int_pt_r, int_parm_r ) && 
        t2i_closest ( intlist, FALSE, int_pt_l, int_parm_l ) ;
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    intlist = NULL ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2i_closest ( intlist, dir, int_pt, int_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 int_pt ;
BOOLEAN dir ;
DML_LIST intlist ;
T2_PARM int_parm ;
{
    DML_ITEM item, item0 ;
    REAL t0 ;
    T2_INT_REC ti ;
/* Added 12-06-91 */
    if ( int_pt == NULL && int_parm == NULL ) 
        RETURN ( TRUE ) ;
/* Added 12-06-91 */
    for ( item = DML_FIRST(intlist), item0 = NULL ; 
        item != NULL && item0 == NULL ; item = DML_NEXT(item) ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        t0 = T2_INT_REC_T1(ti) ;
        if ( t2i_closest_valid ( dir, ti ) )
            item0 = item ;
    }
    if ( item0 == NULL ) 
        RETURN ( FALSE ) ;

    DML_FOR_LOOP ( DML_NEXT(item0), item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        if ( dir ) {
            if ( t2i_closest_valid ( dir, ti ) && T2_INT_REC_T1(ti) < t0 ) {
                item0 = item ;
                t0 = T2_INT_REC_T1(ti) ;
            }                               
        }
        else {
            if ( t2i_closest_valid ( dir, ti ) && T2_INT_REC_T1(ti) < t0 ) {
                item0 = item ;
                t0 = T2_INT_REC_T1(ti) ;
            }                               
        }
   }

    ti = (T2_INT_REC)DML_RECORD(item0) ;
    if ( int_pt != NULL ) 
        C2V_COPY ( T2_INT_REC_PT(ti), int_pt ) ;
    if ( int_parm != NULL ) {
        T2_PARM_EDGE(int_parm) = T2_INT_REC_EDGE2(ti) ;
        PARM_COPY ( T2_INT_REC_PARM2(ti), T2_PARM_CPARM(int_parm) ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2i_closest_valid ( dir, ti ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
BOOLEAN dir ;
T2_INT_REC ti ;
{
    T2_EDGE edge ;

    if ( dir ) {
        if ( T2_INT_REC_T1(ti) > BBS_ZERO )
            RETURN ( TRUE ) ;
        else if ( T2_INT_REC_TYPE(ti) == -2 ) {
            edge = T2_INT_REC_EDGE2(ti) ;
            if ( T2_INT_REC_T2(ti) >= T2_EDGE_T_MAX(edge) - BBS_ZERO ) 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( T2_INT_REC_T1(ti) >= -BBS_ZERO ) ;
    }
    else {
        if ( T2_INT_REC_T1(ti) < -BBS_ZERO )
            RETURN ( TRUE ) ;
        else if ( T2_INT_REC_TYPE(ti) == -2 ) {
            edge = T2_INT_REC_EDGE2(ti) ;
            if ( T2_INT_REC_T2(ti) >= T2_EDGE_T_MAX(edge) - BBS_ZERO ) 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( T2_INT_REC_T1(ti) <= BBS_ZERO ) ;
    }
#ifdef __BRLNDC__
	RETURN ( T2_INT_REC_T1(ti) <= BBS_ZERO ) ;
#endif
#ifdef __WATCOM__
	RETURN ( T2_INT_REC_T1(ti) <= BBS_ZERO ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_region_curve ( region, curve, ext_curve, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
C2_CURVE curve ;
BOOLEAN ext_curve ;
DML_LIST intlist ;
{
    INT n = 0 ;
    DML_ITEM item ;
    T2_LOOP loop ;
    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item )
    {
        loop = ( T2_LOOP ) dml_record ( item ) ;
        n += t2i_loop_curve ( loop, curve, ext_curve, intlist ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_loop_curve ( loop, curve, ext_curve, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
C2_CURVE curve ;
BOOLEAN ext_curve ;
DML_LIST intlist ;
{
    INT n = 0 ;
    DML_ITEM item ;
    T2_EDGE edge ;
    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item )
    {
        edge = ( T2_EDGE ) dml_record ( item ) ;
        n += t2i_edge_curve ( edge, curve, ext_curve, intlist ) ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_loop_circle ( loop, ctr, rad, pt0, parm0, pt1, parm1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 ctr ;
REAL rad ;
PT2 pt0 ;
T2_PARM parm0 ;
PT2 pt1 ;
T2_PARM parm1 ; 
{ 
#ifndef __BRLNDC__
    INT n ;
#endif
    C2_CURVE circle ;
    DML_LIST intlist ;
    BOOLEAN status ;

    intlist = dml_create_list();
    circle = c2d_circle ( ctr, rad ) ;
#ifndef __BRLNDC__
    n = t2i_loop_curve ( loop, circle, TRUE, intlist ) ;
#endif
    status = t2i_first_last_ti ( intlist, pt0, parm0, pt1, parm1 ) ;
    c2d_free_curve ( circle ) ;
    dml_destroy_list ( intlist, ( PF_ACTION ) t2i_free_int_rec ) ;
    intlist = NULL ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2i_first_last_ti ( intlist, pt0, parm0, pt1, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
PT2 pt0 ;
T2_PARM parm0 ;
PT2 pt1 ;
T2_PARM parm1 ;
{
    DML_ITEM item ;
    T2_INT_REC ti, ti0, ti1 ;
    BOOLEAN status = FALSE ;

    ti0 = NULL ;
    ti1 = NULL ;

    DML_WALK_LIST ( intlist, item ) {
        ti = (T2_INT_REC)DML_RECORD(item) ;
        if ( pt0 != NULL || parm0 != NULL ) {
            if ( ti0 == NULL || t2c_compare_cparms ( T2_INT_REC_EDGE1(ti), 
                T2_INT_REC_PARM1(ti), T2_INT_REC_EDGE1(ti0), 
                T2_INT_REC_PARM1(ti0) ) == 1 ) 
                ti0 = ti ;
        }
        if ( pt1 != NULL || parm1 != NULL ) {
            if ( ti1 == NULL || t2c_compare_cparms ( T2_INT_REC_EDGE1(ti), 
                T2_INT_REC_PARM1(ti), T2_INT_REC_EDGE1(ti1), 
                T2_INT_REC_PARM1(ti1) ) == - 1 ) 
                ti1 = ti ;
        }
    }

    if ( ti0 != NULL ) {
        if ( pt0 != NULL ) {
            C2V_COPY ( T2_INT_REC_PT(ti0), pt0 ) ;
        }
        if ( parm0 != NULL ) {
            PARM_COPY ( T2_INT_REC_PARM1(ti0), T2_PARM_CPARM(parm0) ) ;
            T2_PARM_EDGE(parm0) = T2_INT_REC_EDGE1(ti0) ;
        }
        status = TRUE ;
    }

    if ( ti1 != NULL ) {
        if ( pt1 != NULL ) {
            C2V_COPY ( T2_INT_REC_PT(ti1), pt1 ) ;
        }
        if ( parm1 != NULL ) {
            PARM_COPY ( T2_INT_REC_PARM1(ti1), T2_PARM_CPARM(parm1) ) ;
            T2_PARM_EDGE(parm1) = T2_INT_REC_EDGE1(ti1) ;
        }
        status = TRUE ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_edge_curve ( edge, curve, ext_curve, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
C2_CURVE curve ;
BOOLEAN ext_curve ;
DML_LIST intlist ;
{
    DML_LIST list = dml_create_list() ;
    DML_ITEM item ;
    INT n ;
    C2_INT_REC ci ;
    T2_INT_REC ti ;

    n = ext_curve ? c2c_intersect_ext ( T2_EDGE_CURVE(edge), curve, list ) : 
        c2c_intersect ( T2_EDGE_CURVE(edge), curve, list ) ;

    if ( n < 0 ) 
        RETURN ( 0 ) ;
    DML_WALK_LIST ( list, item ) {
        ci = ( C2_INT_REC ) dml_record ( item ) ;
        if ( ( C2_INT_REC_T1(ci) >= T2_EDGE_T_MIN(edge) - BBS_ZERO ) 
          && ( C2_INT_REC_T1(ci) < T2_EDGE_T_MAX(edge) + BBS_ZERO ) )
        {
            ti = t2i_create_int_rec ( edge, NULL, ci ) ;
            dml_append_data ( intlist, ti ) ;
        }
        else {
            n-- ;
            C2_FREE_INT_REC(ci) ;
            ci = NULL ;
        }
    }
    dml_free_list ( list ) ;
    list = NULL ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT t2i_loop_edge ( loop, edge, intlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
T2_EDGE edge ;
DML_LIST intlist ;
{
    DML_ITEM item ;
    INT n ;
    T2_INT_REC ti ;

    n = t2i_loop_curve ( loop, T2_EDGE_CURVE(edge), FALSE, intlist ) ;

    DML_WALK_LIST ( intlist, item ) {
        ti = DML_RECORD(item) ;
        T2_INT_REC_EDGE2(ti) = edge ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN t2i_trim_2loops ( loop0, loop1, ext_only )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop0, loop1 ;
BOOLEAN ext_only ;
{
    DML_LIST intlist ;
    DML_ITEM item ;
    BOOLEAN status = FALSE ;
    REAL u, cross, dist, dist_min = 0.0 ;
    PT2 p0, p1, p, t0, t1 ;
    C2_INT_REC ci, ci0 ;
    T2_EDGE edge0, edge1, new ;

    edge0 = t2c_last_edge ( loop0 ) ;
    edge1 = t2c_first_edge ( loop1 ) ;
    t2c_ept_tan1 ( edge0, p0, t0 ) ;
    t2c_ept_tan0 ( edge1, p1, t1 ) ;
    cross = C2V_CROSS ( t0, t1 ) ;
    if ( ext_only && ( cross < - BBS_ZERO ) )
        RETURN ( C2V_IDENT_PTS ( p0, p1 ) ) ;
    if ( fabs(cross) <= BBS_ZERO * C2V_NORML1 ( t0 ) * C2V_NORML1 ( t1 ) ) {
        C2V_MID_PT ( p0, p1, p ) ;
    }
    else {
        C2V_SUB ( p1, p0, p ) ;
        u = C2V_CROSS ( p, t1 ) / cross ;
        C2V_ADDT ( p0, t0, u, p ) ;
    }

    intlist = dml_create_list () ;
    c2c_intersect_ext ( T2_EDGE_CURVE(edge0), T2_EDGE_CURVE(edge1), intlist ) ;
    DML_WALK_LIST ( intlist, item ) {
        ci = DML_RECORD(item) ;
        dist = C2V_DIST ( C2_INT_REC_PT(ci), p ) ;
        if ( !status || ( dist < dist_min ) ) {
            dist_min = dist ;
            ci0 = ci ;
            status = TRUE ;
        }
    }
    if ( status ) {
        new = t2d_trim_ext_edge ( edge0, NULL, C2_INT_REC_PARM1(ci0) ) ;
        if ( new != NULL ) {
            T2_EDGE_LOOP(new) = loop0 ;
            dml_insert_prior ( T2_LOOP_EDGE_LIST(loop0), 
                DML_LAST(T2_LOOP_EDGE_LIST(loop0)), new ) ;
        }        
        new = t2d_trim_ext_edge ( edge1, C2_INT_REC_PARM2(ci0), NULL ) ;
        if ( new != NULL ) {
            T2_EDGE_LOOP(new) = loop1 ;
            dml_insert ( T2_LOOP_EDGE_LIST(loop1), new, TRUE ) ;
        }        
    }
    else {
        new = t2d_create_edge ( NULL, c2d_line ( p0, p1 ), 1 ) ;
        T2_EDGE_LOOP(new) = loop0 ;
        if ( new != NULL ) 
            dml_append_data ( T2_LOOP_EDGE_LIST(loop0), new ) ;
        status = TRUE ;
    }
    dml_destroy_list ( intlist, ( PF_ACTION ) c2d_free_int_rec ) ;
    RETURN ( status ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

