/* -2 -3 */
/********************************** C2GW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alwdefs.h>
#include <c2gdefs.h>
#include <c2gmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_get ( file, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
PARM parm0, parm1 ;
{
    C2_ARC arc = c2g_alloc () ;
    if ( !alw_get_array ( file, (REAL *)arc, 5 ) || 
        !alw_get_int ( file, ( INT* ) C2_ARC_ZERO_RAD(arc) ) ||
        !alw_get_parm ( file, parm0 ) ||
        !alw_get_parm ( file, parm1 ) ) {
        c2g_free ( arc ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_put ( file, arc, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C2_ARC arc ;
PARM parm0, parm1 ;
{
    RETURN ( alw_put_array ( file, (REAL*)arc, 5, NULL ) &&
        alw_put_int ( file, C2_ARC_ZERO_RAD(arc), NULL ) &&
        alw_put_parm ( file, parm0, NULL ) &&
        alw_put_parm ( file, parm1, NULL ) ) ;
}

