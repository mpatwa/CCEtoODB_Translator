/* -2 -3 */
/******************************* C2SO.C *********************************/ 
/*************** Two-dimensional spline offset routines *****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2sdefs.h>
#include <c2apriv.h>
#include <c2vdefs.h>
#include <dmldefs.h>
#include <c2vmcrs.h>
#include <c2cxtrm.h>

STATIC  void        append_trim_rec  __(( DML_LIST, PARM, PARM )) ;

/*-----------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_offset ( a, n, d, knot, w, curv_extr_no, curv_extr, 
    parm0, parm1, offset, trim_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
INT curv_extr_no ;
C2_CURV_EXTR curv_extr ;
PARM parm0, parm1 ;
REAL offset ;
DML_LIST trim_list ;
{
    INT i, position ;
    REAL w_total, curvature ;
    PARM_S parm_s[2], prev_parm_s ;
    PARM parm[2], prev_parm = &prev_parm_s;
    C2_CURV_EXTR extr_rec ;

    parm[0] = &(parm_s[0]) ;
    parm[1] = &(parm_s[1]) ;

    w_total = w + offset ;       /* total offset */
    position = 0 ;
    
    if ( c2s_curvature ( a, d, knot, 0.0, parm0, &curvature ) 
        && ( 1.0 + w_total * curvature < 0.0 ) ) {
        position = 0 ;
    }
    else {
        COPY_PARM ( parm0, parm[0] ) ;
        position = 1 ;
    }

    COPY_PARM ( parm0, prev_parm ) ;
    
    for ( i = 0 ; i < curv_extr_no ; i++ ) {
    
        extr_rec = curv_extr + i ;
        curvature = 1.0 + w_total * C2_CURV_EXTR_CURV(extr_rec) ;

        if ( C2_CURV_EXTR_T(extr_rec) < PARM_T(parm0) ) 
            continue ;

        /********************  table of choices ******************
        last segment
        position        curvature       trim by
        1               >0              parm[0], end_parm ;
        1               <0              root, end_parm ;
        0               >0
        0               <0              parm[0], root ;
    
        regular segment
        position        curvature       trim by
        1               <0              parm[0], root ;
        *********************************************************/
    
        else if ( C2_CURV_EXTR_T(extr_rec) > PARM_T(parm1) ) {

            if ( c2s_curvature ( a, d, knot, 0.0, parm1, &curvature ) )
                curvature = 1.0 + w_total * curvature ;
            else 
                curvature = 1.0 ;

            if ( position==1 ) { 
                if ( curvature <= 0.0 && c2s_curv_w_root ( a, n, d, knot, 
                        w, offset, prev_parm, parm1, parm[position] ) ) 
                    append_trim_rec ( trim_list, parm[0], parm[1] ) ;
                else 
                    append_trim_rec ( trim_list, parm[0], parm1 ) ;
            }

            else if ( curvature > 0.0 ) {  /* position==0 && curvature > 0 */
                if ( c2s_curv_w_root ( a, n, d, knot, w, offset, prev_parm, 
                    parm1, parm[position] ) ) 
                    append_trim_rec ( trim_list, parm[0], parm1 ) ;
            }    
            RETURN ( DML_LENGTH(trim_list) ) ;
        }
        else if ( ( position==1 && curvature<-BBS_ZERO ) || 
            ( position==0 && curvature>BBS_ZERO ) ) {
            /* There is a sign change between this parameter value and the 
            previous parameter, so find a root */

            if ( c2s_curv_w_root ( a, n, d, knot, w, offset, prev_parm, 
                C2_CURV_EXTR_PARM(extr_rec), parm[position] ) ) {
                if ( position==1 ) {
                    /* trim the curve and append it to the list */
                    append_trim_rec ( trim_list, parm[0], parm[1] ) ;
                    position = 0 ;
                }
                else 
                    position = 1 ;
            }
        }   

        COPY_PARM ( C2_CURV_EXTR_PARM(extr_rec), prev_parm ) ;

    }
    if ( c2s_curvature ( a, d, knot, 0.0, parm1, &curvature ) )
        curvature = 1.0 + w_total * curvature ;
    else 
        curvature = 1.0 ;

    if ( position==1 ) { 
        if ( curvature <= 0.0 && c2s_curv_w_root ( a, n, d, knot, w, offset, 
                prev_parm, parm1, parm[1] ) ) 
            append_trim_rec ( trim_list, parm[0], parm[1] ) ;
        else 
            append_trim_rec ( trim_list, parm[0], parm1 ) ;
    }

    else if ( curvature > 0.0 ) {  /* position==0 && curvature > 0 */
        if ( c2s_curv_w_root ( a, n, d, knot, w, offset, prev_parm, parm1, 
                parm[position] ) ) 
            append_trim_rec ( trim_list, parm[0], parm1 ) ;
    }    
    RETURN ( DML_LENGTH(trim_list) ) ;
}


/*----------------------------------------------------------------------*/
STATIC void append_trim_rec ( trim_list, parml, parmr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST trim_list ;
PARM parml, parmr ;
{
    PARM trim_parm ;

    trim_parm = MALLOC ( 2, PARM_S ) ;

    PARM_COPY ( parml, trim_parm ) ;
    PARM_COPY ( parmr, trim_parm+1 ) ;
    dml_append_data ( trim_list, trim_parm ) ;
}


/*-----------------------------------------------------------------------*/
BBS_PRIVATE INT c2s_offset_polygon ( a, n, offset, offset_pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n ;
REAL offset ;
PT2 *offset_pt ;
{
    PT2 p, q, diff ;
    INT i ;
    BOOLEAN non_zero ;

    C2V_SUB ( a[1], a[0], q ) ;
    c2v_normalize ( q, q ) ;

    c2a_offset ( a[0], NULL, q, offset, offset_pt[0] ) ;

    for ( i=1 ; i<n-1 ; i++ ) {

        C2V_COPY ( q, p ) ;
        C2V_SUB ( a[i+1], a[i], q ) ;
        non_zero = c2v_normalize ( q, q ) ;

        if ( non_zero && !c2a_offset ( a[i], p, q, offset, offset_pt[i] ) ) {
            c2a_offset ( a[i], p, NULL, offset, offset_pt[i] ) ;
            RETURN ( i ) ;
        }

        C2V_SUB ( offset_pt[i], offset_pt[i-1], diff ) ;
        if ( C2V_IS_ZERO ( diff ) || C2V_DOT ( p, diff ) < 0.0 ) 
            RETURN ( i ) ;
    }

    c2a_offset ( a[n-1], q, NULL, offset, offset_pt[n-1] ) ;
    RETURN ( n ) ;
}
#endif  /* SPLINE */

