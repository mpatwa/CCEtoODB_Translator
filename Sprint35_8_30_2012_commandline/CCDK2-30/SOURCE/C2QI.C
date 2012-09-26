/* -2 -3 */
/********************************** C2QI.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c2ldefs.h>
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <dmldefs.h>
#include <c2mem.h>
#include <c2gmcrs.h>
#include <c2qmcrs.h>
#include <c2vmcrs.h>

STATIC BOOLEAN c2q_append_int_rec __(( REAL, INT, REAL, INT, 
            PT2, INT, INT, DML_LIST )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_intersect ( buffer1, k1, buffer2, k2, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer1, buffer2 ;
INT *k1, *k2 ;
DML_LIST int_list ;
{
    REAL t1[2], t2[2] ;
    PT2 pt[2] ;
    INT i1, i2, i, type[2], m, n ;

    if ( buffer1 == buffer2 ) 
        RETURN ( c2q_self_inters ( buffer1, k1, int_list ) ) ;

    n = 0 ;
    for ( i1=0 ; i1<C2_PC_BUFFER_N(buffer1)-1 ; i1++ ) {
        for ( i2=0 ; i2<C2_PC_BUFFER_N(buffer2)-1 ; i2++ ) {
            m = c2r_intersect ( C2_PC_BUFFER_ARC(buffer1,i1), 
                C2_PC_BUFFER_ARC(buffer2,i2), t1, t2, pt, type ) ;
            if ( m < 0 ) 
                m = c2r_intersect_coinc ( C2_PC_BUFFER_ARC(buffer1,i1), 
                    C2_PC_BUFFER_ARC(buffer2,i2), t1, t2, pt, type ) ;
            for ( i=0 ; i<m ; i++ ) {
                t1[i] += (REAL)(i1+(*k1)) ;
                t2[i] += (REAL)(i2+(*k2)) ;
                if ( c2q_append_int_rec ( t1[i], i1, t2[i], i2, 
                    pt[i], type[i], n, int_list ) )
                    n++ ;
            }
        }
    }
    *k1 += C2_PC_BUFFER_N(buffer1) - 1 ;
    *k2 += C2_PC_BUFFER_N(buffer2) - 1 ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_self_inters ( buffer, k, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT *k ;
DML_LIST int_list ;
{
    REAL t1[2], t2[2] ;
    PT2 pt[2] ;
    INT i1, i2, i, type[2], m, n ;

    n = 0 ;
    for ( i1=0 ; i1<C2_PC_BUFFER_N(buffer)-1 ; i1++ ) {
        for ( i2=i1+1 ; i2<C2_PC_BUFFER_N(buffer)-1 ; i2++ ) {
            m = c2r_intersect ( C2_PC_BUFFER_ARC(buffer,i1), 
                C2_PC_BUFFER_ARC(buffer,i2), t1, t2, pt, type ) ;
            for ( i=0 ; i<m ; i++ ) {
                t1[i] += (REAL)(i1+(*k)) ;
                t2[i] += (REAL)(i2+(*k)) ;
                if ( !IS_SMALL(t1[i]-t2[i]) && 
                    c2q_append_int_rec ( t1[i], i1, t2[i], i2, 
                        pt[i], type[i], n, int_list ) )
                    n++ ;
            }
        }
    }
    *k += C2_PC_BUFFER_N(buffer) - 1 ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_inters_line ( buffer, k, line, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT *k ;
C2_LINE line ;
DML_LIST int_list ;
{
    REAL t1[2], t2[2] ;
    PT2 pt[2] ;
    INT j, i, type[2], m, n ;

    n = 0 ;
    for ( j=0 ; j<C2_PC_BUFFER_N(buffer)-1 ; j++ ) {
        m = c2r_inters_line ( C2_PC_BUFFER_ARC(buffer,j), 
            line, t1, t2, pt, type ) ;
        for ( i=0 ; i<m ; i++ ) {
            t1[i] += (REAL)(j+(*k)) ;
            if ( c2q_append_int_rec ( t1[i], j, t2[i], 1, 
                pt[i], type[i], n, int_list ) )
                n++ ;
        }
    }
    *k += C2_PC_BUFFER_N(buffer) - 1 ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2q_inters_arc ( buffer, k, arc, full_circle, int_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer ;
INT *k ;
C2_ARC arc ;
BOOLEAN full_circle ;
DML_LIST int_list ;
{
    REAL t1[2], t2[2] ;
    PT2 pt[2] ;
    INT j, i, type[2], m, n ;

    n = 0 ;
    for ( j=0 ; j<C2_PC_BUFFER_N(buffer)-1 ; j++ ) {
        m = c2r_intersect ( C2_PC_BUFFER_ARC(buffer,j), C2_ARC_SEG(arc), 
            t1, t2, pt, type ) ;
        for ( i=0 ; i<m ; i++ ) {
            t1[i] += (REAL)(j+(*k)) ;
            if ( c2q_append_int_rec ( t1[i], j, t2[i], 0, 
                pt[i], type[i], n, int_list ) )
                n++ ;
        }
    }

    if ( full_circle ) {
        C2_ARC_S arc1 ;

        c2r_complement ( C2_ARC_SEG(arc), C2_ARC_SEG(&arc1) ) ;
        for ( j=0 ; j<C2_PC_BUFFER_N(buffer)-1 ; j++ ) {
            m = c2r_intersect ( C2_PC_BUFFER_ARC(buffer,j), C2_ARC_SEG(&arc1), 
                t1, t2, pt, type ) ;
            for ( i=0 ; i<m ; i++ ) {
                t1[i] += (REAL)(j+(*k)) ;
                if ( c2q_append_int_rec ( t1[i], j, t2[i]+1.0, 1, 
                    pt[i], type[i], n, int_list ) )
                n++ ;
            }
        }
    }

    *k += C2_PC_BUFFER_N(buffer) - 1 ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2q_append_int_rec ( t1, i1, t2, i2, pt, type, n, int_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL t1 ;
INT i1 ;
REAL t2 ;
INT i2 ;
PT2 pt ;
INT type, n ;
DML_LIST int_list ;
{
    INT i ;
    DML_ITEM item ;
    C2_INT_REC int_rec ;

    for ( i=0, item=DML_LAST(int_list) ; i<n && item!=NULL ; 
        i++, item=DML_PREV(item) ) {
        int_rec = (C2_INT_REC)DML_RECORD(item) ;
        if ( IS_SMALL(t1-C2_INT_REC_T1(int_rec)) ||
            IS_SMALL(t2-C2_INT_REC_T2(int_rec)) ) {
            if ( type < 0 ) 
                C2_INT_REC_TYPE(int_rec) = type ;
            RETURN ( FALSE ) ;
        }
    }

    int_rec = C2_ALLOC_INT_REC ;
    C2_INT_REC_T1(int_rec) = t1 ;
    C2_INT_REC_J1(int_rec) = i1 ;
    C2_INT_REC_T2(int_rec) = t2 ;
    C2_INT_REC_J2(int_rec) = i2 ;
    C2V_COPY ( pt, C2_INT_REC_PT(int_rec) ) ;
    C2_INT_REC_DIST(int_rec) = 0.0 ;
    C2_INT_REC_TYPE(int_rec) = type ;
    dml_append_data ( int_list, int_rec ) ;
    RETURN ( TRUE ) ;
}

