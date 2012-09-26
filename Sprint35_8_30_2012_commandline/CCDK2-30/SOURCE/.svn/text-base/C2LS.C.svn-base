/* -2 -3*/
/********************************** C2LS.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ldefs.h>
#include <c2lmcrs.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2l_select ( line, t0, t1, pt, tol, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
REAL t0, t1 ;
PT2 pt ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    PT2 dir_vec, dir_vec1, sel_vec1 ;
    REAL t, u, l;
    INT j0, j1 ;
    
    C2_LINE_DIR_VEC ( line, dir_vec ) ;
	l = c2v_norm ( dir_vec );
	c2v_sub ( pt, C2_LINE_PT0(line), sel_vec1 );
	t = c2v_dot ( sel_vec1, dir_vec ) / ( l * l );
    if ( fabs(dir_vec[0]) > fabs(dir_vec[1]) ) {
        j0 = 0 ;
        j1 = 1 ;
    }
    else {
        j1 = 0 ;
        j0 = 1 ;
    }

    u = tol / fabs(dir_vec[j0]) ;

    if ( ( t < t0 - u ) || ( t > t1 + u ) )
        RETURN ( FALSE ) ;

    if ( t_ptr != NULL ) 
        *t_ptr = t ;

	c2v_addt ( C2_LINE_PT0(line), dir_vec, t, dir_vec1 );
	u = c2v_dist ( dir_vec1, pt );

    if ( fabs(u) > tol ) 
        RETURN ( FALSE ) ;
    if ( dist_ptr != NULL ) 
        *dist_ptr = u;
    RETURN ( TRUE ) ;

#ifdef OLD_CODE
    C2_LINE_DIR_VEC ( line, dir_vec ) ;
    if ( fabs(dir_vec[0]) > fabs(dir_vec[1]) ) {
        j0 = 0 ;
        j1 = 1 ;
    }
    else {
        j1 = 0 ;
        j0 = 1 ;
    }
    t = ( pt[j0] - C2_LINE_PT0(line)[j0] ) / dir_vec[j0] ;
    u = tol / fabs(dir_vec[j0]) ;
    if ( ( t < t0 - u ) || ( t > t1 + u ) )
        RETURN ( FALSE ) ;
    if ( t_ptr != NULL ) 
        *t_ptr = t ;
    u = pt[j1] - C2_LINE_PT0(line)[j1] - t * dir_vec[j1] ;
    if ( fabs(u) > tol ) 
        RETURN ( FALSE ) ;
    if ( dist_ptr != NULL ) 
        *dist_ptr = HYPOT ( u, 
            pt[j0] - C2_LINE_PT0(line)[j0] - t * dir_vec[j0] ) ;
    RETURN ( TRUE ) ;
#endif // OLD_CODE 
}

