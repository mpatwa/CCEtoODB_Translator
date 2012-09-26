/* -2 -3 */
/************************************* C2AP.C ******************************/
/***************** Arrays of two-dimensional homogeneous points ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                     All rights reserved                  !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef   SPLINE
#include <c2coned.h>
#include <c2adefs.h>
#include <c2apriv.h>
#include <c2vmcrs.h>
#include <c2conem.h>

STATIC  INT     c2ap_parlgrm_set_vtces __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            INT DUMMY2 , C2_CONE DUMMY3 , PT2* DUMMY4 )) ;
STATIC  BOOLEAN c2ap_parlgrm_boxes_overlap __(( PT2* DUMMY0 , INT DUMMY1 , 
            PT2* DUMMY2 , INT DUMMY3 )) ;
STATIC  void    c2ap_cross_min_max __(( PT2* DUMMY0 , INT DUMMY1 , PT2 DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BOOLEAN c2a_parlgrm_inside ( a, b, cone, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* c is inside of the parallelogram */
PT2 a, b ;
C2_CONE cone ;
PT2 c ;
{
    PT2 vec ;
    
    C2V_SUB ( c, a, vec ) ;
    if ( !c2a_cone_inside ( cone, vec ) ) 
        RETURN ( FALSE ) ;
    C2V_SUB ( b, c, vec ) ;
    if ( !c2a_cone_inside ( cone, vec ) ) 
        RETURN ( FALSE ) ;
    RETURN ( TRUE ) ;
}   
#endif /*NEW_CODE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_parlgrm_w_inside ( a, b, cone, c, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* c is inside of the parallelogram */
PT2 a, b ;
C2_CONE cone ;
PT2 c ;
REAL w ;
{
    PT2 vec ;
    
    C2V_SUB ( c, a, vec ) ;
    if ( !c2a_cone_w_inside ( cone, vec, w ) ) 
        RETURN ( FALSE ) ;
    C2V_SUB ( b, c, vec ) ;
    if ( !c2a_cone_w_inside ( cone, vec, w ) ) 
        RETURN ( FALSE ) ;
    RETURN ( TRUE ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_parlgrm_overlap ( a1, b1, conv1, cone1, 
            a2, b2, conv2, cone2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a1, b1, a2, b2 ;
INT conv1, conv2 ;
C2_CONE cone1, cone2 ;
{
    PT2 *c1, *c2 ;
    INT i1, n1, i2, n2 ;
    
    if ( abs(conv1)>1 || abs(conv2)>1 )
        RETURN ( TRUE ) ;
    c1 = MALLOC ( 4, PT2 ) ;
    if ( c1==NULL ) 
        RETURN ( TRUE ) ;
    c2 = MALLOC ( 4, PT2 ) ;
    if ( c2==NULL ) 
        RETURN ( TRUE ) ;
    n1 = c2ap_parlgrm_set_vtces ( a1, b1, conv1, cone1, c1 ) ;
    n2 = c2ap_parlgrm_set_vtces ( a2, b2, conv2, cone2, c2 ) ;
    if ( !c2ap_parlgrm_boxes_overlap ( c1, n1, c2, n2 ) ) {
        FREE ( c1 ) ;
        FREE ( c2 ) ;
        RETURN ( FALSE ) ;
    }

    for ( i1=1 ; i1<n1 ; i1++ ) {
        for ( i2=1 ; i2<n2 ; i2++ )
            if ( c2a_inters_lines ( c1[i1-1], c1[i1], c2[i2-1], c2[i2] ) ) {
                FREE ( c1 ) ;
                FREE ( c2 ) ;
                RETURN ( TRUE ) ;
            }
        if ( c2a_inters_lines ( c1[i1-1], c1[i1], c2[n2-1], c2[0] ) ) {
                FREE ( c1 ) ;
                FREE ( c2 ) ;
                RETURN ( TRUE ) ;
            }
    }
    for ( i2=1 ; i2<n2 ; i2++ )
        if ( c2a_inters_lines ( c1[n1-1], c1[0], c2[i2-1], c2[i2] ) ) {
                FREE ( c1 ) ;
                FREE ( c2 ) ;
                RETURN ( TRUE ) ;
            }
    if ( c2a_inters_lines ( c1[n1-1], c1[0], c2[n2-1], c2[0] ) ) {
                FREE ( c1 ) ;
                FREE ( c2 ) ;
                RETURN ( TRUE ) ;
            }
    FREE ( c1 ) ;
    FREE ( c2 ) ;
    RETURN ( c2a_pt_in_polygon ( c1, n1, a2 ) || 
             c2a_pt_in_polygon ( c2, n2, a1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_parlgrm_separated ( a1, b1, conv1, cone1, 
            a2, b2, conv2, cone2, dir_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a1, b1, a2, b2 ;
INT conv1, conv2 ;
C2_CONE cone1, cone2 ;
PT2 dir_vec ;
{
    PT2 *c1, *c2 ;
    INT n1, n2 ;
    REAL cross1_min, cross1_max, cross2_min, cross2_max ;

    if ( abs(conv1)>1 || abs(conv2)>1 )
        RETURN ( FALSE ) ;
    c1 = MALLOC ( 4, PT2 ) ;
    if ( c1==NULL ) 
        RETURN ( TRUE ) ;
    c2 = MALLOC ( 4, PT2 ) ;
    if ( c2==NULL ) 
        RETURN ( TRUE ) ;
    n1 = c2ap_parlgrm_set_vtces (a1, b1, conv1, cone1, c1 ) ;
    n2 = c2ap_parlgrm_set_vtces ( a2, b2, conv2, cone2, c2 ) ;
    c2ap_cross_min_max ( c1, n1, dir_vec, &cross1_min, &cross1_max ) ;
    c2ap_cross_min_max ( c2, n2, dir_vec, &cross2_min, &cross2_max ) ;
    FREE ( c1 ) ;
    FREE ( c2 ) ;
    RETURN ( ( cross1_min >= cross2_max - BBS_TOL ) || 
             ( cross2_min >= cross1_max - BBS_TOL ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  void c2ap_cross_min_max ( c, n, dir_vec, cross_min, cross_max )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *c ;
INT n ;
PT2 dir_vec ;
REAL *cross_min, *cross_max ;
{
    INT i ;
    REAL cross ;

    *cross_min = *cross_max = C2V_CROSS ( c[0], dir_vec ) ; ;
    for ( i=1 ; i<n ; i++ ) {
        cross = C2V_CROSS ( c[i], dir_vec ) ; ;
        if ( cross < *cross_min )
            *cross_min = cross ;
        else if ( cross > *cross_max )
            *cross_max = cross ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC  BOOLEAN c2ap_parlgrm_boxes_overlap ( c1, n1, c2, n2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *c1, *c2 ;
INT n1, n2 ;
{
    C2_BOX_S box1, box2 ;

    c2a_box_poly ( c1, n1, &box1 ) ;
    c2a_box_poly ( c2, n2, &box2 ) ;
    RETURN ( c2a_box_overlap ( &box1, &box2 ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC  INT     c2ap_parlgrm_set_vtces ( a, b, conv, cone, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a, b ;
PT2 *c ;
INT conv ;
C2_CONE cone ;
{
    INT n ; 
    PT2 vec ;
    REAL cross, p ;

    C2V_SUB ( b, a, vec ) ;
    cross = C2V_CROSS ( C2_CONE_VEC1(cone), C2_CONE_VEC2(cone) ) ;
    n = 1 ;
    C2V_COPY ( a, c[0] ) ;
    if ( !IS_SMALL ( cross ) ) {
        p = C2V_CROSS ( vec, C2_CONE_VEC2(cone) ) / cross ;
    
        if ( conv >=0 ) {
            C2V_ADDT ( a, C2_CONE_VEC1(cone), p, c[n] ) ;
            n++ ;
        }
        C2V_COPY ( b, c[n] ) ;
        n++ ;
        if ( conv <= 0 ) {
            C2V_ADDT ( b, C2_CONE_VEC1(cone), -p, c[n] ) ;
            n++ ;
        }
    }
    else {
        C2V_COPY ( b, c[n] ) ;
        n++ ;
    }
    RETURN ( n ) ;
}
#endif   /*SPLINE*/

