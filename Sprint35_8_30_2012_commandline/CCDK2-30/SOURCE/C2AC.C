/* -2 -3 */
/************************************* C2AC.C ******************************/
/***************** Arrays of two-dimensional homogeneous points ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                     All rights reserved                  !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef   SPLINE
#include <c2coned.h>
#include <c2apriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>
#include <c2conem.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2a_flatness ( a, b, cone ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 a, b ;
C2_CONE cone ;
{
    PT2 c ;
    REAL cross, norm_c , cross_1, cross_2 ;

    C2V_SUB ( b, a, c ) ;
    norm_c = C2V_NORM ( c ) ;
    cross = C2_CONE_SIN ( cone ) ;
    if ( cross <= BBS_TOL ) {
        if ( C2_CONE_COS ( cone ) > 0.0 )
            RETURN ( 0.25 * cross * norm_c ) ;
        else 
            RETURN ( norm_c ) ;
    }
    else {
        cross_1 = C2V_CROSS ( C2_CONE_VEC1(cone), c ) ;
        cross_2 = C2V_CROSS ( c, C2_CONE_VEC2(cone) ) ;
        RETURN ( ( cross_1 * cross_2 ) / ( cross * norm_c ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c2a_flat_epts_cone ( ept0, ept1, conv, cone ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ept0, ept1 ;
INT *conv ;
C2_CONE cone ;
{
    RETURN ( abs(*conv) > 1 ? -1.0 : c2a_flatness ( ept0, ept1, cone ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2a_dcone ( a, d, conv, dcone ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT2 *a  ;          /* Input array */
