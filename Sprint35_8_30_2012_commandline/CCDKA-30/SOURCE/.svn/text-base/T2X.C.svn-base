/* -Z -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2X.C ***********************************/
/***************************  2-D Regions **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <c2adefs.h>
#include <m2cdefs.h>
#include <m2xdefs.h>
#include <t2cdefs.h>
#include <t2xdefs.h>
#include <c2vmcrs.h>

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

STATIC  INT     t2x_m __(( T2_LOOP, PT2, REAL, REAL, REAL, PT2 )) ;
STATIC  T2_XHATCH   t2x_sort __(( DML_LIST* DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , T2_REGION DUMMY4, T2_LOOP DUMMY5 )) ;
STATIC  INT     t2x_sort_pass1 __(( DML_LIST, REAL, REAL, 
            T2_REGION, T2_LOOP )) ;
STATIC BOOLEAN t2x_sort_pass2 __(( DML_LIST )) ;
STATIC void t2x_sort_pass3 __(( DML_LIST, T2_REGION, T2_LOOP )) ;
STATIC void t2x_sort_pass30 __(( DML_LIST )) ;
STATIC void t2x_sort_pass31 __(( DML_LIST )) ;
STATIC  void    t2x_sort_pass4 __(( DML_LIST, T2_XHATCH, INT* )) ;
STATIC  BOOLEAN t2x_sort_key __(( M2_XH_REC, M2_XH_REC, INT* )) ;
STATIC  INT     t2x_loop_inters __(( T2_LOOP, PT2, REAL, REAL, REAL, 
            DML_LIST*, INT )) ;
STATIC  INT     t2x_edge_inters __(( T2_EDGE, PT2, REAL, REAL, REAL, 
            DML_LIST*, INT )) ;
STATIC  T2_XHATCH   t2x_create __(( INT DUMMY0 )) ;

/*-------------------------------------------------------------------------*/
STATIC T2_XHATCH t2x_create ( n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
{
    T2_XHATCH xhatch = T2_ALLOC_XHATCH ;

    if ( xhatch == NULL ) 
        RETURN ( NULL ) ;
    T2_XHATCH_N(xhatch) = n ;
    if ( n == 0 ) {
        T2_XHATCH_PT_PTR(xhatch) = NULL ;
        T2_XHATCH_PARM(xhatch) = NULL ;
        T2_XHATCH_OWNER(xhatch) = NULL ;
    }
    else {
        T2_XHATCH_PT_PTR(xhatch) = MALLOC ( n, PT2 ) ;
        if ( T2_XHATCH_PT_PTR(xhatch) == NULL ) 
            RETURN ( NULL ) ;
        T2_XHATCH_PARM(xhatch) = MALLOC ( n, PARM_S ) ;
        if ( T2_XHATCH_PARM(xhatch) == NULL ) 
            RETURN ( NULL ) ;
        T2_XHATCH_OWNER(xhatch) = MALLOC ( n, ANY ) ;
        if ( T2_XHATCH_OWNER(xhatch) == NULL ) 
            RETURN ( NULL ) ;
    }
    RETURN ( xhatch ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2x_free ( xhatch )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xhatch ;
{
    if ( xhatch != NULL ) { 
        FREE ( T2_XHATCH_PT_PTR(xhatch) ) ;
        T2_XHATCH_PT_PTR(xhatch) = NULL ;
        FREE ( T2_XHATCH_PARM(xhatch) ) ;
        T2_XHATCH_PARM(xhatch) = NULL ;
        FREE ( T2_XHATCH_OWNER(xhatch) ) ;
        T2_XHATCH_OWNER(xhatch) = NULL ;
        T2_FREE_XHATCH ( xhatch ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2x_reverse ( xhatch )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xhatch ;
{
    INT i0, i1 ;
    PT2 pt ;
    PARM_S parm ;
    ANY owner ;

    if ( xhatch == NULL ) 
        RETURN ;

    for ( i0=0, i1=T2_XHATCH_N(xhatch)-1 ; i0 < i1 ; i0++, i1-- ) {
        if ( T2_XHATCH_PT_PTR(xhatch) != NULL ) {
            C2V_COPY ( T2_XHATCH_PT(xhatch)[i0], pt ) ;
            C2V_COPY ( T2_XHATCH_PT(xhatch)[i1], T2_XHATCH_PT(xhatch)[i0] ) ;
            C2V_COPY ( pt, T2_XHATCH_PT(xhatch)[i1] ) ;
        }
        if ( T2_XHATCH_PARM(xhatch) != NULL ) {
            PARM_COPY ( T2_XHATCH_PARM(xhatch)+i0, &parm ) ;
            PARM_COPY ( T2_XHATCH_PARM(xhatch)+i1, 
                    T2_XHATCH_PARM(xhatch)+i0 ) ;
            PARM_COPY ( &parm, T2_XHATCH_PARM(xhatch)+i1 ) ;
        }
        if ( T2_XHATCH_OWNER(xhatch) != NULL ) {
            owner = T2_XHATCH_OWNER(xhatch)[i0] ;
            T2_XHATCH_OWNER(xhatch)[i0] = T2_XHATCH_OWNER(xhatch)[i1] ;
            T2_XHATCH_OWNER(xhatch)[i1] = owner ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2x_info ( xh, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_XHATCH xh ;
FILE *outfile ;
{
    INT i ;
    if ( outfile == NULL ) {
        printf ( "n = %d\n", T2_XHATCH_N(xh) ) ;
        for ( i=0 ; i<T2_XHATCH_N(xh) ; i+=2 ) 
            printf ( "%lf\t%lf\t%lf\t%lf\n", 
                T2_XHATCH_PT(xh)[i][0], T2_XHATCH_PT(xh)[i][1], 
                T2_XHATCH_PT(xh)[i+1][0], T2_XHATCH_PT(xh)[i+1][1] ) ;
    }
    else {
        fprintf ( outfile, "n = %d\n", T2_XHATCH_N(xh) ) ;
        for ( i=0 ; i<T2_XHATCH_N(xh) ; i+=2 ) 
            fprintf ( outfile, "%lf\t%lf\t%lf\t%lf\n", 
                T2_XHATCH_PT(xh)[i][0], T2_XHATCH_PT(xh)[i][1], 
                T2_XHATCH_PT(xh)[i+1][0], T2_XHATCH_PT(xh)[i+1][1] ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC T2_XHATCH t2x_sort ( intlist, m, c, s, region, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST *intlist ;
INT m ;
REAL c, s ;
T2_REGION region ;
T2_LOOP loop ;
{
    T2_XHATCH xhatch ;
    INT i, k, n, n1 ;

    n = 0 ;
    for ( i=0 ; i<m ; i++ ) {
        n1 = t2x_sort_pass1 ( intlist[i], c, s, region, loop ) ;
        n += n1 ;
    }
    xhatch = t2x_create ( n ) ;
    if ( xhatch == NULL ) 
        RETURN ( NULL ) ;
    k = 0 ;
    for ( i=0 ; i<m ; i++ ) 
        t2x_sort_pass4 ( intlist[i], xhatch, &k ) ;
    
    RETURN ( xhatch ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT t2x_sort_pass1 ( intlist, c, s, region, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
REAL c, s ;
T2_REGION region ;
T2_LOOP loop ;
{
    INT sort_coord ;
    BOOLEAN status ;

    sort_coord = ( fabs(c) < fabs(s) ? 1 : 0 ) ;
    dml_sort_list_data ( intlist, ( PF_SORT_DATA ) t2x_sort_key, 
    					 &sort_coord ) ;
    status = t2x_sort_pass2 ( intlist ) ;
    if ( DML_LENGTH(intlist) == 1 ) {
        m2x_free_xh_rec ( (M2_XH_REC)DML_FIRST_RECORD(intlist) ) ;
        DML_FIRST_RECORD(intlist) = NULL ;
        dml_remove_first ( intlist ) ;
    }
    else if ( !status ) 
        t2x_sort_pass3 ( intlist, region, loop ) ;
    RETURN ( DML_LENGTH(intlist) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2x_sort_pass2 ( intlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
{
    M2_XH_REC rec0, rec1 ;
    DML_ITEM item0, item1 ;
    BOOLEAN status = TRUE ;

    for ( item0 = DML_FIRST(intlist) ; item0 != NULL ; item0 = item1 ) {
        rec0 = DML_RECORD(item0) ;
        item1 = DML_NEXT(item0) ;
        if ( !M2_XH_REC_STATUS(rec0) )
            status = FALSE ;
        if ( item1 != NULL ) {
            rec1 = DML_RECORD(item1) ;
            if ( C2V_IDENT_PTS ( M2_XH_REC_PT(rec0), M2_XH_REC_PT(rec1) ) ) {
                m2x_free_xh_rec ( rec1 ) ;
                rec1 = NULL ;
                dml_remove_item ( intlist, item1 ) ;
                item1 = item0 ;
                status = FALSE ;
            }
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2x_sort_pass3 ( intlist, region, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
T2_REGION region ;
T2_LOOP loop ;
{
    M2_XH_REC rec, rec0, rec1 ;
    DML_ITEM item, item0, item1 ;
    PT2 p0, p1 ;
    T2_PT_POSITION pos0, pos1 ;
    BOOLEAN outside0=FALSE, outside1=FALSE ;

    if ( DML_LENGTH(intlist) == 2 ) {
        item0 = DML_FIRST(intlist) ;
        rec0 = DML_RECORD(item0) ;
        item1 = DML_LAST(intlist) ;
        rec1 = DML_RECORD(item1) ;
        C2V_MID_PT ( M2_XH_REC_PT(rec0), M2_XH_REC_PT(rec1), p0 ) ;
        pos0 = ( region != NULL ) ? t2c_pt_pos_region ( region, p0 ) :
            t2c_pt_pos_loop ( loop, p0 ) ;
        if ( pos0 != T2_PT_INSIDE ) {
            m2x_free_xh_rec ( rec0 ) ;
            m2x_free_xh_rec ( rec1 ) ;
            dml_remove_first ( intlist ) ;
            dml_remove_first ( intlist ) ;
        }
        RETURN ;
    }

    for ( item0 = DML_FIRST(intlist) ; item0 != NULL ; item0 = item ) {
        item = DML_NEXT(item0) ;
        item1 = DML_NEXT(item) ;
        if ( item1 == NULL ) {
            if ( outside0 ) 
                t2x_sort_pass30 ( intlist ) ;
            if ( outside1 ) 
                t2x_sort_pass31 ( intlist ) ;
            RETURN ;
        }
        rec0 = DML_RECORD(item0) ;
        rec = DML_RECORD(item) ;
        rec1 = DML_RECORD(item1) ;
        C2V_MID_PT ( M2_XH_REC_PT(rec0), M2_XH_REC_PT(rec), p0 ) ;
        C2V_MID_PT ( M2_XH_REC_PT(rec), M2_XH_REC_PT(rec1), p1 ) ;
        pos0 = ( region != NULL ) ? t2c_pt_pos_region ( region, p0 ) :
            t2c_pt_pos_loop ( loop, p0 ) ;
        pos1 = ( region != NULL ) ? t2c_pt_pos_region ( region, p1 ) :
            t2c_pt_pos_loop ( loop, p1 ) ;
        if ( item0 == DML_FIRST(intlist) ) 
            outside0 = ( pos0 == T2_PT_OUTSIDE ) ;
        if ( item1 == DML_LAST(intlist) ) 
            outside1 = ( pos1 == T2_PT_OUTSIDE ) ;
        if ( ( pos0 == pos1 ) || 
            ( ( pos0 == T2_PT_ON_BOUNDARY ) && ( pos1 == T2_PT_INSIDE ) ) ||
            ( ( pos1 == T2_PT_ON_BOUNDARY ) && ( pos0 == T2_PT_INSIDE ) ) ) {
            m2x_free_xh_rec ( rec ) ;
            rec = NULL ;
            dml_remove_item ( intlist, item ) ;
            item = item0 ;
        }
    }
    if ( outside0 ) 
        t2x_sort_pass30 ( intlist ) ;
    if ( outside1 ) 
        t2x_sort_pass31 ( intlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2x_sort_pass30 ( intlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
{
    m2x_free_xh_rec ( (M2_XH_REC)DML_FIRST_RECORD(intlist) ) ;
    dml_remove_first ( intlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2x_sort_pass31 ( intlist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
{
    m2x_free_xh_rec ( (M2_XH_REC)DML_LAST_RECORD(intlist) ) ;
    dml_remove_last ( intlist ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2x_sort_key ( xh_rec1, xh_rec2, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
M2_XH_REC xh_rec1, xh_rec2 ;
INT *data ;
{
    RETURN ( M2_XH_REC_PT(xh_rec1)[*data] < M2_XH_REC_PT(xh_rec2)[*data] ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2x_sort_pass4 ( intlist, xhatch, k ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
T2_XHATCH xhatch ;
INT *k ;
{
    DML_ITEM item ;
    M2_XH_REC xh_record ;

    DML_WALK_LIST ( intlist, item ) {
        xh_record = DML_RECORD(item) ;
        C2V_COPY ( M2_XH_REC_PT(xh_record), T2_XHATCH_PT(xhatch)[*k] ) ;
        PARM_COPY ( M2_XH_REC_PARM(xh_record), T2_XHATCH_PARM(xhatch)+(*k) ) ;
        T2_XHATCH_OWNER(xhatch)[*k] = M2_XH_REC_OWNER(xh_record) ;
        (*k)++ ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_XHATCH t2x_region ( region, pt, h, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_REGION region ;
PT2 pt ;
REAL h, angle ;
{
    PT2 mid_pt, p0 ;
    REAL c, s ;
    T2_LOOP x_loop = DML_FIRST_RECORD(T2_REGION_LOOP_LIST(region)) ;
    DML_LIST *intlist ;
    INT m, i ; 
    T2_XHATCH xhatch ;
    DML_ITEM item ;

    if ( pt == NULL ) {
        pt = mid_pt ;
        c2a_box_get_ctr ( T2_LOOP_BOX(x_loop), pt ) ;
    }

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    m = t2x_m ( T2_REGION_EXT_LOOP(region), pt, h, c, s, p0 ) ;

    intlist = CREATE ( m, DML_LIST ) ;
    if ( intlist == NULL ) 
        RETURN ( NULL ) ;
    for ( i=0 ; i<m ; i++ ) {
        intlist[i] = dml_create_list () ;
        if ( intlist[i] == NULL ) 
            RETURN ( NULL ) ;
    }

    DML_WALK_LIST ( T2_REGION_LOOP_LIST(region), item ) 
        if ( t2x_loop_inters ( (T2_LOOP)DML_RECORD(item), 
            p0, h, c, s, intlist, m ) < 0 ) 
            RETURN ( NULL ) ;

    xhatch = t2x_sort ( intlist, m, c, s, region, NULL ) ;
    for ( i=0 ; i<m ; i++ ) {
        dml_destroy_list ( intlist[i], ( PF_ACTION ) m2x_free_xh_rec ) ;
        intlist[i] = NULL ;
    }
    KILL ( intlist ) ;
    RETURN ( xhatch ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_XHATCH t2x_loop ( loop, pt, h, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
REAL h, angle ;
{
    PT2 mid_pt, p0 ;
    REAL c, s ;
    DML_LIST *intlist ;
    INT m, i ; 
    T2_XHATCH xhatch ;

    if ( pt == NULL ) {
        pt = mid_pt ;
        c2a_box_get_ctr ( T2_LOOP_BOX(loop), pt ) ;
    }

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    m = t2x_m ( loop, pt, h, c, s, p0 ) ;

    intlist = CREATE ( m, DML_LIST ) ;
    if ( intlist == NULL ) 
        RETURN ( NULL ) ;
    for ( i=0 ; i<m ; i++ ) {
        intlist[i] = dml_create_list () ;
        if ( intlist[i] == NULL ) 
            RETURN ( NULL ) ;
    }

    if ( t2x_loop_inters ( loop, p0, h, c, s, intlist, m ) < 0 ) 
        RETURN ( NULL ) ;

    xhatch = t2x_sort ( intlist, m, c, s, NULL, loop ) ;
    for ( i=0 ; i<m ; i++ ) {
        dml_destroy_list ( intlist[i], ( PF_ACTION ) m2x_free_xh_rec ) ;
        intlist[i] = NULL ;
    }
    KILL ( intlist ) ;
    RETURN ( xhatch ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT t2x_m ( loop, pt, h, c, s, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
REAL h, c, s ;
PT2 p ;
{
    INT m ; 
    if ( IS_ZERO(c) ) 
        m = (INT) ceil ( ( ( T2_LOOP_MAX_X(loop) - pt[0] ) * s ) / h ) ;
    else if ( c > 0.0 ) 
        m = (INT) ceil ( ( ( T2_LOOP_MAX_X(loop) - pt[0] ) * s 
            - ( T2_LOOP_MIN_Y(loop) - pt[1] ) * c ) / h ) ;
    else 
        m = (INT) ceil ( ( ( T2_LOOP_MAX_X(loop) - pt[0] ) * s 
            - ( T2_LOOP_MAX_Y(loop) - pt[1] ) * c ) / h ) ;
    p[0] = pt[0] + m * h * s ;
    p[1] = pt[1] - m * h * c ;

    if ( IS_ZERO(c) )
        m = (INT) ceil ( ( ( p[0] - T2_LOOP_MIN_X(loop) ) * s ) / h ) + 1 ;
    else if ( c > 0.0 ) 
        m = (INT) ceil ( ( ( p[0] - T2_LOOP_MIN_X(loop) ) * s 
            - ( p[1] - T2_LOOP_MAX_Y(loop) ) * c ) / h ) + 1 ;
    else 
        m = (INT) ceil ( ( ( p[0] - T2_LOOP_MIN_X(loop) ) * s 
            - ( p[1] - T2_LOOP_MIN_Y(loop) ) * c ) / h ) + 1 ;
    RETURN ( m ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT t2x_loop_inters ( loop, pt, h, c, s, intlist, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_LOOP loop ;
PT2 pt ;
REAL h, c, s ;
DML_LIST *intlist ;
INT m ;
{
    DML_ITEM item ;
    INT k, n = 0 ;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item ) {
        k = t2x_edge_inters ( (T2_EDGE)DML_RECORD(item), 
            pt, h, c, s, intlist, m ) ;
        if ( k < 0 ) 
            RETURN ( k ) ;
        n += k ;
    }
    RETURN ( n ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT t2x_edge_inters ( edge, pt, h, c, s, intlist, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
PT2 pt ;
REAL h, c, s ;
DML_LIST *intlist ;
INT m ;
{
#ifdef CCDK_DEBUG
{
PT2 p ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 10 ) ;
    getch();
}
else
    DISPLAY-- ;
}
#endif
    RETURN ( m2c_xhatch_inters ( T2_EDGE_CURVE(edge), pt, h, c, s, 
        edge, intlist, m ) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

