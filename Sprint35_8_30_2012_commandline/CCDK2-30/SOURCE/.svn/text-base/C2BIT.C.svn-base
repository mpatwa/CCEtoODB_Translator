/* -2 -3 */
/*********************************** C2BI.C ********************************/
/*************** Intersection of two-dimensional Bezier curves *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2bdefs.h>
#include <c2ldefs.h>
#include <c2vdefs.h>
#include <dmldefs.h>
#include <c2vmcrs.h>
#include <c2apriv.h>
#include <c2coned.h>

STATIC  INT inters_tan_1 __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , INT DUMMY6 , 
            REAL DUMMY7 , C2_CONE DUMMY8 , PT2 DUMMY9 , PT2 DUMMY10 , 
            HPT2* DUMMY11 , INT DUMMY12 , REAL DUMMY13 , REAL DUMMY14 , 
            REAL DUMMY15 , INT DUMMY16 , INT DUMMY17 , REAL DUMMY18 , 
            C2_CONE DUMMY19 , PT2 DUMMY20 , PT2 DUMMY21 , DML_LIST  DUMMY22 )) ;
STATIC  INT inters_tan_iter __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            PT2 DUMMY3 , PT2 DUMMY4 , REAL DUMMY5 , HPT2* DUMMY6 , 
            INT DUMMY7 , REAL DUMMY8 , PT2 DUMMY9 , PT2 DUMMY10 , 
            REAL DUMMY11 , REAL* DUMMY12 , REAL*  DUMMY13 )) ;
STATIC  INT inters_coinc __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            PT2* DUMMY3 , HPT2* DUMMY4 , INT DUMMY5 , REAL DUMMY6 , 
            PT2* DUMMY7 , REAL* DUMMY8 , REAL* DUMMY9 , INT*  DUMMY10 )) ;
STATIC  INT inters_tan_init __(( HPT2* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , HPT2* DUMMY5 , INT DUMMY6 , 
            REAL DUMMY7 , REAL DUMMY8 , REAL DUMMY9 , REAL* DUMMY10 , 
            REAL*  DUMMY11 )) ;
#ifdef DEBUG
GLOBAL C2B_INTERS_TAN = 0 ;
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2b_inters_tan ( 
    b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
    b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
    inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t01, t11 ;
INT j1 ;
INT conv1 ;
REAL fl1 ;
C2_CONE cone1 ;
PT2 ept01, ept11 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t02, t12 ;
INT j2 ;
INT conv2 ;
REAL fl2 ;
C2_CONE cone2 ;
PT2 ept02, ept12 ;
DML_LIST inters_list ;    /* List of intersection records */
{
    HPT2 *a1, *a2, *bl, *br ;
    PT2 v1, v2, v, ept1[2], ept2[2], rot_origin ;
    INT i, m, n, convl, convr ;
    BOOLEAN new_int = FALSE ;
    REAL dist, t1[2], t2[2], t, fll, flr ;
    C2_CONE_S conel, coner ;
    INT type[2] ;

#ifdef DEBUG
C2B_INTERS_TAN++ ;
#endif

    (void) c2a_dir_vec_cone ( cone1, v1 ) ;
    (void) c2a_dir_vec_cone ( cone2, v2 ) ;
    if ( C2V_DOT ( v1, v2 ) > 0.0 ) {
        C2V_ADD ( v1, v2, v ) ;
    }
    else {
        C2V_SUB ( v1, v2, v ) ;
    }
    c2v_normalize ( v, v ) ;

    a1 = CREATE ( d1, HPT2 ) ;
    if ( a1 == NULL )
        RETURN ( -1 ) ;
    a2 = CREATE ( d2, HPT2 ) ;
    if ( a2 == NULL ) {
        KILL ( a1 ) ;
        RETURN ( -1 ) ;
    }
    C2V_SET_ZERO ( rot_origin ) ;
    c2a_rotate_hpt_cs ( b1, d1, rot_origin, v[0], v[1], a1 ) ;
    c2a_rotate_hpt_cs ( b2, d2, rot_origin, v[0], v[1], a2 ) ;
    C2V_ROTATE_VEC_CS ( ept01, v[0], v[1], ept1[0] ) ;
    C2V_ROTATE_VEC_CS ( ept11, v[0], v[1], ept1[1] ) ;
    C2V_ROTATE_VEC_CS ( ept02, v[0], v[1], ept2[0] ) ;
    C2V_ROTATE_VEC_CS ( ept12, v[0], v[1], ept2[1] ) ;

    m = inters_tan_iter ( a1, d1, w1, ept1[0], ept1[1], BBS_ZERO/(t11-t01), 
            a2, d2, w2, ept2[0], ept2[1], BBS_ZERO/(t12-t02), t1, t2 ) ;
    if ( m > 0 ) {
        c2b_eval_pt ( b1, d1, w1, t1[0], v1 ) ;
        c2b_eval_pt ( b2, d2, w2, t2[0], v2 ) ;
        dist = C2V_DISTL1 ( v1, v2 ) ;
        if ( IS_SMALL(dist) ) {
            C2V_MID_PT ( v1, v2, v ) ;
            new_int = ( c2b_append_inters_rec ( inters_list, t1[0], t01, 
                t11, j1, t2[0], t02, t12, j2, v, dist, m ) > 0 ) ;
        }
        else
            m = -1 ;
    }

    if ( m!=0 ) {
        KILL ( a1 ) ;
        KILL ( a2 ) ;
        RETURN ( m>0 && new_int ? 1 : 0 ) ;
    }

    n = inters_coinc ( a1, d1, w1, ept1, a2, d2, w2, ept2, t1, t2, type ) ;
    m = 0 ;
    for ( i=0 ; i<n ; i++ ) {
        c2b_eval_pt ( b1, d1, w1, t1[i], v1 ) ;
        c2b_eval_pt ( b2, d2, w2, t2[i], v2 ) ;
        dist = C2V_DISTL1 ( v1, v2 ) ;
        if ( IS_SMALL(dist) ) {
            C2V_MID_PT ( v1, v2, v ) ;
            m += c2b_append_inters_rec ( inters_list, t1[i], t01, 
                t11, j1, t2[i], t02, t12, j2, v, dist, -2 ) ;
        }
    }

    KILL ( a1 ) ; 
    KILL ( a2 ) ; 

    if ( n > 0 ) 
        RETURN ( m ) ;
    
    if ( IS_SMALL(fl1) && IS_SMALL(fl2) )
        RETURN ( 0 ) ;

    if ( fl1 > fl2 ) {
        if ( !c2b_sbdv_bez ( b1, d1, w1, t01, t11, conv1, ept01, ept11, 
            &bl, &br, &t, &convl, &convr, v, &fll, &flr, &conel, &coner ) )
            RETURN ( 0 ) ;
        m = inters_tan_1 ( 
            bl, d1, w1, t01, t, j1, convl, fll, &conel, ept01, v, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) +
            inters_tan_1 ( 
            br, d1, w1, t, t11, j1, convr, flr, &coner, v, ept11, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) ;
    }

    else {
        if ( !c2b_sbdv_bez ( b2, d2, w2, t02, t12, conv2, ept02, ept12, 
            &bl, &br, &t, &convl, &convr, v, &fll, &flr, &conel, &coner ) )
            RETURN ( 0 ) ;
        m = inters_tan_1 ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            bl, d2, w2, t02, t, j2, convl, fll, &conel, ept02, v, 
            inters_list ) +
            inters_tan_1 ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            br, d2, w2, t, t12, j2, convr, flr, &coner, v, ept12, 
            inters_list ) ;
    }
    FREE ( bl ) ;
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT inters_coinc ( b1, d1, w1, ept1, b2, d2, w2, ept2, t1, t2, type )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
PT2     ept1[2] ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
PT2     ept2[2] ;
REAL    *t1;   /* Parameter of the intersection point */
REAL    *t2;   /* Parameter of the intersection point */
INT type[2] ;
{
    PT2 vec1, vec2, vec, int_pt[2] ;
    INT i, m ;
    REAL cross, dist ;

    C2V_SUB ( ept1[1], ept1[0], vec1 ) ;
    C2V_SUB ( ept2[1], ept2[0], vec2 ) ;
    C2V_SUB ( ept2[0], ept1[0], vec ) ;
    c2v_normalize ( vec1, vec1 ) ;
    c2v_normalize ( vec2, vec2 ) ;
    cross = C2V_CROSS ( vec1, vec2 ) ;
    dist = C2V_CROSS ( vec, vec1 ) ;
    if ( ( cross * cross > BBS_ZERO ) || ( dist * dist > BBS_TOL ) )
        RETURN ( 0 ) ;

    m = c2l_intersect_coinc ( (C2_LINE)ept1, 0.0, 1.0, (C2_LINE)ept2, 
        0.0, 1.0, t1, t2, int_pt, type ) ;

    for ( i=0 ; i<m ; i++ ) {
        (void)c2b_project ( b1, d1, w1, int_pt[i], t1+i, vec, &dist ) ;
        (void)c2b_project ( b2, d2, w2, int_pt[i], t2+i, vec, &dist ) ;
    }
    RETURN ( m ) ;
}   


