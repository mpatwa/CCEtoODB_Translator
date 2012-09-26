/* -R -Z -S -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/******************************* T2C1.C *********************************/ 
/**************** Two-dimensional topology ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h> 
#include <fndefs.h>
#include <c2adefs.h> 
#include <c2cdefs.h> 
#include <c2ddefs.h> 
#include <c2vdefs.h> 
#include <c2vmcrs.h> 
#include <m2cdefs.h>
#include <t2cdefs.h>
#include <t2cpriv.h>
#include <t2ddefs.h>
#include <t2attrd.h>
#include <t2xrecm.h>
#include <m2cdefs.h>

#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

BBS_PRIVATE T2_LOC_EXTR_REC t2c_follow_loc_extr_flat __((
    INT dir, INT *dir1, DML_ITEM *item, 
    T2_EDGE *edge, PT2 p, PT2 p1, INT coord ));

BBS_PRIVATE T2_LOC_EXTR_REC t2c_create_loc_extr_rec __(( INT type, 
    INT range, T2_EDGE edge0, REAL t0, T2_EDGE edge1, REAL t1, 
    REAL value ));

BBS_PRIVATE void t2c_loop_heal_lines_coord __(( T2_LOOP loop, INT coord ));

/*----------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_local_extrema_region ( region, coord, list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                       !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989-93 Building Block Software        !!!!!!!*/
/*!!!!!!!!              All rights reserved.                     !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

T2_REGION region;
INT coord;
DML_LIST list;
{   
    RETURN ( dml_length ( list ) ) ;
}            

/*----------------------------------------------------------------------*/
BBS_PUBLIC INT t2c_local_extrema_loop ( loop, coord, list )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                       !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989-93 Building Block Software        !!!!!!!*/
/*!!!!!!!!              All rights reserved.                     !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

T2_LOOP loop;
INT coord;
DML_LIST list;
{   
/* generally used for turning-like profiles; step vertex by vertex 
checking if it is a max, min, or the beginning of a flat; if a flat, 
step vertex by vertex until the flat becomes a local min, or 
inflection; the ends of the loop are always considered extrema */ 

    PT2 p, p1 ;
    INT dir, dir1, type ;
    T2_EDGE edge ;
#ifndef __BRLNDC__
	T2_EDGE edge1 ;
#endif
    DML_ITEM item;
    T2_LOC_EXTR_REC extr_rec ;

    t2c_loop_heal_lines_coord ( loop, coord );

    item = dml_first ( T2_LOOP_EDGE_LIST(loop) ) ;
    edge = dml_record ( item ) ;

    t2c_ept0 ( edge, p ) ;
    t2c_ept1 ( edge, p1 ) ;

    if ( IS_SMALL(p[coord] - p1[coord]) )
        dir = T2_FLAT;
    else 
    {
        if ( p1[coord] > p[coord] )
            dir = T2_UP;
        else
            dir = T2_DOWN;
    }
#ifndef __BRLNDC__
    edge1 = edge ;
#endif
    for ( item = dml_next ( item ); item != NULL; 
        item = dml_next ( item ) )
    {
        edge = dml_record ( item ) ;

        c2v_copy ( p1, p ) ;
        t2c_ept1 ( edge, p1 ) ;

        if ( IS_SMALL(p[coord] - p1[coord]) )
        {
            dir1 = T2_FLAT;
        }
        else 
        {
            if ( p1[coord] > p[coord] )
                dir1 = T2_UP;
            else
                dir1 = T2_DOWN;
        }

        if ( dir != dir1 )
        {
            extr_rec = NULL;

            if ( dir1 == T2_FLAT )
            {
                extr_rec = t2c_follow_loc_extr_flat ( dir, &dir1, 
                    &item, &edge, p, p1, coord );

                if ( extr_rec == NULL )
                    break;
            }
            else
            {
                if ( dir != T2_FLAT )
                {
                    if ( dir == T2_UP )
                        type = T2_LOC_MAX ;
                    else
                        type = T2_LOC_MIN ;

                    extr_rec = t2c_create_loc_extr_rec ( type, T2_POINT_EXTR, 
                        edge, T2_EDGE_T0(edge), ( T2_EDGE ) NULL, 0.0, 
                        p[coord] );
                }
            }

            if ( extr_rec != NULL )
                dml_append_data ( list, ( ANY ) extr_rec ) ;

            dir = dir1 ;
        }
#ifndef __BRLNDC__
        edge1 = edge ;
#endif
    }

    RETURN ( dml_length ( list ) ) ;
}


