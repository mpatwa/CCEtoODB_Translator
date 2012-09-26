/* -S -Z -L -T __BBS_MILL__=1 __BBS_TURN__=1 */
/********************************* T2BD.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <c2vmcrs.h>
#include <c2tdefs.h>
#include <dmldefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2odm.h>
#if ( __BBS_MILL__>=1 || __BBS_TURN__>=1 )

/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_OD t2b_od_lookup ( desc, owner, dist0, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_OFFSET_DESC desc ; 
ANY owner ;
REAL dist0, *dist ;
{
    if ( desc == NULL ) {
        *dist = dist0 ;
        RETURN ( NULL ) ;
    }
    else
        RETURN ( t2b_od_lookup_list ( T2_OFFSET_DESC_DLIST(desc), 
            owner, dist0, dist ) ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE T2_OD t2b_od_lookup_list ( distlist, owner, dist0, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST distlist ; 
ANY owner ;
REAL dist0, *dist ;
{
#if ( __BBS_MILL__ >= 3 || __BBS_TURN__ >=3 )
    RETURN ( t2b_od3_lookup_list ( distlist, owner, dist0, dist ) ) ;
#else
    distlist = NULL ;
    owner = NULL ;
    *dist = dist0 ;
    RETURN ( NULL ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2b_od_dist_max ( dist, desc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL dist ;
T2_OFFSET_DESC desc ;
{
    RETURN ( ( desc == NULL ) ? dist : 
        t2b_od_dist_max_list ( dist, T2_OFFSET_DESC_DLIST(desc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL t2b_od_dist_max_list ( dist, distlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL dist ;
DML_LIST distlist ;
{
#if ( __BBS_MILL__ >= 3 || __BBS_TURN__ >= 3 )
    RETURN ( t2b_od3_dist_max_list ( dist, distlist ) ) ;
#else
    distlist = NULL ;
    RETURN ( dist ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2b_free_od ( od )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_OD od;
{
    FREE ( od );
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_ITEM t2b_append_od ( owner, pt, dist, desc )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY owner ;
PT2 pt ;
REAL dist;
T2_OFFSET_DESC desc ;
{
#if ( __BBS_MILL__ >= 3 || __BBS_TURN__ >= 3 )
    RETURN ( t2b_append_od3 ( owner, pt, dist, desc ) ) ;
#else
    desc = NULL ;
    RETURN ( NULL ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_ITEM t2b_append_od_list ( owner, pt, dist, distlist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY owner ;
PT2 pt ;
REAL dist;
DML_LIST distlist ;
{
#if ( __BBS_MILL__ >= 3 || __BBS_TURN__ >= 3 )
    RETURN ( t2b_append_od3_list ( owner, pt, dist, distlist ) ) ;
#else
    distlist = NULL ;
    RETURN ( NULL ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_OD t2b_create_od ( owner, pt, dist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY owner ;
PT2 pt ;
REAL dist;
{
    T2_OD od = MALLOC ( 1, T2_OD_S );
    if ( od != NULL ) {
        T2_OD_OWNER(od) = owner;
        T2_OD_DIST(od) = dist;
        if ( owner == NULL && pt != NULL ) {
            C2V_COPY ( pt, T2_OD_PT(od) ) ;
        }
    }
    RETURN ( od );
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_OFFSET_DESC t2b_create_offset_desc __(( void )) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_OFFSET_DESC desc ;
    desc = MALLOC ( 1, T2_OFFSET_DESC_S ) ;
    if ( desc == NULL ) 
        RETURN ( NULL ) ;
    T2_OFFSET_DESC_MODE(desc) = T2_FILLET ;
    T2_OFFSET_DESC_DLIST(desc) = NULL ;
    RETURN ( desc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC T2_OFFSET_DESC t2b_offset_set_mode ( desc, mode ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_OFFSET_DESC desc ;
T2_OFFSET_MODE mode ;
{
    if ( desc == NULL ) 
        desc = t2b_create_offset_desc () ;
    if ( desc == NULL ) 
        RETURN ( NULL ) ;
#if ( __BBS_MILL__ >= 2 || __BBS_TURN__ >= 2 ) 
    RETURN ( t2b_offset2_set_mode ( desc, mode ) ) ;
#else
    T2_OFFSET_DESC_MODE(desc) = T2_FILLET ;
#endif
#if 0
    RETURN ( desc ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void t2b_free_offset_desc ( desc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
T2_OFFSET_DESC desc ;
{
    dml_destroy_list ( T2_OFFSET_DESC_DLIST(desc), ( PF_ACTION ) t2b_free_od ) ;
    FREE ( desc ) ;
}
#endif /* __BBS_TURN__ >= 1 || __BBS_MILL__ >= 1 */