INT  d ;            /* Number of points */
INT *conv ;
C2_CONE dcone ;
{
    PT2 prev_diff, diff ;
    REAL cross, cross1, cross2 ;
    INT i, j, k ;

    if ( d==2 ) {
        C2H_DIFF ( a[1], a[0], C2_CONE_VEC1(dcone) ) ;
        C2V_COPY ( C2_CONE_VEC1(dcone), C2_CONE_VEC2(dcone) ) ;
    }
    else if ( *conv==1 ) {
        C2H_DIFF ( a[1], a[0], C2_CONE_VEC1(dcone) ) ;
        C2H_DIFF ( a[d-1], a[d-2], C2_CONE_VEC2(dcone) ) ;
        if ( C2V_CROSS ( C2_CONE_VEC1(dcone), C2_CONE_VEC2(dcone) ) < 0.0 ) 
            *conv = 2 ; 
    }   

    else if ( *conv==-1 ) {
        C2H_DIFF ( a[1], a[0], C2_CONE_VEC2(dcone) ) ;
        C2H_DIFF ( a[d-1], a[d-2], C2_CONE_VEC1(dcone) ) ;
        if ( C2V_CROSS ( C2_CONE_VEC1(dcone), C2_CONE_VEC2(dcone) ) < 0.0 ) 
            *conv = -2 ; 
    }

    else {
        k = d ;
        *conv = 0 ;
        j = 0 ;
        for ( i=1 ; i<d && j==0 ; i++ ) {
            C2H_DIFF ( a[i], a[i-1], prev_diff ) ;
            if ( c2v_normalize_l1 ( prev_diff, prev_diff ) )
                j=i ;
        }
        for ( i=j+1 ; i<d && *conv == 0 ; i++ ) {
            C2H_DIFF ( a[i], a[i-1], diff ) ;
            if ( c2v_normalize_l1 ( diff, diff ) ) {

                cross = C2V_CROSS ( prev_diff, diff ) ;

                if ( cross > BBS_ZERO ) {
                    *conv = 1 ; 
                    C2V_COPY ( prev_diff, C2_CONE_VEC1(dcone) ) ;
                    C2V_COPY ( diff, C2_CONE_VEC2(dcone) ) ;
                    k = i ;
                }
                else if ( cross < - BBS_ZERO ) {
                    *conv = -1 ; 
                    C2V_COPY ( prev_diff, C2_CONE_VEC2(dcone) ) ;
                    C2V_COPY ( diff, C2_CONE_VEC1(dcone) ) ;
                    k = i ;
                }
            }
        }    
        
        if ( k==d ) {
            C2V_COPY ( prev_diff, C2_CONE_VEC2(dcone) ) ;
            C2V_COPY ( diff, C2_CONE_VEC1(dcone) ) ;
        }

        for ( i=k+1 ; i<d && abs(*conv) <= 1 ; i++ ) {

            C2V_COPY ( diff, prev_diff ) ;
            C2H_DIFF ( a[i], a[i-1], diff ) ;
            cross1 = C2V_CROSS ( C2_CONE_VEC1(dcone), diff ) ;
            cross2 = C2V_CROSS ( diff, C2_CONE_VEC2(dcone) ) ;
                
            if ( cross1 <= 0.0  ) { 
                if ( cross2 <= 0.0 ) /* cone angle ò 180ø */
                    *conv = ( *conv>0 ? 2 : -2 ) ;
                else { 
                    c2v_copy ( diff, C2_CONE_VEC1(dcone) ) ; 
                    if ( cross2 <= BBS_ZERO ) /* new cone angle ò 180ø-î */
                        *conv = ( *conv>0 ? 2 : -2 ) ;
                }
            }
            else if ( cross2 <= 0.0 ) {
                c2v_copy ( diff, C2_CONE_VEC2(dcone) ) ;
                if ( cross1 <= BBS_ZERO ) /* new cone angle ò 180ø-î */
                    *conv = ( *conv>0 ? 2 : -2 ) ;
            }
            if ( abs(*conv) == 1 ) {
                cross = C2V_CROSS ( prev_diff, diff ) ;
                if ( ( *conv==1 && cross < BBS_ZERO ) ||
                     ( *conv==-1 && cross > -BBS_ZERO ) )
                    *conv = 0 ;
            }
        }
    }
    if ( abs(*conv) <= 1 ) {
        c2v_normalize ( C2_CONE_VEC1(dcone), C2_CONE_VEC1(dcone) ) ;
        c2v_normalize ( C2_CONE_VEC2(dcone), C2_CONE_VEC2(dcone) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_dir_vec_cone ( cone, dir_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CONE cone ;
PT2 dir_vec ;
{
    C2V_ADD ( C2_CONE_VEC1(cone), C2_CONE_VEC2(cone), dir_vec ) ;
    RETURN ( c2v_normalize ( dir_vec, dir_vec ) ) ;
}

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BOOLEAN c2a_cone_inside ( cone, vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CONE cone ;
PT2 vec ;
{
    RETURN ( C2V_CROSS ( C2_CONE_VEC1(cone), vec ) >= -BBS_TOL  && 
             C2V_CROSS ( vec, C2_CONE_VEC2(cone) ) >= -BBS_TOL ) ;
}
#endif /*NEW_CODE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_cone_w_inside ( cone, vec, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CONE cone ;
PT2 vec ;
REAL w ;
{
    RETURN ( C2V_CROSS ( C2_CONE_VEC1(cone), vec ) >= -w  && 
             C2V_CROSS ( vec, C2_CONE_VEC2(cone) ) >= -w ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_cone_overlap ( cone1, cone2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* algorithm :
cij = C2V_CROSS ( C2_CONE_VECi(cone1), C2_CONE_VECj(cone2) ) > 0.0 
overlap = (c11!=c21) || (c12!=c22) || (c11!=c12) || (c21!=c22)  */

C2_CONE cone1, cone2 ;
{
    REAL cross ;
    BOOLEAN c11, c12, c21, c22 ;

    cross = C2V_CROSS ( C2_CONE_VEC1(cone1), C2_CONE_VEC1(cone2) ) ;
    if ( IS_SMALL ( cross ) ) 
        RETURN ( TRUE ) ;
    c11 = cross > 0.0 ;

    cross = C2V_CROSS ( C2_CONE_VEC2(cone1), C2_CONE_VEC1(cone2) ) ;
    if ( IS_SMALL ( cross ) ) 
        RETURN ( TRUE ) ;
    c21 = cross > 0.0 ;

    if ( c11 != c21 ) 
        RETURN ( TRUE ) ;

    cross = C2V_CROSS ( C2_CONE_VEC1(cone1), C2_CONE_VEC2(cone2) ) ;
    if ( IS_SMALL ( cross ) ) 
        RETURN ( TRUE ) ;
    c12 = cross > 0.0 ;

    if ( c11 != c12 ) 
        RETURN ( TRUE ) ;

    cross = C2V_CROSS ( C2_CONE_VEC2(cone1), C2_CONE_VEC2(cone2) ) ;
    if ( IS_SMALL ( cross ) ) 
        RETURN ( TRUE ) ;
    c22 = cross > 0.0 ;

    if ( c22 != c12 ) 
        RETURN ( TRUE ) ;

    RETURN ( FALSE ) ;
}

#endif   /*SPLINE*/