BBS_PRIVATE T2_LOC_EXTR_REC t2c_follow_loc_extr_flat ( dir, dir1, 
    item, edge, p, p1, coord )

INT dir;
INT *dir1;
DML_ITEM *item;
T2_EDGE *edge;
PT2 p;
PT2 p1;
INT coord;
{
    T2_EDGE ledge;
    INT type;
    T2_LOC_EXTR_REC extr_rec;

    *item = dml_next ( *item );

    if ( *item == NULL )
        return ( NULL );
    
    ledge = *edge;
    *edge = dml_record ( *item );

    c2v_copy ( p1, p );
    t2c_ept1 ( *edge, p1 );

    if ( p1[coord] > p[coord] )
        *dir1 = T2_UP;
    else 
        *dir1 = T2_DOWN;

    if ( *dir1 != dir )
    {
        if ( *dir1 == T2_DOWN )
            type = T2_LOC_MAX;
        else
            type = T2_LOC_MIN;
    }
    else
        type = T2_LOC_INFL;

    extr_rec = t2c_create_loc_extr_rec ( type, T2_INTERVAL_EXTR, 
        ledge, T2_EDGE_T0(ledge), ledge, T2_EDGE_T1(ledge), 
        p[coord] );

    return ( extr_rec );
}

BBS_PRIVATE void t2c_print_loc_extr ( extr_rec )

T2_LOC_EXTR_REC extr_rec;
{
    if ( extr_rec == NULL )
        return;

    printf ( "Extremum record data:\n\n" );
    printf ( "type: %d\n", T2_LOC_EXTR_REC_TYPE(extr_rec) );
    printf ( "range: %d\n", T2_LOC_EXTR_REC_RANGE(extr_rec) );
    printf ( "value: %lf\n", T2_LOC_EXTR_REC_VALUE(extr_rec) );
    printf ( "edge0: %lx t0: %lf\n", 
        T2_PARM_EDGE(T2_LOC_EXTR_REC_TPARM0(extr_rec)),
        T2_PARM_T(T2_LOC_EXTR_REC_TPARM0(extr_rec)) );
    printf ( "edge1: %lx t1: %lf\n\n", 
        T2_PARM_EDGE(T2_LOC_EXTR_REC_TPARM1(extr_rec)),
        T2_PARM_T(T2_LOC_EXTR_REC_TPARM1(extr_rec)) );
}

BBS_PRIVATE T2_LOC_EXTR_REC t2c_create_loc_extr_rec ( type, range, edge0, 
    t0, edge1, t1, value )

INT type;
INT range;
T2_EDGE edge0;
REAL t0;
T2_EDGE edge1;
REAL t1;
REAL value;
{
    T2_LOC_EXTR_REC extr_rec = T2_ALLOC_LOC_EXTR_REC ;

    T2_LOC_EXTR_REC_TYPE(extr_rec) = type;
    T2_LOC_EXTR_REC_RANGE(extr_rec) = range;
    T2_PARM_EDGE(T2_LOC_EXTR_REC_TPARM0(extr_rec)) = edge0;
    T2_PARM_T(T2_LOC_EXTR_REC_TPARM0(extr_rec)) = t0;
    T2_PARM_EDGE(T2_LOC_EXTR_REC_TPARM1(extr_rec)) = edge1;
    T2_PARM_T(T2_LOC_EXTR_REC_TPARM1(extr_rec)) = t1;
    T2_LOC_EXTR_REC_VALUE(extr_rec) = value;

    return ( extr_rec );
}


