/* -2 -3 */
/******************************* C2QO.C *********************************/ 
/*************** Two-dimensional spline offset routines *****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <c2edefs.h>
#include <c2qmcrs.h>
#include <c2rmcrs.h>
#include <c2vmcrs.h>
STATIC INT      c2q_offset_pass1 __(( REAL*, INT, REAL, C2_PC_BUFFER )) ;
STATIC INT      c2q_trim_2arcs __(( C2_ASEG, C2_ASEG )) ;
STATIC void     c2q_eval_offset_pt0 __(( C2_ASEG, REAL, PT2 )) ;

/*-----------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_offset ( buffer, k, offset, offset_buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT k ;
REAL offset ;
C2_PC_BUFFER offset_buffer ;
{
    RETURN ( c2q_offset_pass1 ( C2_PC_BUFFER_PT(buffer,k), 
        C2_PC_BUFFER_N(buffer) - k, offset, offset_buffer ) ) ;
}


/*-----------------------------------------------------------------------*/
STATIC INT c2q_offset_pass1 ( a, n, offset, offset_buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *a ;
INT n ;
REAL offset ;
C2_PC_BUFFER offset_buffer ;
{
    INT status, i ;
    REAL *p, s=0.0 ;
    PT2 vec0, vec1 ;
    C2_ASEG arc ;
    C2_ASEG_S arc1 ;

    c2q_eval_offset_pt0 ( (C2_ASEG)a, offset, 
        C2_PC_BUFFER_PT(offset_buffer,0) ) ;

    for ( i=0, p=a ; i<n-1 ; i++, p+=3 ) {
        arc = (C2_ASEG)p ;
        if ( c2r_offset ( arc, offset, &arc1 ) ) {

            if ( i != 0 ) {
                C2V_SUB ( C2_PC_BUFFER_PT1(offset_buffer), 
                    C2_ASEG_PT0(arc), vec0 ) ;
                C2V_SUB ( C2_ASEG_PT0(&arc1), C2_ASEG_PT0(arc), vec1 ) ;
                s = C2V_CROSS ( vec0, vec1 ) / ( offset * offset ) ;
            }
            if ( i==0 || ( IS_ZERO(s) && ( C2V_DOT(vec0,vec1) > 0.0 ) ) ) {
                if ( c2q_add_arc ( offset_buffer, &arc1 ) == NULL ) 
                    RETURN ( i ) ;
            }
            else if ( IS_ZERO(s) ) {
                if ( c2q_trim_2arcs ( C2_PC_BUFFER_ARC1(offset_buffer), 
                    &arc1 ) > 0 ) {
                    if ( c2q_add_arc ( offset_buffer, &arc1 ) == NULL )
                        RETURN ( i+1 ) ;
                }
                else { 
                    if ( c2q_add_arc_tan ( offset_buffer, 
                        C2_ASEG_PT0(&arc1) ) == NULL || 
                        c2q_add_arc ( offset_buffer, &arc1 ) == NULL )
                        RETURN ( i+1 ) ;
                }
            }

            else if ( (s>0.0) == (offset>0.0) ) {
                if ( c2q_add_arc_tan ( offset_buffer, 
                    C2_ASEG_PT0(&arc1) ) == NULL || 
                    c2q_add_arc ( offset_buffer, &arc1 ) == NULL )
                    RETURN ( i+1 ) ;
            }

            else {
                status = c2q_trim_2arcs ( C2_PC_BUFFER_ARC1(offset_buffer), 
                    &arc1 ) ;
                if ( status == 0 )
                    RETURN ( i+1 ) ;
                else if ( status == -1 )
                    RETURN ( i ) ;
                else if ( status == -2 )
                    RETURN ( i+1 ) ;
                else if ( status == -3 )
                    RETURN ( i ) ;
                if ( c2q_add_arc ( offset_buffer, &arc1 ) == NULL )
                    RETURN ( i+1 ) ;
            }
        }
    }

    if ( C2V_IDENT_PTS ( a, a+3*n-3 ) ) {

        C2V_SUB ( C2_PC_BUFFER_PT1(offset_buffer), a, vec0 ) ;
        C2V_SUB ( C2_PC_BUFFER_PT0(offset_buffer), a, vec1 ) ;
        s = C2V_CROSS ( vec0, vec1 ) / ( offset * offset ) ;

        if ( IS_ZERO ( s ) ) ;
        else if ( (s>0.0) == (offset>0.0) ) 
            (void)c2q_add_arc_ctr_pt ( offset_buffer, a, 
                C2_PC_BUFFER_PT0(offset_buffer) ) ;
        else 
            (void) c2q_trim_2arcs ( C2_PC_BUFFER_ARC1(offset_buffer), 
                C2_PC_BUFFER_ARC0(offset_buffer) ) ; 
    }
    RETURN ( n ) ;
}


/*-----------------------------------------------------------------------*/
STATIC INT c2q_trim_2arcs ( arc0, arc1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc0, arc1 ;
{
    INT i, n ;
    REAL t1[2], t2[2] ;
    PT2 pt[2] ;
    INT type[2] ;

    n = c2r_intersect ( arc0, arc1, t1, t2, pt, type ) ;
    if ( n<=0 ) 
        RETURN ( -3 ) ;
    i = 0 ;

    if ( n==2 ) {
        if ( t1[1] > t1[i] ) { 
            if ( t2[1] < t2[i] ) 
                i = 1 ;
            else
                RETURN ( 0 ) ;
        }
        else {
            if ( t2[1] < t2[i] ) 
                RETURN ( 0 ) ;
        }
    }
    if ( t1[i] <= BBS_ZERO ) 
        RETURN ( -1 ) ;
    if ( t2[i] >= 1.0 - BBS_ZERO ) 
        RETURN ( -2 ) ;
    c2r_trim ( arc0, 0.0, t1[i], arc0 ) ;
    c2r_trim ( arc1, t2[i], 1.0, arc1 ) ;
    RETURN ( 1 ) ;
}


/*-----------------------------------------------------------------------*/
STATIC void c2q_eval_offset_pt0 ( arc, offset, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc ;
REAL offset ;
PT2 pt ;
{
    PT2 x[2] ;

    c2r_eval ( arc, 0.0, 1, x ) ;
    (void)c2e_offset ( x, offset, 0, (PT2*)pt ) ;
}

