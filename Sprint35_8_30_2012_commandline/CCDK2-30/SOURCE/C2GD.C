/* -2 -3 */
/********************************** C2GD .C ********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2vdefs.h>
#include <c2mem.h>
#include <c2gmcrs.h>
#include <c2vmcrs.h>
#include <dmbdefs.h>
GLOBAL      INT             C2_ARC_BUF_SIZE=0 ;
GLOBAL      INT             C2_ARCS_NUMBER=0 ;
GLOBAL      C2_ARC          *C2_ARC_BUFFER ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2g_set_buf_size ( n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n ;
{
    C2_ARC_BUF_SIZE = n ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_alloc ( ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    
    RETURN ( (C2_ARC)dmb_malloc ( (ANY**)&C2_ARC_BUFFER, C2_ARC_BUF_SIZE, 
        &C2_ARCS_NUMBER, sizeof(C2_ARC_S) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_free ( arc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;     /* Arc to be freed, can be NULL */
{
    dmb_free ( arc, (ANY**)&C2_ARC_BUFFER, C2_ARC_BUF_SIZE, 
        &C2_ARCS_NUMBER, PF_NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_copy ( arc0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0 ;
{
    C2_ARC arc1 = c2g_alloc() ;
    RETURN ( c2g_copy ( arc0, arc1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_copy ( arc0, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc ;
{
    C2_ARC_ZERO_RAD(arc) = C2_ARC_ZERO_RAD(arc0) ;
    c2r_copy ( C2_ARC_SEG(arc0), C2_ARC_SEG(arc) ) ;
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2g_offset ( arc0, w, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc0, arc ;
REAL w ;
{
    if ( C2_ARC_ZERO_RAD(arc0) ) {
        C2_ARC_ZERO_RAD(arc) = FALSE ;
        RETURN ( c2r_offset_zero_arc ( C2_ARC_SEG(arc0), w, 
            C2_ARC_SEG(arc) ) ) ;
    }
    if ( c2r_offset ( C2_ARC_SEG(arc0), w, C2_ARC_SEG(arc) ) &&
         c2r_rad ( C2_ARC_SEG(arc) ) > BBS_TOL ) {
#ifdef DEBUG
{
REAL r ;
r = c2r_rad ( C2_ARC_SEG(arc) ) ;
}
#endif
        C2_ARC_ZERO_RAD(arc) = FALSE ;
        RETURN ( TRUE ) ;
    }
    else {
        c2g_copy ( arc0, arc ) ;
        C2_ARC_ZERO_RAD(arc) = TRUE ;
        RETURN ( TRUE ) ;
    }
}

/*----------------------------------------------------------------------*/
BBS_PRIVATE REAL c2g_arc ( ctr, rad, st_angle, sweep, dir, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad, st_angle, sweep ;
INT dir ;
C2_ARC arc ;
{
    if ( IS_SMALL(rad) ) {
        REAL w ;
        C2_ARC_ZERO_RAD(arc) = TRUE ;
        w = fabs ( sin ( 0.5 * sweep ) ) ;
        if ( w <= 0.01 ) 
            w = 0.01 ;
        rad = 0.05 * bbs_get_world_size () / w ;
    }
    else
        C2_ARC_ZERO_RAD(arc) = FALSE ;
    if ( sweep >= TWO_PI - BBS_ZERO ) {
        (void) c2r_arc ( ctr, rad, st_angle, PI, dir, C2_ARC_SEG(arc) ) ;
        RETURN ( 2.0 ) ;
    }
    else {
        (void) c2r_arc ( ctr, rad, st_angle, sweep, dir, C2_ARC_SEG(arc) ) ;
        RETURN ( 1.0 ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2g_arc_pts_bulge ( pt0, pt1, bulge, arc )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1 ;
REAL bulge ;
C2_ARC arc ;
{
    c2r_arc_pts_bulge ( pt0, pt1, bulge, C2_ARC_SEG(arc) ) ;
    C2_ARC_ZERO_RAD(arc) = FALSE ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_arc ( ctr, rad, st_angle, sweep, dir, t1_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad, st_angle, sweep ;
INT dir ;
REAL *t1_ptr ;
{
    C2_ARC arc ;
    arc = c2g_alloc() ;
    if ( arc != NULL ) 
        *t1_ptr = c2g_arc ( ctr, rad, st_angle, sweep, dir, arc ) ;
    RETURN ( arc ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2g_circle ( ctr, rad, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL rad ;
C2_ARC arc ;
{
    C2_ARC_ZERO_RAD(arc) = IS_SMALL(rad) ;
    if ( C2_ARC_ZERO_RAD(arc) )
        rad = 0.1 * bbs_get_world_size () ;
    c2r_circle ( ctr, rad, C2_ARC_SEG(arc) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_circle ( ctr, rad ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT2 ctr ;
REAL rad ;
{
    C2_ARC arc ;
    arc = c2g_alloc() ;
    if ( arc != NULL ) 
        c2g_circle ( ctr, rad, arc ) ;
    RETURN ( arc ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2g_circle_ctr_pt ( ctr, pt, arc ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt ;
C2_ARC arc ;
{
    if ( C2V_IDENT_PTS ( ctr, pt ) ) 
        c2g_circle ( ctr, 0.0, arc ) ;
    else {
        c2r_circle_ctr_pt ( ctr, pt, C2_ARC_SEG(arc) ) ;
        C2_ARC_ZERO_RAD(arc) = FALSE ; 
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_circle_ctr_pt ( ctr, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
PT2 pt ;
{
    C2_ARC arc ;
    arc = c2g_alloc() ;
    if ( arc != NULL ) 
        c2g_circle_ctr_pt ( ctr, pt, arc ) ;
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_3pts ( pt1, pt2, pt3 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt1, pt2, pt3 ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = FALSE ;
    if ( c2r_3pts ( pt1, pt2, pt3, C2_ARC_SEG(arc) ) )
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_ctr_2pts ( ctr, pt0, pt1, dir ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, pt0, pt1 ;
INT dir ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = FALSE ;
/*
    if ( ( dir == 1 ? c2r_2pts_ctr ( pt0, pt1, ctr, C2_ARC_SEG(arc) ) : 
        c2r_2pts_ctr ( pt1, pt0, ctr, C2_ARC_SEG(arc) ) ) != NULL ) 
        RETURN ( arc ) ;
*/
    if ( c2r_2pts_ctr ( pt0, pt1, ctr, dir, C2_ARC_SEG(arc) ) != NULL ) 
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_ctr_2tans ( ctr, rad, tan0, tan1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr, tan0, tan1 ;
REAL rad ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = IS_ZERO(rad) ;
    if ( C2_ARC_ZERO_RAD(arc) )
        rad = 0.1 * bbs_get_world_size () ;
    if ( c2r_ctr_2tans ( ctr, rad, tan0, tan1, C2_ARC_SEG(arc) ) != NULL ) 
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_create_zero_rad ( p0, p1, tan0, tan1, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p0, p1, tan0, tan1 ;
REAL w ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = TRUE ;
    if ( c2r_zero_rad ( p0, p1, tan0, tan1, w, C2_ARC_SEG(arc) ) != NULL ) 
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
}    

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_2pts_tan ( pt0, tan0, pt1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, tan0, pt1 ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = FALSE ;
    if ( c2r_2pts_tan ( pt0, tan0, pt1, C2_ARC_SEG(arc) ) != NULL ) 
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_2pts_tan1 ( pt0, pt1, tan1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, pt1, tan1 ;
{
    PT2 tan0 ;
    C2_ARC arc ;

    C2V_NEGATE ( tan1, tan0 ) ;
    arc = c2g_2pts_tan ( pt1, tan0, pt0 ) ;
    if ( arc != NULL ) 
        c2r_reverse ( C2_ARC_SEG(arc), C2_ARC_SEG(arc) ) ;
    RETURN ( arc ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c2g_2pts_2tans ( pt0, tan0, pt1, tan1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 pt0, tan0, pt1, tan1 ;
{
    C2_ARC arc ;

    arc = c2g_alloc() ;
    if ( arc == NULL ) 
        RETURN ( NULL ) ;
    C2_ARC_ZERO_RAD(arc) = FALSE ;
    if ( c2r_2pts_2tans ( pt0, tan0, pt1, tan1, C2_ARC_SEG(arc) ) != NULL ) 
        RETURN ( arc ) ;
    else {
        C2_FREE_ARC ( arc ) ;
        RETURN ( NULL ) ;
    }
} 

