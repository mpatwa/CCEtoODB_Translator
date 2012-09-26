/* -2 -3 */
/********************************** C2N1.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <bbsdefs.h>
#ifdef  SPLINE
#include <c2ndefs.h>
#include <c2sdefs.h>
#include <dmldefs.h>
#include <c2nmcrs.h>
#include <c2cxtrm.h>
#include <c2mem.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_CURV_EXTR c2n_create_curv_extr ( t, j, curvature ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL t ;
INT j ;
REAL curvature ;
{
    C2_CURV_EXTR curv_extr ;

    curv_extr = C2_ALLOC_CURV_EXTR ;
    if ( curv_extr == NULL ) 
        RETURN ( NULL ) ;
    C2_CURV_EXTR_T(curv_extr) = t ;
    C2_CURV_EXTR_J(curv_extr) = j ;
    C2_CURV_EXTR_CURV(curv_extr) = curvature ;
    RETURN ( curv_extr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2n_free_curv_extr ( curv_extr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURV_EXTR curv_extr ;
{
    C2_FREE_CURV_EXTR ( curv_extr ) ;
}


/*------------------------------------------------------------------------*/
BBS_PRIVATE INT c2n_setup_curv_extr ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb ;
{
    DML_LIST extr_list ;

    if ( C2_NURB_CURV_EXTR_NO(nurb) > -1 ) 
        RETURN ( C2_NURB_CURV_EXTR_NO(nurb) ) ;
    /* initialize and create the list of curvature extremums */
    extr_list = dml_create_list();
    C2_NURB_CURV_EXTR_NO(nurb) = 
        c2s_curv_extrs ( C2_NURB_CTPT(nurb), C2_NURB_N(nurb), 
        C2_NURB_D(nurb), C2_NURB_KNOT(nurb), extr_list ) ;
    C2_NURB_CURV_EXTR_PTR(nurb) = (C2_CURV_EXTR) dml_create_list_to_array 
        ( extr_list, sizeof(C2_CURV_EXTR_S) ) ;
    dml_destroy_list ( extr_list, ( PF_ACTION ) c2n_free_curv_extr ) ;
    RETURN ( C2_NURB_CURV_EXTR_NO(nurb) ) ;
}

#endif  /*SPLINE*/