BBS_PUBLIC INT t2c_loop_edge_pos ( loop, edge )

T2_LOOP loop;
T2_EDGE edge;
{
    INT pos = -1, i = 0;
    DML_ITEM item;
    T2_EDGE edge1;

    if ( ( loop == NULL ) || ( T2_LOOP_EDGE_LIST(loop) == NULL ) )
        return ( pos );

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item )
    {
        edge1 = dml_record ( item );

        if ( edge1 == edge )
            break;

        i++;
    }

    if ( item != NULL )
        pos = i;

    return ( pos );
}

BBS_PUBLIC T2_EDGE t2c_loop_edge_by_pos ( loop, pos )

T2_LOOP loop;
INT pos;
{
    INT i = 0;
    DML_ITEM item;
    T2_EDGE edge;

    if ( ( loop == NULL ) || ( T2_LOOP_EDGE_LIST(loop) == NULL ) )
        return ( NULL );

    if ( pos < 0 )
        return ( NULL );

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item )
    {
        if ( i == pos )
            break;

        i++;
    }

    if ( item != NULL )
    {
        edge = dml_record ( item );

        return ( edge );
    }
    else
        return ( NULL );
}

BBS_PRIVATE void t2c_loop_heal_lines_coord ( loop, coord )

T2_LOOP loop;
INT coord;
{
    DML_ITEM item, item1;
    T2_EDGE edge, edge1;
    PT2 ep0, ep1, e1p0, e1p1;
    C2_CURVE line;

    if ( ( loop == NULL ) || ( T2_LOOP_EDGE_LIST(loop) == NULL ) )
        return;

    for ( item = dml_first ( T2_LOOP_EDGE_LIST(loop) );
            item != NULL;
            item = item1 )
    {
        item1 = dml_next ( item );

        if ( item1 == NULL )
            break;

        edge = dml_record ( item );
        edge1 = dml_record ( item1 );

        if ( ( C2_CURVE_IS_LINE(T2_EDGE_CURVE(edge)) == TRUE ) &&
            ( C2_CURVE_IS_LINE(T2_EDGE_CURVE(edge1)) == TRUE ) )
        {
            t2c_ept0 ( edge, ep0 );
            t2c_ept1 ( edge, ep1 );
            t2c_ept0 ( edge1, e1p0 );
            t2c_ept1 ( edge1, e1p1 );

            if ( IS_SMALL ( ep0[coord] - ep1[coord] ) == FALSE )
                continue;

            if ( IS_SMALL ( e1p0[coord] - e1p1[coord] ) == FALSE )
                continue;

            if ( IS_SMALL ( ep0[coord] - e1p0[coord] ) == FALSE )
                continue;

            line = c2d_line ( ep0, e1p1 );

            c2d_free_curve ( T2_EDGE_CURVE(edge) );
            T2_EDGE_CURVE(edge) = line;
            T2_EDGE_DIR(edge) = 1;

            item = dml_next ( item1 );
            dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item1 );
            item1 = item;

            t2d_free_edge ( edge1 );
        }
    }
}


BBS_PUBLIC void t2c_loop_heal_lines ( loop )

