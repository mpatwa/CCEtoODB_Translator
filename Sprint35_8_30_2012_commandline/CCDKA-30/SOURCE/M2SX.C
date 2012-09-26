/* __BBS_MILL__=3 __BBS_TURN__=3 */
/********************************** M2SX.C *********************************/
/******************** Routines for processing splines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef SPLINE
#include <c2sdefs.h>
#include <c2apriv.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <dmldefs.h>
#include <m2xdefs.h>
#include <m2sdefs.h>
#include <c2vmcrs.h>
STATIC INT m2s_xhatch_hor_inters __(( HPT2*, INT, INT, REAL*, REAL, 
            PARM, PARM, PT2, REAL, ANY, DML_LIST*, INT )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT m2s_xhatch_inters ( a, n, d, knot, w, parm0, parm1, pt, 
            h, c, s, owner, intlist, m )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot, w ;
PARM parm0, parm1 ;
PT2 pt ;
REAL h, c, s ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    INT i, k ;
    DML_LIST *temp_list ;
    PT2 p0 ;
    M2_XH_REC xh_rec ;
    DML_ITEM item ;

    if ( IS_ZERO(s) && IS_ZERO(c-1.0) ) 
        RETURN ( m2s_xhatch_hor_inters ( a, n, d, knot, w, parm0, parm1, 
            pt, h, owner, intlist, m ) ) ;

    temp_list = CREATE ( m, DML_LIST ) ;
    if ( temp_list == NULL ) 
        RETURN ( 0 ) ;
    for ( i=0 ; i<m ; i++ ) {
        temp_list[i] = dml_create_list () ;
        if ( temp_list[i] == NULL ) 
            RETURN ( 0 ) ;
    }
    c2a_rotate_hpt_cs ( a, n, pt, c, -s, a ) ;
    C2V_SET_ZERO ( p0 ) ;
    k = m2s_xhatch_hor_inters ( a, n, d, knot, w, parm0, parm1, 
        p0, h, owner, temp_list, m ) ;

    for ( i=0 ; i<m ; i++ ) {
        DML_WALK_LIST ( temp_list[i], item ) {
            xh_rec = DML_RECORD(item) ;
            c2v_rotate_pt_cs ( M2_XH_REC_PT(xh_rec), pt, c, s, 
                M2_XH_REC_PT(xh_rec) ) ;
        }
        dml_append_list ( intlist[i], temp_list[i] ) ;
        dml_free_list ( temp_list[i] ) ;
    }
    KILL ( temp_list ) ;
    c2a_rotate_hpt_cs ( a, n, pt, c, s, a ) ;
    RETURN ( k ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT m2s_xhatch_hor_inters ( a, n, d, knot, w, parm0, parm1, pt, h, 
        owner, intlist, m )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot, w ;
PARM parm0, parm1 ;
PT2 pt ;
REAL h ;
ANY owner ;
DML_LIST *intlist ;
INT m ;
{
    PT2 p ;
    INT i, k = 0 ;
    C2_INT_REC ci ;
    DML_ITEM item ;
    DML_LIST inters_list = dml_create_list () ;

    C2V_COPY ( pt, p ) ;

    for ( i=0 ; i < m ; i++, p[1] += h ) {
        c2s_inters_hor_line ( a, n, d, knot, w, parm0, parm1, p, 
            0, inters_list ) ;
        DML_WALK_LIST ( inters_list, item ) {
            ci = DML_RECORD(item) ;
            if ( m2x_append ( intlist[i], C2_INT_REC_PT(ci)[0], 
                C2_INT_REC_PT(ci)[1], C2_INT_REC_T1(ci), C2_INT_REC_J1(ci), 
                    owner, C2_INT_REC_TRANS(ci) && 
                    ( PARM_T(parm0)+BBS_ZERO < C2_INT_REC_T1(ci) ) && 
                    ( C2_INT_REC_T1(ci) < PARM_T(parm1)-BBS_ZERO ) ) == NULL )
                        RETURN ( -1 ) ;
            k++ ;
            c2d_free_int_rec ( ci ) ;
        }
        dml_clear_list ( inters_list ) ;
    }
    dml_free_list ( inters_list ) ;
    RETURN ( k ) ;
}
#endif
