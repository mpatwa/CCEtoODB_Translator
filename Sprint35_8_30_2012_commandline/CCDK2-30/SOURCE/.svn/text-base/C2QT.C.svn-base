/* -2 -3 */
/********************************** C2QT.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2tdefs.h>
#include <c2qmcrs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2q_rotate_cs ( buffer0, origin, c, s, buffer1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer0, buffer1 ;
PT2 origin ;
REAL c, s ;
{
    INT i ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer0)-1 ; i++ ) {
        c2v_rotate_pt_cs ( C2_PC_BUFFER_PT(buffer0,i), origin, c, s, 
            C2_PC_BUFFER_PT(buffer1,i) ) ;
        C2_PC_BUFFER_D(buffer1,i) = C2_PC_BUFFER_D(buffer0,i) ;
    }
    c2v_rotate_pt_cs ( 
        C2_PC_BUFFER_PT(buffer0,C2_PC_BUFFER_N(buffer0)-1), origin, c, s, 
        C2_PC_BUFFER_PT(buffer1,C2_PC_BUFFER_N(buffer0)-1) ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2q_transform ( buffer0, t, buffer1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer0, buffer1 ;
C2_TRANSFORM t ;
{
    INT i ;
    BOOLEAN pos = c2t_positive ( t ) ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer0)-1 ; i++ ) {
        c2t_eval_pt ( C2_PC_BUFFER_PT(buffer0,i), t, 
            C2_PC_BUFFER_PT(buffer1,i) ) ;
        C2_PC_BUFFER_D(buffer1,i) = 
            pos ? C2_PC_BUFFER_D(buffer0,i) : - C2_PC_BUFFER_D(buffer0,i) ;
    }
    c2t_eval_pt ( C2_PC_BUFFER_PT(buffer0,C2_PC_BUFFER_N(buffer0)-1), 
        t, C2_PC_BUFFER_PT(buffer1,C2_PC_BUFFER_N(buffer0)-1) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2q_translate ( buffer, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 shift ;
{
    INT i ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer) ; i++ ) {
        C2V_ADD ( C2_PC_BUFFER_PT(buffer,i), shift, 
            C2_PC_BUFFER_PT(buffer,i) ) ; 
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2q_scale ( buffer, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
REAL factor ;
PT2 origin ;
{
    INT i ;
    PT2 a ;

    a[0] = ( 1.0 - factor ) * origin[0] ;
    a[1] = ( 1.0 - factor ) * origin[1] ;

    for ( i=0 ; i < C2_PC_BUFFER_N(buffer) ; i++ ) 
        C2V_ADDT ( a, C2_PC_BUFFER_PT(buffer,i), factor, 
            C2_PC_BUFFER_PT(buffer,i) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2q_mirror_dir ( buffer, origin, normal ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
PT2 origin, normal ;
{
    INT i ;

    for ( i=0 ; i < C2_PC_BUFFER_N(buffer) ; i++ ) 
        c2v_mirror_pt ( C2_PC_BUFFER_PT(buffer,i), origin, normal, 
            C2_PC_BUFFER_PT(buffer,i) ) ;
    for ( i=0 ; i < C2_PC_BUFFER_N(buffer) - 1 ; i++ ) 
        C2_PC_BUFFER_D(buffer,i) = -C2_PC_BUFFER_D(buffer,i) ;
}