/*-------------------------------------------------------------------------*/
STATIC INT inters_tan_1 ( 
    b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
    b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
    inters_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t01, t11 ;
INT j1 ;
INT conv1 ;
REAL fl1 ;
C2_CONE cone1 ;
PT2 ept01, ept11 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t02, t12 ;
INT j2 ;
INT conv2 ;
REAL fl2 ;
C2_CONE cone2 ;
PT2 ept02, ept12 ;
DML_LIST inters_list ;    /* List of intersection records */
{
    if ( !c2a_parlgrm_overlap ( ept01, ept11, conv1, cone1, 
            ept02, ept12, conv2, cone2 ) )
        RETURN ( 0 ) ;  /* No intersections */
    else 
        RETURN ( c2b_inters_tan ( 
            b1, d1, w1, t01, t11, j1, conv1, fl1, cone1, ept01, ept11, 
            b2, d2, w2, t02, t12, j2, conv2, fl2, cone2, ept02, ept12, 
            inters_list ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC INT inters_tan_iter ( b1, d1, w1, ept01, ept11, parm_tol1, 
            b2, d2, w2, ept02, ept12, parm_tol2, t1_ptr, t2_ptr )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
PT2     ept01, ept11 ;
REAL    parm_tol1 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
PT2     ept02, ept12 ;
REAL    parm_tol2 ;
REAL    *t1_ptr;   /* Parameter of the intersection point */
REAL    *t2_ptr;   /* Parameter of the intersection point */
{
    REAL x1_min, x1_max, x2_min, x2_max, x, t1_init, t2_init ;

    if ( ept01[0] < ept11[0] ) {
        x1_min = ept01[0] ;
        x1_max = ept11[0] ;
    }
    else {
        x1_max = ept01[0] ;
        x1_min = ept11[0] ;
    }
    
    if ( ept02[0] < ept12[0] ) {
        x2_min = ept02[0] ;
        x2_max = ept12[0] ;
    }
    else {
        x2_max = ept02[0] ;
        x2_min = ept12[0] ;
    }
    
    if ( x1_max <= x2_min || x2_max <= x1_min ) 
        RETURN ( -1 ) ;
    else
        x = 0.5 * ( MAX ( x1_min, x2_min ) + MIN ( x1_max, x2_max ) ) ;
    t1_init = ( x - ept01[0] ) / ( ept11[0] - ept01[0] ) ;
    t2_init = ( x - ept02[0] ) / ( ept12[0] - ept02[0] ) ;
    /* do not start at the endpoints */

    if ( t1_init < 0.01 ) 
        t1_init = 0.01 ;
    else if ( t1_init > 0.99 ) 
        t1_init = 0.99 ;
    if ( t2_init < 0.01 ) 
        t2_init = 0.01 ;
    else if ( t2_init > 0.99 ) 
        t2_init = 0.99 ;

    if ( x2_max - x2_min < x1_max - x1_min ) 
        RETURN ( inters_tan_init ( b1, d1, w1, t1_init, parm_tol1, 
            b2, d2, w2, t2_init, parm_tol2, t1_ptr, t2_ptr ) ) ;
    else
        RETURN ( inters_tan_init ( b2, d2, w2, t2_init, parm_tol2, 
            b1, d1, w1, t1_init, parm_tol1, t2_ptr, t1_ptr ) ) ;
    
}   


/*-------------------------------------------------------------------------*/
STATIC INT inters_tan_init ( b1, d1, w1, t1_init, parm_tol1, 
            b2, d2, w2, t2_init, parm_tol2, t1_ptr, t2_ptr )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2    *b1 ;  /* Control points of the first segment */
INT     d1 ;    /* Order of the first segment */
REAL    w1 ;    /* Offset of the first segment */
REAL    t1_init ;
REAL    parm_tol1 ;
HPT2    *b2 ;  /* Control points of the second segment */
INT     d2 ;    /* Order of the second segment */
REAL    w2 ;    /* Offset of the second segment */
REAL    t2_init ;
REAL    parm_tol2 ;
REAL    *t1_ptr;   /* Parameter of the intersection point */
REAL    *t2_ptr;   /* Parameter of the intersection point */
{

    INT count1, count2, m=0, m1, d ;
    PT2 x1[3], x2[3] ;
    REAL u1, u2, f0, f1, f2, h, h0, h1, dt1_dt2_1, dt1_dt2_2, dt1_dt2_sq ;

    *t1_ptr = t1_init ;
    *t2_ptr = t2_init ;
    d = ( d1 < d2 ? d2 : d1 ) + 1 ;

    for ( count2=0 ; count2<=MAX_COUNT; count2++ ) {
        if ( count2>=MAX_COUNT ) 
            RETURN ( 0 ) ;

        c2b_eval ( b2, d2, w2, *t2_ptr, 2, x2 ) ;
        
        for ( count1=0 ; ; count1++ ) {
            if ( count1>=MAX_COUNT ) 
                break ;
        /* solve x1(t1) = x2(t2) wrt t1 */
            c2b_eval ( b1, d1, w1, *t1_ptr, 1, x1 ) ;
            if ( IS_SMALL ( x1[1][0] ) )
                break ;
            u1 = *t1_ptr - ( x1[0][0] - x2[0][0] ) / x1[1][0] ;
            if ( u1 < 0.0 ) {
                if ( count1 >= 2 ) 
                    break ;
                u1 = 0.0 ;
            }
            else if ( u1 > 1.0 ) {
                if ( count1 >= 2 ) 
                    break ;
                u1 = 1.0 ;
            }
            if ( fabs ( u1 - *t1_ptr ) < parm_tol1 ) {
                *t1_ptr = u1 ;
                break ;
            }
            *t1_ptr = u1 ;
        }

    /* solve h(t2) = f(t2)/f'(t2)=0, where f=y1(t1)-y2(t2) */
        c2b_eval ( b1, d1, w1, *t1_ptr, 2, x1 ) ;
        dt1_dt2_1 = x2[1][0] / x1[1][0] ;
        dt1_dt2_sq = dt1_dt2_1 * dt1_dt2_1 ;
        dt1_dt2_2 = ( x2[2][0] - x1[2][0] * dt1_dt2_sq ) / x1[1][0] ;

        f0 = x1[0][1] - x2[0][1] ;
        f1 = x1[1][1] * dt1_dt2_1 - x2[1][1] ;
        f2 = x1[2][1] * dt1_dt2_sq + x1[1][1] * dt1_dt2_2 - x2[2][1] ;

        if ( IS_SMALL(f1) || IS_SMALL(f2) ) {
            if ( IS_SMALL(f0) ) {
                if ( dt1_dt2_1 > 0.0 ) {
                    if ( ( fabs ( *t1_ptr ) < parm_tol1 ) && 
                         ( fabs ( *t2_ptr - 1.0 ) < parm_tol2 ) )
                        RETURN ( -1 ) ;
                    else if ( ( fabs ( *t1_ptr - 1.0 ) < parm_tol1 ) && 
                         ( fabs ( *t2_ptr ) < parm_tol2 ) )
                        RETURN ( -1 ) ;
                    else 
                        RETURN ( m ) ;
                }
                else {
                    if ( ( fabs ( *t1_ptr ) < parm_tol1 ) && 
                         ( fabs ( *t2_ptr ) < parm_tol2 ) )
                        RETURN ( -1 ) ;
                    else if ( ( fabs ( *t1_ptr - 1.0 ) < parm_tol1 ) && 
                         ( fabs ( *t2_ptr - 1.0 ) < parm_tol2 ) )
                        RETURN ( -1 ) ;
                    else 
                        RETURN ( m ) ;
                }
            }
            else 
                RETURN ( -1 ) ;
        }

        h0 = f0 / f1 ;
        h1 = f1 / f2 ;
        if ( IS_SMALL(h0) || IS_SMALL(h1) ) 
            RETURN ( IS_SMALL(f0) ? m : -1 ) ;
        h = 1.0 - h0 / h1 ;
        if ( count2 > 0 ) {
            m1 = (INT) ( 1.0 / h + 0.5 ) ;
            if ( m1 != m ) 
                RETURN ( IS_SMALL(f0) ? m : -1 ) ;
            m = m1 ;
        }
        else {
            m = (INT) ( 1.0 / h + 0.5 ) ;
            if ( m > d ) 
                m = -1 ;
        }
        u2 = *t2_ptr - h0 / h ;

        if ( u2 < 0.0 ) {
            u2 = 0.0 ;
            if ( count2 >= 2 ) 
                RETURN ( 0 ) ;
        }
        else if ( u2 > 1.0 ) {
            u2 = 1.0 ;
            if ( count2 >= 2 ) 
                RETURN ( 0 ) ;
        }
        
        if ( fabs ( u2 - *t2_ptr ) < parm_tol2 ) {
            *t2_ptr = u2 ;
            RETURN ( m ) ;
        }
        *t2_ptr = u2 ;
    }
    RETURN ( 0 ) ;
}    

#endif /*SPLINE*/

