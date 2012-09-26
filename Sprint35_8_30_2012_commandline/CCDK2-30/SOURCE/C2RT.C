/* -2 -3 */
/********************************** C2RT.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2rdefs.h>
#include <c2tdefs.h>
#include <c2vdefs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_translate ( arc0, shift, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0 ;
PT2 shift ;
C2_ASEG arc1 ;
{
    C2V_ADD ( C2_ASEG_PT0(arc0), shift, C2_ASEG_PT0(arc1) ) ;
    C2V_ADD ( C2_ASEG_PT1(arc0), shift, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = C2_ASEG_D(arc0) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_scale ( arc0, pt, factor, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0 ;
PT2 pt ;
REAL factor ;
C2_ASEG arc1 ;
{
    C2V_ADDT ( pt, C2_ASEG_PT0(arc0), factor, C2_ASEG_PT0(arc1) ) ;
    C2V_ADDT ( pt, C2_ASEG_PT1(arc0), factor, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = C2_ASEG_D(arc0) * factor ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_rotate_cs ( arc0, origin, c, s, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
PT2 origin ;
REAL c, s ;
{
    c2v_rotate_pt_cs ( C2_ASEG_PT0(arc0), origin, c, s, C2_ASEG_PT0(arc1) ) ;
    c2v_rotate_pt_cs ( C2_ASEG_PT1(arc0), origin, c, s, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = C2_ASEG_D(arc0) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2r_mirror ( arc0, origin, normal, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
PT2 origin, normal ;
{
    c2v_mirror_pt ( C2_ASEG_PT0(arc0), origin, normal, C2_ASEG_PT0(arc1) ) ;
    c2v_mirror_pt ( C2_ASEG_PT1(arc0), origin, normal, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = - C2_ASEG_D(arc0) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2r_transform ( arc0, t, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
C2_TRANSFORM t ;
{
    if ( !c2t_orthogonal ( t ) )
        RETURN ( FALSE ) ;

    c2t_eval_pt ( C2_ASEG_PT0(arc0), t, C2_ASEG_PT0(arc1) ) ;
    c2t_eval_pt ( C2_ASEG_PT1(arc0), t, C2_ASEG_PT1(arc1) ) ;
    C2_ASEG_D(arc1) = c2t_positive ( t ) ? C2_ASEG_D(arc0) : -C2_ASEG_D(arc0) ;
    RETURN ( TRUE ) ;
}