T2_LOOP loop;
{
    DML_ITEM item, item1;
    T2_EDGE edge, edge1;
    PT2 ep0, e1p1, v01, v10;
    C2_CURVE line;
    REAL xp, tol = bbs_get_tol();

    if ( ( loop == NULL ) || ( T2_LOOP_EDGE_LIST(loop) == NULL ) )
        return;

    for ( item = dml_first ( T2_LOOP_EDGE_LIST(loop) );
            item != NULL;
            item = item1 )
    {
        item1 = dml_next ( item );

        if ( item1 == NULL )
            break;

        edge = dml_record ( item );
        edge1 = dml_record ( item1 );

        if ( ( C2_CURVE_IS_LINE(T2_EDGE_CURVE(edge)) == TRUE ) &&
            ( C2_CURVE_IS_LINE(T2_EDGE_CURVE(edge1)) == TRUE ) )
        {
            t2c_etan1 ( edge, v01 );
            t2c_etan0 ( edge1, v10 );

            xp = c2v_cross ( v01, v10 );

            if ( fabs ( xp ) < tol * tol )
            {
                t2c_ept0 ( edge, ep0 );
                t2c_ept1 ( edge1, e1p1 );

                line = c2d_line ( ep0, e1p1 );

                c2d_free_curve ( T2_EDGE_CURVE(edge) );
                T2_EDGE_CURVE(edge) = line;
                T2_EDGE_DIR(edge) = 1;

                item1 = item;
                item = dml_next ( item1 );
                dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item );

                t2d_free_edge ( edge1 );
            }
        }
    }
}


BBS_PUBLIC void t2c_loop_rearrange_to_nontan ( loop  )

T2_LOOP loop;
{
    T2_EDGE edge, edge1;
    REAL xp, tol = bbs_get_tol();
    DML_ITEM item, item1;
    PT2 v0, v1;

    if ( T2_LOOP_CLOSED(loop) == FALSE )
        return;

    DML_WALK_LIST ( T2_LOOP_EDGE_LIST(loop), item )
    {
        item1 = dml_next ( item );

        if ( item1 == NULL )
            break;

        edge = dml_record ( item );
        edge1 = dml_record ( item1 );

        t2c_etan1 ( edge, v1 );
        t2c_etan0 ( edge1, v0 );

        xp = c2v_cross ( v1, v0 );

        if ( fabs ( xp ) > tol )
        {
            dml_rearrange ( T2_LOOP_EDGE_LIST(loop), item1 );
            break;
        }
    }
}


BBS_PRIVATE void t2c_loop_change_convex_corners ( loop, rad, desc  )

T2_LOOP loop;
REAL rad;
T2_OFFSET_DESC desc;
{
    INT i, n, dir, ff = 1;
    DML_ITEM item, item1, item2;
    T2_EDGE edge, edge1;
    C2_CURVE arc, line;
    PT2 a[4];
    REAL r;

/* loop is assumed to be oriented such that the arcs that will be 
changed are convex */

    if ( rad < 0.0 )
    {
        ff = -1 ;
        rad = fabs ( rad );
    }

    for ( item = dml_first ( T2_LOOP_EDGE_LIST(loop) );
            item != NULL; item = item1 )
    {
        item1 = dml_next ( item );

        edge = dml_record ( item );

        if ( C2_CURVE_IS_ARC(T2_EDGE_CURVE(edge)) )
        {
            arc = T2_EDGE_CURVE(edge);

            c2c_get_arc_radius ( arc, &r );

            if ( IS_SMALL ( r - rad ) )
            {
                c2c_get_arc_direction ( arc, &dir );

                if ( ( ( dir == ff ) && ( T2_EDGE_DIR(edge) == 1 ) )
                    || ( ( dir == - ff ) && ( T2_EDGE_DIR(edge) == -1 ) ) )
                {
                    n = ( T2_OFFSET_DESC_MODE(desc) == T2_SHARP ) ? 3 : 4 ;
                    n = m2c_arc_polygon ( arc, n, a ) ;

                    item2 = item;

                    for ( i = 1 ; i < n ; i++ ) 
                    {
                        line = c2d_line ( a[i-1], a[i] ) ;

                        edge1 = t2d_create_edge ( loop, line, 
                            T2_EDGE_DIR(edge) ) ;

                        if ( T2_EDGE_DIR(edge) == 1 )
                            item2 = t2d_insert_edge ( loop, item2, edge1, 
                                FALSE ) ;
                        else
                            item2 = t2d_insert_edge ( loop, item2, edge1, 
                                TRUE ) ;
                    }

                    dml_remove_item ( T2_LOOP_EDGE_LIST(loop), item );
                    t2d_free_edge ( edge );
                }
            }
        }
    }
}

#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */


