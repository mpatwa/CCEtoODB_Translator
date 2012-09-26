/* __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* M2X.C ***********************************/
/***************************  2-D Xhatch ***********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2vmcrs.h>
#include <dmldefs.h>
#include <m2xdefs.h>

STATIC M2_XH_REC m2x_alloc_xh_rec __(( void )) ;
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

/*-------------------------------------------------------------------------*/
STATIC M2_XH_REC m2x_alloc_xh_rec ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    M2_XH_REC xh_rec ;

    xh_rec = M2_ALLOC_XH_REC ;
    RETURN ( xh_rec ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE M2_XH_REC m2x_create_xh_rec ( x, y, t, j, owner, status ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, t ;
INT j ;
BOOLEAN status ;
ANY owner ;
{
    M2_XH_REC xh_record = m2x_alloc_xh_rec() ;

    if ( xh_record != NULL ) {
        M2_XH_REC_X(xh_record) = x ;
        M2_XH_REC_Y(xh_record) = y ;
        M2_XH_REC_T(xh_record) = t ;
        M2_XH_REC_J(xh_record) = j ;
        M2_XH_REC_OWNER(xh_record) = owner ;
        M2_XH_REC_STATUS(xh_record) = status ;
    }
    RETURN ( xh_record ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void m2x_free_xh_rec ( xh_record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
M2_XH_REC xh_record ;
{
    if ( xh_record != NULL ) 
        M2_FREE_XH_REC ( xh_record ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM m2x_append ( intlist, x, y, t, j, owner, status ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST intlist ;
REAL x, y, t ;
ANY owner ;
INT j ;
BOOLEAN status ;
{
    M2_XH_REC xh_record = m2x_create_xh_rec ( x, y, t, j, owner, status ) ;
#ifdef CCDK_DEBUG
{
PT2 p ;
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    C2V_SET ( x, y, p ) ;
    paint_point ( p, 0.03, 11 ) ;
    getch();
}
else
    DISPLAY-- ;
}
#endif
    RETURN ( xh_record == NULL ? NULL : 
        dml_append_data ( intlist, xh_record ) ) ;
}

