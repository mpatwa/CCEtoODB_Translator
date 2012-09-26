/* -R __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************** T2CW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <alwdefs.h>
#include <c2cdefs.h>
#include <t2cdefs.h>
#include <t2ddefs.h>
#include <dmldefs.h>
#include <c2vmcrs.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_region ( filename, region ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
T2_REGION region ;
{
    RETURN ( region == NULL || 
        t2c_write_looplist ( filename, T2_REGION_LOOP_LIST(region) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_looplist ( filename, looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST looplist ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = t2c_put_looplist ( file, looplist ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_looplist ( file, looplist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST looplist ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    if ( looplist == NULL ) 
        RETURN ( status ) ; 
    DML_WALK_LIST ( looplist, item ) {
        status = t2c_put_loop ( file, DML_RECORD(item) ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_loop ( filename, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
T2_LOOP loop ;
{
    RETURN ( loop == NULL || 
        t2c_write_edgelist ( filename, T2_LOOP_EDGE_LIST(loop) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_loop ( file, loop ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
T2_LOOP loop ;
{
    RETURN ( loop == NULL || 
        t2c_put_edgelist ( file, T2_LOOP_EDGE_LIST(loop) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_edgelist ( filename, edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST edgelist ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = t2c_put_edgelist ( file, edgelist ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_edgelist ( file, edgelist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST edgelist ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    if ( edgelist == NULL ) 
        RETURN ( status ) ; 
    DML_WALK_LIST ( edgelist, item ) {
        status = t2c_put_edge ( file, DML_RECORD(item) ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_edge ( filename, edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
T2_EDGE edge ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = t2c_put_edge ( file, edge ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_edge ( file, edge ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
T2_EDGE edge ;
{
    RETURN ( c2c_put_curve ( file, T2_EDGE_CURVE(edge) ) && 
        alw_put_int ( file, T2_EDGE_DIR(edge), NULL ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_region_attr ( filename, region, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
T2_REGION region ;
ATTR attr ;
{
    RETURN ( region == NULL || 
        t2c_write_looplist_attr ( filename, T2_REGION_LOOP_LIST(region), 
            attr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_looplist_attr ( filename, looplist, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST looplist ;
ATTR attr ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = t2c_put_looplist_attr ( file, looplist, attr ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_looplist_attr ( file, looplist, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST looplist ;
ATTR attr ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    if ( looplist == NULL ) 
        RETURN ( status ) ; 
    DML_WALK_LIST ( looplist, item ) {
        status = t2c_put_loop_attr ( file, DML_RECORD(item), attr ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_loop_attr ( filename, loop, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
T2_LOOP loop ;
ATTR attr ;
{
    RETURN ( loop == NULL || 
        t2c_write_edgelist_attr ( filename, T2_LOOP_EDGE_LIST(loop), attr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_loop_attr ( file, loop, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
T2_LOOP loop ;
ATTR attr ;
{
    RETURN ( loop == NULL || 
        t2c_put_edgelist_attr ( file, T2_LOOP_EDGE_LIST(loop), attr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_write_edgelist_attr ( filename, edgelist, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST edgelist ;
ATTR attr ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = t2c_put_edgelist_attr ( file, edgelist, attr ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_edgelist_attr ( file, edgelist, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST edgelist ;
ATTR attr ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    if ( edgelist == NULL ) 
        RETURN ( status ) ; 
    DML_WALK_LIST ( edgelist, item ) {
        status = t2c_put_edge_attr ( file, DML_RECORD(item), attr ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN t2c_put_edge_attr ( file, edge, attr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
T2_EDGE edge ;
ATTR attr ;
{
    if ( T2_EDGE_ATTR(edge) & attr )
        RETURN ( c2c_put_curve ( file, T2_EDGE_CURVE(edge) ) && 
            alw_put_int ( file, T2_EDGE_DIR(edge), NULL ) ) ;
    else
        RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_get_edge ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    T2_EDGE edge ;
    C2_CURVE curve ;
    INT dir ;

    curve = c2c_get_curve ( file ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( !alw_get_int ( file, &dir ) )
        RETURN ( NULL ) ;
    edge = t2d_create_edge ( NULL, curve, dir ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2c_get_edgelist ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    T2_EDGE edge ;
    DML_LIST edgelist ;

    edgelist = dml_create_list () ;
    for ( edge = t2c_get_edge ( file ) ; edge != NULL ; 
          edge = t2c_get_edge ( file ) )
        dml_append_data ( edgelist, edge ) ;
    RETURN ( edgelist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LOOP t2c_get_loop ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    T2_LOOP loop ;
    DML_LIST edgelist ;

    edgelist = t2c_get_edgelist ( file ) ;
    loop = t2d_create_loop ( NULL, edgelist ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2c_get_looplist ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    T2_LOOP loop ;
    DML_LIST edgelist, list1, looplist ;
    DML_ITEM item ;
    T2_EDGE edge ;
    PT2 p0, p1 ;

    edgelist = t2c_get_edgelist ( file ) ;
    if ( edgelist == NULL ) 
        RETURN ( NULL ) ;
    looplist = dml_create_list () ;
    list1 = NULL ;

    DML_WALK_LIST ( edgelist, item ) {
        edge = DML_RECORD(item) ;
        if ( list1 == NULL ) {
            t2c_ept0 ( edge, p0 ) ;
            list1 = dml_create_list () ;
        }
        dml_append_data ( list1, edge ) ;
        t2c_ept1 ( edge, p1 ) ;
        if ( C2V_IDENT_PTS ( p0, p1 ) ) {
            loop = t2d_create_loop ( NULL, list1 ) ;
            dml_append_data ( looplist, loop ) ;
            list1 = NULL ;
        }
    }
    dml_free_list ( edgelist ) ;
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_EDGE t2c_read_edge ( filename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
{
    FILE* file ;
    T2_EDGE edge ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    edge = t2c_get_edge ( file ) ;
    fclose ( file ) ;
    RETURN ( edge ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_LOOP t2c_read_loop ( filename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
{
    FILE* file ;
    T2_LOOP loop ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    loop = t2c_get_loop ( file ) ;
    fclose ( file ) ;
    RETURN ( loop ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2c_read_looplist ( filename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
{
    FILE* file ;
    DML_LIST looplist ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    looplist = t2c_get_looplist ( file ) ;
    fclose ( file ) ;
    RETURN ( looplist ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_REGION t2c_read_region ( filename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
{
    DML_LIST looplist ;

    looplist = t2c_read_looplist ( filename ) ;
    RETURN ( t2d_create_region ( looplist ) ) ;
}
#endif /* __BBS_MILL__>=1 || __BBS_TURN__>=1 */

