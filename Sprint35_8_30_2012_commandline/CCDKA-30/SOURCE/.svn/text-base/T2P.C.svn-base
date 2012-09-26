/* -Z -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2P.C ***********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <t2cdefs.h> 
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2pdefs.h>
#include <t2attrd.h>
#include <c2vmcrs.h>
#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
#endif

#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )
STATIC BOOLEAN t2p_check_last __(( DML_LIST, T2_EDGE )) ; 

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_edge ( zigzag_list, ept, edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
PT2 ept ;
T2_EDGE edge ;
{
    PT2 p0, p1 ;
    DML_LIST edgelist ;
    BOOLEAN new ;
    T2_LOOP zigzag ;

    if ( zigzag_list == NULL )
        RETURN ;
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_edge ( edge, 13 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif

    if ( DML_LENGTH(zigzag_list) == 0 ) 
        new = TRUE ;
    else {
        t2p_check_last ( zigzag_list, edge ) ;
        zigzag = DML_LAST_RECORD(zigzag_list) ;
        t2c_loop_ept1 ( zigzag, p0 ) ;
        t2c_ept0 ( edge, p1 ) ;
        new = !C2V_IDENT_PTS ( p0, p1 ) ;
    }

    if ( new ) {
        edgelist = dml_create_list() ;
        dml_append_data ( edgelist, edge ) ;
        zigzag = t2d_create_loop ( NULL, edgelist ) ;
        dml_append_data ( zigzag_list, zigzag ) ;
    }

    else 
        t2d_append_edge ( zigzag, edge, FALSE ) ;
    if ( ept != NULL )
        t2c_ept1 ( edge, ept ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_chain ( edge0, parm0, edge1, parm1, dir, 
            attr, ept, zigzag_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
BOOLEAN dir ;
ATTR attr ;
PT2 ept ;
DML_LIST zigzag_list ;
{
    DML_LIST edgelist ;
    BOOLEAN cont_loop ;

    edgelist = dml_create_list() ;
    cont_loop = t2p_append_trim_dir ( edge0, parm0, edge1, parm1, 
        dir, attr, edgelist ) ;
    t2p_append_chain1 ( cont_loop, ept, edgelist, zigzag_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_append_chain1 ( cont_loop, ept, edgelist, zigzag_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
BOOLEAN cont_loop ;
PT2 ept ;
DML_LIST edgelist, zigzag_list ;
{
    T2_LOOP zigzag ;
    PT2 p0, p1 ;
    T2_EDGE edge ;
    DML_ITEM item ;

    if ( !cont_loop ) {
        DML_WALK_LIST ( edgelist, item ) {
            edge = DML_RECORD(item) ;
            t2p_append_edge ( zigzag_list, ept, edge ) ;
        }
        dml_free_list ( edgelist ) ;
        RETURN ;
    }


    if ( DML_LENGTH(edgelist) == 0 ) {
        dml_free_list ( edgelist ) ;
        zigzag = NULL ;
    }
    else if ( DML_LENGTH(zigzag_list) == 0 ) {
        zigzag = t2d_create_loop ( NULL, edgelist ) ;
        dml_append_data ( zigzag_list, zigzag ) ;
    }

    else {
        t2p_check_last ( zigzag_list, DML_FIRST_RECORD(edgelist) ) ;
        zigzag = DML_LAST_RECORD(zigzag_list) ;
        if ( DML_LENGTH ( T2_LOOP_EDGE_LIST(zigzag) ) > 0 ) {
            t2c_loop_ept1 ( zigzag, p0 ) ;
            edge = DML_FIRST_RECORD(edgelist) ;
            t2c_ept0 ( edge, p1 ) ;
        }
        if ( DML_LENGTH ( T2_LOOP_EDGE_LIST(zigzag) ) == 0 || 
            C2V_IDENT_PTS ( p0, p1 ) ) {
            t2d_append_edgelist ( zigzag, edgelist ) ;
            dml_free_list ( edgelist ) ;
        }
        else {
            zigzag = t2d_create_loop ( NULL, edgelist ) ;
            dml_append_data ( zigzag_list, zigzag ) ;
        }
    }
    if ( zigzag != NULL && ept != NULL )
        t2c_loop_ept1 ( zigzag, ept ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN t2p_check_last ( zigzag_list, edge )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST zigzag_list ;
T2_EDGE edge ;
{
    T2_LOOP zigzag ;
    T2_EDGE edge0 ;
    PT2 p0, p1 ;

    if ( zigzag_list == NULL || DML_LENGTH(zigzag_list) == 0 ) 
        RETURN ( FALSE ) ;
    zigzag = DML_LAST_RECORD(zigzag_list) ;
    if ( DML_LENGTH ( T2_LOOP_EDGE_LIST(zigzag) ) == 0 ) 
        RETURN ( FALSE ) ;
    edge0 = DML_LAST_RECORD ( T2_LOOP_EDGE_LIST(zigzag) ) ;
    if ( T2_EDGE_ATTR(edge0) & T2_ATTR_LAST ) {
        if ( t2c_ept0 ( edge0, p0 ) && t2c_ept0 ( edge, p1 ) && 
            C2V_IDENT_PTS ( p0, p1 ) ) {
            t2d_free_edge ( edge0 ) ;
            edge0 = NULL ;
            dml_remove_last ( T2_LOOP_EDGE_LIST(zigzag) ) ;
            if ( DML_LENGTH ( T2_LOOP_EDGE_LIST(zigzag) ) == 0 ) {
                t2d_free_loop ( zigzag ) ;
                dml_remove_last ( zigzag_list ) ;
            }
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    else
        RETURN ( FALSE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2p_append_trim_dir ( edge0, parm0, edge1, parm1, dir, 
            attr, edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
BOOLEAN dir ;
ATTR attr ;
DML_LIST edgelist ;
{
    DML_LIST templist ;
    BOOLEAN cont_loop ;
    if ( dir ) {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_chain ( NULL, edge0, parm0, edge1, parm1 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
        if ( IS_ZERO(T2_EDGE_T0(edge0)-PARM_T(parm0)) ) 
            t2p_mark_edge_pr1 ( t2c_prev_edge ( edge0 ) ) ;
        else if ( IS_ZERO(T2_EDGE_T1(edge0)-PARM_T(parm0)) ) 
            t2p_mark_edge_pr1 ( edge0 ) ;
        if ( IS_ZERO(T2_EDGE_T1(edge1)-PARM_T(parm1)) ) 
            t2p_mark_edge_pr0 ( t2c_next_edge ( edge1 ) ) ;
        else if ( IS_ZERO(T2_EDGE_T0(edge1)-PARM_T(parm1)) ) 
            t2p_mark_edge_pr0 ( edge1 ) ;
        cont_loop = t2d_append_trim ( edge0, parm0, edge1, parm1, 
            attr, edgelist ) ;
    }
    else {
#ifdef CCDK_DEBUG
DISPLAY++ ;
if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
    paint_chain ( NULL, edge1, parm1, edge0, parm0 ) ;
    getch ();
}
else
    DISPLAY-- ;
#endif
        if ( IS_ZERO(T2_EDGE_T1(edge0)-PARM_T(parm0)) ) 
            t2p_mark_edge_pr0 ( t2c_next_edge ( edge0 ) ) ;
        else if ( IS_ZERO(T2_EDGE_T0(edge0)-PARM_T(parm0)) ) 
            t2p_mark_edge_pr0 ( edge0 ) ;
        if ( IS_ZERO(T2_EDGE_T0(edge1)-PARM_T(parm1)) ) 
            t2p_mark_edge_pr1 ( t2c_prev_edge ( edge1 ) ) ;
        else if ( IS_ZERO(T2_EDGE_T1(edge1)-PARM_T(parm1)) ) 
            t2p_mark_edge_pr1 ( edge1 ) ;
        templist = dml_create_list();
        cont_loop = t2d_append_trim ( edge1, parm1, edge0, parm0, 
            attr, templist ) ;
        t2c_reverse_edgelist ( templist ) ;
        dml_append_list ( edgelist, templist ) ;
        dml_free_list ( templist ) ;
        templist = NULL ;
    }
    RETURN ( cont_loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_edge_pr0 ( edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    T2_EDGE ref ;
    if ( edge != NULL ) {
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_PROCSD_0 ;
        ref = T2_EDGE_REF(edge) ;
        if ( ref != NULL ) 
            T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) | T2_ATTR_PROCSD_1 ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void t2p_mark_edge_pr1 ( edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge ;
{
    T2_EDGE ref ;
    if ( edge != NULL ) {
        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | T2_ATTR_PROCSD_1 ;
        ref = T2_EDGE_REF(edge) ;
        if ( ref != NULL ) 
            T2_EDGE_ATTR(ref) = T2_EDGE_ATTR(ref) | T2_ATTR_PROCSD_0 ;
    }
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

