/* -2 -3 */
/******************************* C2DO.C *********************************/
/************ Two-dimensional curve offset routines *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2cdefs.h>
#include <c2ddefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2vmcrs.h>
/*----------------------------------------------------------------------*/
BBS_PUBLIC INT c2d_offset_through ( curve, pt, offset_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_CURVE curve ;
PT2 pt ;
DML_LIST offset_list ;
{
    PT2 vec, tan_vec, c ;
    REAL offset ;
    PARM_S parm ;

    if ( c2c_project ( curve, pt, &parm, NULL ) ) {
        c2c_eval_pt_tan ( curve, &parm, c, tan_vec ) ;
        C2V_SUB ( pt, c, vec ) ;
        offset = C2V_NORM ( vec ) ;
        if ( C2V_CROSS ( vec, tan_vec ) < 0.0 ) 
            offset = - offset ;
        RETURN ( c2d_offset ( curve, offset, offset_list ) ) ;
    }
    else
        RETURN ( 0 ) ;
}




/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_offset_curve_through ( curve0, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve0 ;
PT2 pt ;
{
    PT2 vec, tan_vec, c ;
    REAL offset ;
    PARM_S parm ;

    if ( !c2c_project ( curve0, pt, &parm, NULL ) ) 
        RETURN ( NULL ) ;
    c2c_eval_pt_tan ( curve0, &parm, c, tan_vec ) ;
    C2V_SUB ( pt, c, vec ) ;
    offset = C2V_NORM ( vec ) ;
    if ( C2V_CROSS ( vec, tan_vec ) < 0.0 ) 
        offset = - offset ;
    RETURN ( c2d_offset_curve ( curve0, offset ) ) ;
}


