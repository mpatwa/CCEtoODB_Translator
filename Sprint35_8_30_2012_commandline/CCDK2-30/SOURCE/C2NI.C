/* -2 -3 */
/******************************* C2NI.C *********************************/ 
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <c2nmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_self_inters ( nurb, parm0, parm1, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
DML_LIST inters_list ;
{
    RETURN ( c2s_self_inters ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
            C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
            parm0, parm1, inters_list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_inters_line ( nurb, parm0, parm1, line, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
C2_LINE line ;
DML_LIST inters_list ;
{
    RETURN ( c2s_inters_line ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
        parm0, parm1, line, inters_list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_inters_arc ( nurb, parm0, parm1, arc, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
PARM parm0, parm1 ;
C2_ARC arc ;
DML_LIST inters_list ;
{
    RETURN ( c2s_inters_arc ( C2_NURB_CTPT(nurb), C2_NURB_D(nurb), 
        C2_NURB_KNOT(nurb), C2_NURB_W(nurb), 
        parm0, parm1, arc, inters_list ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_inters_splines ( nurb1, parm10, parm11, 
            nurb2, parm20, parm21, inters_list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb1 ;
PARM parm10, parm11 ;
C2_NURB nurb2 ;
PARM parm20, parm21 ;
DML_LIST inters_list ;
{
    RETURN ( c2s_inters_splines ( 
        C2_NURB_CTPT(nurb1), C2_NURB_N(nurb1), C2_NURB_D(nurb1), 
        C2_NURB_KNOT(nurb1), C2_NURB_W(nurb1), parm10, parm11, 
        C2_NURB_CTPT(nurb2), C2_NURB_N(nurb2), C2_NURB_D(nurb2), 
        C2_NURB_KNOT(nurb2), C2_NURB_W(nurb2), parm20, parm21, 
        inters_list ) ) ;
}


/*------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_compare_knots ( nurb, tol, t_ptr, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
REAL tol ;
REAL *t_ptr ;
INT j ;
{
    REAL *knot ;

    knot = C2_NURB_KNOT(nurb) ; 
    if ( fabs(*t_ptr-knot[j]) <= tol ) 
        *t_ptr = knot[j] ;
    else if ( fabs(*t_ptr-knot[j+1]) <= tol ) 
        *t_ptr = knot[j+1] ;
}    
#endif /* SPLINE */

