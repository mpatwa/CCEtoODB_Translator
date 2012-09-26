/* -2 -3 */
/******************************* C2CE.C *********************************/
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2vdefs.h>
#include <c2zdefs.h>
#include <c2adefs.h>
#include <c2edefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval ( curve, parm, p, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
INT p ;
PT2 *x ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_eval ( C2_CURVE_LINE(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_eval ( C2_CURVE_ARC(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_eval ( C2_CURVE_PCURVE(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_ZERO_ARC(curve) ) {
        c2z_eval ( C2_CURVE_ZERO_ARC(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else 
#ifdef  SPLINE
        RETURN ( c2n_eval ( C2_CURVE_NURB(curve), parm, p, x ) ) ;
#else
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_pt ( curve, parm, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
PT2 pt ;
{
    RETURN ( c2c_eval ( curve, parm, 0, (PT2*)pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_tan ( curve, parm, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
PT2 tan_vec ;
{
    RETURN ( c2c_eval_pt_tan ( curve, parm, NULL, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_pt_tan ( curve, parm, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
PT2 pt ;
PT2 tan_vec ;
{
    if ( C2_CURVE_IS_LINE(curve) ) {
        c2l_pt_tan ( C2_CURVE_LINE(curve), PARM_T(parm), pt, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve) ) {
        c2g_pt_tan ( C2_CURVE_ARC(curve), PARM_T(parm), pt, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) {
        c2p_pt_tan ( C2_CURVE_PCURVE(curve), PARM_T(parm), pt, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C2_CURVE_IS_ZERO_ARC(curve) ) 
        RETURN ( c2z_pt_tan ( C2_CURVE_ZERO_ARC(curve), PARM_T(parm), 
            pt, tan_vec ) ) ;
    else 
#ifdef  SPLINE
        RETURN ( c2n_pt_tan ( C2_CURVE_NURB(curve), parm, pt, tan_vec ) ) ;
#else
    RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}   


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c2c_eval_tan_angle ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
{
    PT2 tan_vec ;
    RETURN ( c2c_eval_tan ( curve, parm, tan_vec ) ? 
        c2v_atan2 ( tan_vec ) : 0.0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_ept0 ( curve, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
{
    RETURN ( c2c_eval_pt ( curve, C2_CURVE_PARM0(curve), pt ) ) ;
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_etan0 ( curve, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 tan_vec ;
{
    RETURN ( c2c_eval_tan ( curve, C2_CURVE_PARM0(curve), tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_ept_tan0 ( curve, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
PT2 tan_vec ;
{
    RETURN ( c2c_eval_pt_tan ( curve, C2_CURVE_PARM0(curve), pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c2c_eval_angle0 ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    RETURN ( c2c_eval_tan_angle ( curve, C2_CURVE_PARM0(curve) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_ept1 ( curve, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
{
    RETURN ( c2c_eval_pt ( curve, C2_CURVE_PARM1(curve), pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_etan1 ( curve, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 tan_vec ;
{
    RETURN ( c2c_eval_tan ( curve, C2_CURVE_PARM1(curve), tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c2c_ept_tan1 ( curve, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PT2 pt ;
PT2 tan_vec ;
{
    RETURN ( c2c_eval_pt_tan ( curve, C2_CURVE_PARM1(curve), pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c2c_eval_angle1 ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    RETURN ( c2c_eval_tan_angle ( curve, C2_CURVE_PARM1(curve) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_offset ( curve, parm, p, offset, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
INT p ;
REAL offset ;
PT2 *x ;
{
    PT2 *z ;
    BOOLEAN status ;

    z = CREATE ( p+1, PT2 ) ;
    if ( z==NULL ) 
        RETURN ( FALSE ) ;
    status = c2c_eval ( curve, parm, p+1, z ) && 
        c2e_offset ( z, offset, p, x ) ;
    KILL ( z ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_offset_pt ( curve, parm, offset, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm ;
REAL offset ;
PT2 x ;
{
    PT2 z[2] ;
    RETURN ( c2c_eval ( curve, parm, 1, z ) && 
        c2e_offset ( z, offset, 0, (PT2*)x ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2c_box ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
{
    if ( curve == NULL ) 
        RETURN ;
    else if ( C2_CURVE_IS_LINE(curve) ) 
        c2l_box ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        c2g_box ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        c2p_box ( C2_CURVE_PCURVE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), C2_CURVE_BOX(curve) ) ;
#ifdef  SPLINE
    else 
        c2n_box ( C2_CURVE_NURB(curve), C2_CURVE_BOX(curve) ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_box_curves ( curves, box ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curves ;
C2_BOX box ;
{
    C2_CURVE curve ;
    DML_ITEM item ;

    if ( dml_length(curves) == 0 )
        RETURN ( FALSE ) ;
    curve = (C2_CURVE)DML_FIRST_RECORD(curves) ;
    c2a_box_copy ( C2_CURVE_BOX(curve), box ) ;
    DML_FOR_LOOP ( DML_SECOND(curves), item ) {
        curve = (C2_CURVE)DML_RECORD(item) ;
        c2a_box_append ( C2_CURVE_BOX(curve), box ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_box_curves_expand ( curves, box ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curves ;
C2_BOX box ;
{
    C2_CURVE curve ;
    DML_ITEM item ;

    if ( dml_length(curves) == 0 )
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( curves, item ) {
        curve = (C2_CURVE)DML_RECORD(item) ;
        c2a_box_append ( C2_CURVE_BOX(curve), box ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c2c_eval_equi_parm ( curve, parm0, parm1, 
            n, x, tan0, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
PARM parm0, parm1 ;
INT n ;
PT2 *x, tan0, tan1 ;
{
    REAL step ;
    PARM_S parm ;
    BOOLEAN status ;
    INT i ;

    if ( n <= 1 ) 
        RETURN ( FALSE ) ;

    status = c2c_eval_pt_tan ( curve, 
        parm0 == NULL ? C2_CURVE_PARM0(curve) : parm0, x[0], tan0 ) ;

    step = ( ( parm1 == NULL ? C2_CURVE_T1(curve) : PARM_T(parm1) ) -
             ( parm0 == NULL ? C2_CURVE_T0(curve) : PARM_T(parm0) ) ) / 
             (REAL)(n-1) ; 
    PARM_COPY ( parm0 == NULL ? C2_CURVE_PARM0(curve) : parm0, &parm ) ;
    for ( i=1 ; i<n-1 ; i++ ) {
        PARM_T(&parm) += step ;
        status = status && c2c_eval_pt ( curve, &parm, x[i] ) ;
    }
    status = status && c2c_eval_pt_tan ( curve, 
        parm1 == NULL ? C2_CURVE_PARM1(curve) : parm1, x[n-1], tan1 ) ;
    RETURN ( status ) ;
}

