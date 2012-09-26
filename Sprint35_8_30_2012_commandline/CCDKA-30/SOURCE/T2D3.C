/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2D3.C **********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2dpriv.h>
#include <t2attrd.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2d_append_trim ( edge0, parm0, edge1, parm1, 
            attr, edgelist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_EDGE edge0, edge1 ;
PARM parm0, parm1 ;
ATTR attr ;
DML_LIST edgelist ;
{
    T2_LOOP loop = T2_EDGE_LOOP(edge1) ;
    DML_ITEM item, item0 ;
    T2_EDGE edge, new ;
    BOOLEAN cont_loop = TRUE, done = FALSE ;

    if ( edge0 == edge1 && IS_ZERO(PARM_T(parm0)-PARM_T(parm1)) &&
        !t2c_closed ( edge0 ) ) 
        RETURN ( cont_loop ) ;

    if ( ( DML_LENGTH(T2_LOOP_EDGE_LIST(loop)) == 1 ) && ( edge0 == edge1 ) 
        && ( parm0 != NULL ) && ( parm1 != NULL ) && 
        ( ( PARM_T(parm0) > PARM_T(parm1) ) == (T2_EDGE_DIR(edge0)==1) ) ) {

        new = t2d_trim_edge ( edge0, parm0, NULL ) ;
        if ( new != NULL ) {
            dml_append_data ( edgelist, new ) ;
            T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | attr ;
        }

        new = t2d_trim_edge ( edge0, NULL, parm1 ) ;
        if ( new != NULL ) {
            dml_append_data ( edgelist, new ) ;
            T2_EDGE_ATTR(edge0) = T2_EDGE_ATTR(edge0) | attr ;
        }
        RETURN ( cont_loop ) ;
    }

    item0 = ( edge0 != NULL ? 
        dml_find_data ( T2_LOOP_EDGE_LIST(loop), edge0 ) : 
        dml_first ( T2_LOOP_EDGE_LIST(loop) ) ) ;

    for ( item=item0 ; item!=NULL && !done ; item = DML_NEXT(item) ) {
        edge = (T2_EDGE) DML_RECORD(item) ;
        if ( edge!=edge0 && edge!=edge1 && attr != T2_ATTR_INIT && 
            ( T2_EDGE_ATTR(edge) & attr ) )
            cont_loop = FALSE ; 
        else {
            new = t2d_copy_edge ( edge ) ;
            if ( new == NULL ) 
                RETURN ( cont_loop ) ;
            if ( edge == edge0 ) 
                t2c_trim_edge ( new, parm0, NULL ) ;
            if ( !IS_ZERO ( T2_EDGE_T0(new) - T2_EDGE_T1(new) ) ) {
                if ( dml_append_data ( edgelist, new ) == NULL )
                    RETURN ( cont_loop ) ;
                if ( edge == edge1 && ( edge1 != edge0 || 
                    ( ( PARM_T(parm0) < PARM_T(parm1) ) ==
                        ( T2_EDGE_DIR(edge0) == 1 ) ) ) ) {
                    t2c_trim_edge ( new, NULL, parm1 ) ;
                    done = TRUE ;
                    if ( IS_ZERO ( T2_EDGE_T0(new) - T2_EDGE_T1(new) ) ) {
                        t2d_free_edge ( new ) ;
                        new = NULL ;
                        dml_remove_last ( edgelist ) ;
                    }
                }

                if ( new != NULL && ( ( edge != edge0 && edge != edge1 ) || 
                    ( IS_ZERO ( T2_EDGE_T0(edge) - T2_EDGE_T0(new) ) &&
                      IS_ZERO ( T2_EDGE_T1(edge) - T2_EDGE_T1(new) ) ) ) ) {
                        T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | attr ;
                        if ( T2_EDGE_REF(edge) != NULL ) 
                            T2_EDGE_ATTR(T2_EDGE_REF(edge)) = 
                                T2_EDGE_ATTR(T2_EDGE_REF(edge)) | attr ;
                }
            }
            else {
                t2d_free_edge ( new ) ;
                new = NULL ;
            }
        }
    }

    for ( item=DML_FIRST(T2_LOOP_EDGE_LIST(loop)) ; item!=NULL && !done ; 
        item = DML_NEXT(item) ) {
        edge = (T2_EDGE) DML_RECORD(item) ;
        if ( edge!=edge0 && edge!=edge1 && attr != T2_ATTR_INIT && 
            ( T2_EDGE_ATTR(edge) & attr ) )
            cont_loop = FALSE ; 
        else {
            if ( edge == edge0 ) 
                done = TRUE ;
            new = t2d_copy_edge ( edge ) ;
            if ( new == NULL ) 
                RETURN ( cont_loop ) ;
            if ( dml_append_data ( edgelist, new ) == NULL )
                RETURN ( cont_loop ) ;
            if ( edge == edge1 ) {
                t2c_trim_edge ( new, NULL, parm1 ) ;
                done = TRUE ;
                if ( IS_ZERO ( T2_EDGE_T0(new) - T2_EDGE_T1(new) ) ) {
                    t2d_free_edge ( new ) ;
                    new = NULL ;
                    dml_remove_last ( edgelist ) ;
                }
            }
            if ( new != NULL && ( ( edge != edge0 && edge != edge1 ) || 
                ( IS_ZERO ( T2_EDGE_T0(edge) - T2_EDGE_T0(new) ) &&
                  IS_ZERO ( T2_EDGE_T1(edge) - T2_EDGE_T1(new) ) ) ) ) {
                    T2_EDGE_ATTR(edge) = T2_EDGE_ATTR(edge) | attr ;
                    if ( T2_EDGE_REF(edge) != NULL ) 
                        T2_EDGE_ATTR(T2_EDGE_REF(edge)) = 
                            T2_EDGE_ATTR(T2_EDGE_REF(edge)) | attr ;
            }
        }
    }
    RETURN ( cont_loop ) ;
}

#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

