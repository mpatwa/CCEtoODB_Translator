/* -S -Z -L -T __BBS_MILL__=2 __BBS_TURN__=2 */
/********************************* T2BB.C **********************************/ 
/***************************  2-D Topology *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/ 
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <dmldefs.h>
#include <c2adefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>
#include <c2ddefs.h>
#include <c2vdefs.h>
#include <t2bdefs.h>
#include <t2bpriv.h>
#include <t2cdefs.h>
#include <t2ddefs.h>

#ifdef CCDK_DEBUG
EXTERN INT DISPLAY ;
EXTERN INT DIS_LEVEL ;
EXTERN BOOLEAN DIR ;
#endif
#if ( __BBS_MILL__>=2 || __BBS_TURN__>=2 )

STATIC void     t2b_union_pass1 __(( T2_REGION, DML_LIST, DML_LIST )) ;
STATIC void     t2b_common_pass1 __(( T2_REGION, DML_LIST, DML_LIST )) ;
STATIC void     t2b_diff_pass2 __(( T2_REGION, DML_LIST, DML_LIST )) ;
STATIC DML_LIST t2b_2regions __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 , 
            BOOLEAN DUMMY2 , DML_LIST DUMMY3 )) ; 

#ifdef __OLD_CODE__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_union_rr ( T2_REGION region1, T2_REGION region2, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_REGION copy1, copy2 ;
    copy1 = t2d_copy_region ( region1 ) ;
    copy2 = t2d_copy_region ( region2 ) ;
    t2c_reverse_region ( copy1 ) ;
    t2c_reverse_region ( copy2 ) ;
    result = t2b_2regions ( copy1, copy2, TRUE, result ) ;
    t2d_free_region ( copy1 ) ;
    t2d_free_region ( copy2 ) ;
    RETURN ( result ) ;
}
#endif // __OLD_CODE__

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_union_rl ( T2_REGION region, DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    DML_LIST list0, list1 ;
    T2_REGION region1 ;

    list0 = dml_create_list () ;
    list1 = dml_create_list () ;
    dml_append_data ( list0, t2d_copy_region ( region ) ) ;

    DML_WALK_LIST ( region_list, item ) {
        region1 = DML_RECORD(item) ;
        t2b_union_pass1 ( region1, list0, list1 ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 12 ) ;
        paint_region ( region1, 13 ) ;
        getch();
        DML_WALK_LIST ( list0, item ) {
            paint_region ( DML_RECORD(item), 14 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    }

    if ( result == NULL ) 
        result = dml_create_list () ;
    dml_append_list ( result, list0 ) ;
    dml_free_list ( list0 ) ;
    dml_free_list ( list1 ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_union_pass1 ( T2_REGION region, DML_LIST list0, DML_LIST list1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    T2_REGION region1 ;

    DML_WALK_LIST ( list0, item ) {
        region1 = DML_RECORD(item) ;
        if ( item == DML_FIRST(list0) ) 
            t2b_union_rr ( region, region1, list1 ) ;
        else
            dml_append_data ( list1, t2d_copy_region ( region1 ) ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 9 ) ;
        paint_region ( region1, 10 ) ;
        getch();
        DML_WALK_LIST ( list1, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    }
    dml_apply ( list0, ( PF_ACTION ) t2d_free_region ) ;
    dml_clear_list ( list0 ) ;
    dml_append_list ( list0, list1 ) ;
}

#ifdef __OLD_CODE__

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_union_l ( DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST list0, list1 ;
    DML_ITEM item ;
    T2_REGION region ;

    list0 = dml_create_list () ;
    list1 = dml_create_list () ;
    DML_WALK_LIST ( region_list, item ) {
        region = DML_RECORD(item) ;
        t2b_union_rl ( region, list0, list1 ) ;
        dml_apply ( list0, ( PF_ACTION ) t2d_free_region ) ;
        dml_clear_list ( list0 ) ;
        dml_append_list ( list0, list1 ) ;
    }
    if ( result == NULL ) 
        result = dml_create_list () ;
    dml_append_list ( result, list0 ) ;
    dml_free_list ( list0 ) ;
    dml_free_list ( list1 ) ;
    RETURN ( result ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_common_rr ( T2_REGION region1, T2_REGION region2, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_REGION copy1, copy2 ;
    copy1 = t2d_copy_region ( region1 ) ;
    copy2 = t2d_copy_region ( region2 ) ;
    result = t2b_2regions ( copy1, copy2, FALSE, result ) ;
    t2d_free_region ( copy1 ) ;
    t2d_free_region ( copy2 ) ;
    RETURN ( result ) ;
}

#endif // __OLD_CODE__

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_common_rl ( T2_REGION region, DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    DML_LIST list0, list1 ;
    T2_REGION region1 ;

    list0 = dml_create_list () ;
    list1 = dml_create_list () ;
    dml_append_data ( list0, t2d_copy_region ( region ) ) ;

    DML_WALK_LIST ( region_list, item ) {
        region1 = DML_RECORD(item) ;
        t2b_common_pass1 ( region1, list0, list1 ) ;
        if ( dml_length(list0) == 0 ) 
            break ;
    }
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 12 ) ;
        paint_region ( region1, 13 ) ;
        getch();
        DML_WALK_LIST ( list0, item ) {
            paint_region ( DML_RECORD(item), 14 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    if ( result == NULL ) 
        result = dml_create_list () ;
    dml_append_list ( result, list0 ) ;
    dml_free_list ( list0 ) ;
    dml_free_list ( list1 ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_common_pass1 ( T2_REGION region, DML_LIST list0, DML_LIST list1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    T2_REGION region1 ;

    DML_WALK_LIST ( list0, item ) {
        region1 = DML_RECORD(item) ;
        t2b_common_rr ( region, region1, list1 ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 9 ) ;
        paint_region ( region1, 10 ) ;
        getch();
        DML_WALK_LIST ( list1, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    }
    dml_apply ( list0, ( PF_ACTION ) t2d_free_region ) ;
    dml_clear_list ( list0 ) ;
    dml_append_list ( list0, list1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_common_l ( DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST list0, list1 ;
    DML_ITEM item ;
    T2_REGION region ;

    list0 = dml_create_list () ;
    list1 = dml_create_list () ;
    DML_WALK_LIST ( region_list, item ) {
        region = DML_RECORD(item) ;
        t2b_common_rl ( region, list0, list1 ) ;
        dml_apply ( list0, ( PF_ACTION ) t2d_free_region ) ;
        dml_clear_list ( list0 ) ;
        dml_append_list ( list0, list1 ) ;
        if ( DML_LENGTH(list0) == 0 )
            break ;
    }
    if ( result == NULL ) 
        result = dml_create_list () ;
    dml_append_list ( result, list0 ) ;
    dml_free_list ( list0 ) ;
    dml_free_list ( list1 ) ;
    RETURN ( result ) ;
}

#ifdef __OLD_CODE__

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_diff_rr ( T2_REGION region1, T2_REGION region2, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_REGION copy1, copy2 ;
    copy1 = t2d_copy_region ( region1 ) ;
    copy2 = t2d_copy_region ( region2 ) ;
    t2c_reverse_region ( copy2 ) ;
    result = t2b_2regions ( copy1, copy2, FALSE, result ) ;
    t2d_free_region ( copy1 ) ;
    t2d_free_region ( copy2 ) ;
    RETURN ( result ) ;
}

#endif // __OLD_CODE__


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_diff_rl ( T2_REGION region, DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST union_list ;

    union_list = t2b_union_l ( region_list, NULL ) ;
    result = t2b_diff_pass1 ( region, union_list, result ) ;
    dml_destroy_list ( union_list, ( PF_ACTION ) t2d_free_region ) ;
    RETURN ( result ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_diff_loops ( T2_LOOP loop, DML_LIST looplist, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_REGION region ;
    DML_LIST region_list ;
    DML_ITEM item ;

    region = t2d_loop_to_region ( loop ) ;
    region_list = dml_create_list () ;
    DML_WALK_LIST ( looplist, item ) 
        dml_append_data ( region_list, 
            t2d_loop_to_region ( DML_RECORD(item) ) ) ;
    result = t2b_diff_rl ( region, region_list, result ) ;
    t2d_clear_region ( region ) ;
    dml_apply ( region_list, ( PF_ACTION ) t2d_clear_region ) ;
    dml_free_list ( region_list ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST t2b_diff_loop_rgns ( T2_LOOP loop, DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    T2_REGION region ;

    region = t2d_loop_to_region ( loop ) ;
    result = t2b_diff_rl ( region, region_list, result ) ;
    t2d_clear_region ( region ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST t2b_diff_pass1 ( T2_REGION region, DML_LIST region_list, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST list0, list1 ;
    DML_ITEM item ;
    T2_REGION region1 ;

#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        DML_WALK_LIST ( region_list, item ) {
            paint_region ( DML_RECORD(item), 9 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    list0 = dml_create_list () ;
    list1 = dml_create_list () ;
    dml_append_data ( list0, t2d_copy_region ( region ) ) ;

    DML_WALK_LIST ( region_list, item ) {
        region1 = DML_RECORD(item) ;
        t2b_diff_pass2 ( region1, list0, list1 ) ;
    }

    if ( result == NULL ) 
        result = dml_create_list () ;
    dml_append_list ( result, list0 ) ;
    dml_free_list ( list0 ) ;
    dml_free_list ( list1 ) ;
    RETURN ( result ) ;
}


/*-------------------------------------------------------------------------*/
STATIC void t2b_diff_pass2 ( T2_REGION region, DML_LIST list0, DML_LIST list1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;
    T2_REGION region1 ;

    DML_WALK_LIST ( list0, item ) {
        region1 = DML_RECORD(item) ;
        t2b_diff_rr ( region1, region, list1 ) ;
#ifdef CCDK_DEBUG
{
    DML_ITEM item ;
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region, 9 ) ;
        paint_region ( region1, 10 ) ;
        getch();
        DML_WALK_LIST ( list1, item ) {
            paint_region ( DML_RECORD(item), 11 ) ;
            getch();
        }
    }
    else
        DISPLAY-- ;
}
#endif
    }
    dml_apply ( list0, ( PF_ACTION ) t2d_free_region ) ;
    dml_clear_list ( list0 ) ;
    dml_append_list ( list0, list1 ) ;
}


#ifdef __OLD_CODE__

/*-------------------------------------------------------------------------*/
STATIC DML_LIST t2b_2regions ( T2_REGION region1, T2_REGION region2, BOOLEAN rev_res, DML_LIST result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_LIST looplist ;
#ifdef CCDK_DEBUG
    DISPLAY++ ;
    if ( DIS_LEVEL == -1 || DISPLAY <= DIS_LEVEL ) {
        paint_region ( region1, 11 ) ;
        getch();
        paint_region ( region2, 12 ) ;
        getch();
    }
    else
        DISPLAY-- ;
#endif
    t2b_split ( region1, region2 ) ;
    looplist = t2b_traverse ( region1, region2, NULL ) ;
    if ( rev_res ) 
        dml_apply ( looplist, ( PF_ACTION ) t2c_reverse_loop ) ;
    result = t2b_create_regions ( looplist, result ) ;
    dml_destroy_list ( looplist, ( PF_ACTION ) t2d_free_loop ) ;
    RETURN ( result ) ;
}
#endif // __OLD_CODE__


T2_BSEG t2b_bseg_create ( T2_EDGE edge, C2_CURVE curve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	T2_BSEG r = MALLOC ( 1, T2_BSEG_S );

	T2_BSEG_EDGE(r) = edge;
	T2_BSEG_CURVE(r) = curve;
	T2_BSEG_STATE(r) = 'u';

	return ( r );
}

/*-------------------------------------------------------------------------*/
void t2b_bseg_free ( T2_BSEG bseg )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	FREE ( bseg );
}

/*-------------------------------------------------------------------------*/
T2_BREG t2b_breg_create ( T2_REGION region )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	T2_BREG r = MALLOC ( 1, T2_BREG_S );

	T2_BREG_REGION(r) = region;

	T2_BREG_CLIST(r) = dml_create_list();
	T2_BREG_ILIST(r) = dml_create_list();
	T2_BREG_OLIST(r) = dml_create_list();
	T2_BREG_WLIST(r) = dml_create_list();
	T2_BREG_ALIST(r) = dml_create_list();

	return ( r );
}

/*-------------------------------------------------------------------------*/
void t2b_breg_free ( T2_BREG r )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	dml_destroy_list ( T2_BREG_CLIST(r), ( PF_ACTION ) t2b_bseg_free );
	dml_destroy_list ( T2_BREG_ILIST(r), ( PF_ACTION ) t2b_bseg_free );
	dml_destroy_list ( T2_BREG_OLIST(r), ( PF_ACTION ) t2b_bseg_free );
	dml_destroy_list ( T2_BREG_WLIST(r), ( PF_ACTION ) t2b_bseg_free );
	dml_destroy_list ( T2_BREG_ALIST(r), ( PF_ACTION ) t2b_bseg_free );

	FREE ( r );
}


/*-------------------------------------------------------------------------*/
DML_LIST t2b_union_rr ( T2_REGION r1, T2_REGION r2, DML_LIST reslist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
 	T2_BREG breg1, breg2;
	T2_BSEG bseg;
	DML_ITEM item;
	DML_LIST list;

	if ( reslist == NULL )
		reslist = dml_create_list();
	
	breg1 = t2b_breg_create ( r1 );
	breg2 = t2b_breg_create ( r2 );

 	t2b_boolean_pass1 ( breg1, breg2 );

	t2b_boolean_pass2 ( breg1, breg2 );
	t2b_boolean_pass2 ( breg2, breg1 );

	dml_append_list ( T2_BREG_OLIST(breg1), T2_BREG_OLIST(breg2) );
	dml_append_list ( T2_BREG_OLIST(breg1), T2_BREG_WLIST(breg1) );

	list = dml_create_list();

	DML_WALK_LIST ( T2_BREG_OLIST(breg1), item )
	{
		bseg = dml_record ( item );

		if ( T2_BSEG_CURVE(bseg) != NULL )
			dml_append_data ( list, ( ANY ) T2_BSEG_CURVE(bseg) );
	}

	t2d_build_regions ( list, NULL, 10.0 * bbs_get_tol(), reslist );

	dml_free_list ( list );

	return ( reslist );
}

/*-------------------------------------------------------------------------*/
DML_LIST t2b_diff_rr ( T2_REGION r1, T2_REGION r2, DML_LIST reslist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
 	T2_BREG breg1, breg2;
	T2_BSEG bseg;
	DML_ITEM item;
	DML_LIST list;

	if ( reslist == NULL )
		reslist = dml_create_list();
	
	breg1 = t2b_breg_create ( r1 );
	breg2 = t2b_breg_create ( r2 );

 	t2b_boolean_pass1 ( breg1, breg2 );

	t2b_boolean_pass2 ( breg1, breg2 );
	t2b_boolean_pass2 ( breg2, breg1 );

	dml_append_list ( T2_BREG_OLIST(breg1), T2_BREG_ILIST(breg2) );
	dml_append_list ( T2_BREG_OLIST(breg1), T2_BREG_ALIST(breg1) );

	list = dml_create_list();

	DML_WALK_LIST ( T2_BREG_OLIST(breg1), item )
	{
		bseg = dml_record ( item );

		if ( T2_BSEG_CURVE(bseg) != NULL )
			dml_append_data ( list, ( ANY ) T2_BSEG_CURVE(bseg) );
	}

	t2d_build_regions ( list, NULL, 10.0 * bbs_get_tol(), reslist );

	dml_free_list ( list );

	t2b_breg_free ( breg1 );
	t2b_breg_free ( breg2 );

	return ( reslist );
}

/*-------------------------------------------------------------------------*/
DML_LIST t2b_common_rr ( T2_REGION r1, T2_REGION r2, DML_LIST reslist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
 	T2_BREG breg1, breg2;
	T2_BSEG bseg;
	DML_ITEM item;
	DML_LIST list;

	if ( reslist == NULL )
		reslist = dml_create_list();
	
	breg1 = t2b_breg_create ( r1 );
	breg2 = t2b_breg_create ( r2 );

 	t2b_boolean_pass1 ( breg1, breg2 );

	t2b_boolean_pass2 ( breg1, breg2 );	// ### problem in classifying segments
	t2b_boolean_pass2 ( breg2, breg1 );

	dml_append_list ( T2_BREG_ILIST(breg1), T2_BREG_ILIST(breg2) );
	dml_append_list ( T2_BREG_ILIST(breg1), T2_BREG_WLIST(breg1) );

	list = dml_create_list();

	DML_WALK_LIST ( T2_BREG_ILIST(breg1), item )
	{
		bseg = dml_record ( item );

		if ( T2_BSEG_CURVE(bseg) != NULL )
			dml_append_data ( list, ( ANY ) T2_BSEG_CURVE(bseg) );
	}

	t2d_build_regions ( list, NULL, 10.0 * bbs_get_tol(), reslist );

	dml_free_list ( list );

	return ( reslist );
}

/*-------------------------------------------------------------------------*/
void t2b_boolean_pass2 ( T2_BREG breg1, T2_BREG breg2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	PT2 p;
	T2_PT_POSITION pos;
	C2_CURVE c;
	DML_ITEM item, item1, bseg1item;
	T2_REGION r = T2_BREG_REGION(breg2);
	T2_BSEG bseg, bseg1;
	INT w;
	REAL tol = bbs_get_tol();

	bbs_put_tol ( 0.01 * tol );

	for ( item = dml_first ( T2_BREG_CLIST(breg1) ); item != NULL; item = item1 )
	{
		item1 = dml_next ( item );

		bseg = dml_record ( item );
		c = T2_BSEG_CURVE(bseg);

		c2c_mid_pt ( c, p );
		pos = t2c_pt_pos_region ( r, p ); 
		// ### returns wrong value for segment X
		
		switch ( pos )
		{
			case T2_PT_OUTSIDE:
				dml_append_data ( T2_BREG_OLIST(breg1), bseg );
				dml_remove_item ( T2_BREG_CLIST(breg1), item );
				break;

			case T2_PT_INSIDE:
				dml_append_data ( T2_BREG_ILIST(breg1), bseg );
				dml_remove_item ( T2_BREG_CLIST(breg1), item );
				break;

			case T2_PT_ON_BOUNDARY:

				bseg1item = t2b_boolean_pass2_x ( bseg, T2_BREG_CLIST(breg2), &w );
				bseg1 = dml_record ( bseg1item );

				if ( w == TRUE )
				{
					dml_append_data ( T2_BREG_WLIST(breg1), bseg );
					dml_append_data ( T2_BREG_WLIST(breg2), bseg1 );
				}
				else
				{
					dml_append_data ( T2_BREG_ALIST(breg1), bseg );
					dml_append_data ( T2_BREG_ALIST(breg2), bseg1 );
				}

				dml_remove_item ( T2_BREG_CLIST(breg1), item );
				dml_remove_item ( T2_BREG_CLIST(breg2), bseg1item );
				break;

			default:
				break;
		}	
	}

	bbs_put_tol ( tol );
}


/*-------------------------------------------------------------------------*/
DML_ITEM t2b_boolean_pass2_x ( T2_BSEG bseg, DML_LIST clist, INT *w )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	DML_ITEM bseg1item;
	T2_BSEG bseg1;
	C2_CURVE c;
	PT2 p, v, v1;
	PARM_S parm_s;
	T2_EDGE e;
	REAL sel_tol = 2.0 * bbs_get_tol(), val;

	e = T2_BSEG_EDGE(bseg);

	c2c_mid_parm ( T2_BSEG_CURVE(bseg), &parm_s );
	c2c_eval_pt_tan ( T2_BSEG_CURVE(bseg), &parm_s, p, v );

	if ( T2_EDGE_REVERSED(e) == TRUE )
		c2v_negate ( v, v );

	DML_WALK_LIST ( clist, bseg1item )
	{
		bseg1 = dml_record ( bseg1item );
		c = T2_BSEG_CURVE(bseg1);

		if ( c2c_select ( c, p, sel_tol, &parm_s, NULL ) )
		{

			e = T2_BSEG_EDGE(bseg1);

			c2c_eval_pt_tan ( T2_BSEG_CURVE(bseg1), &parm_s, NULL, v1 );

			if ( T2_EDGE_REVERSED(e) == TRUE )
				c2v_negate ( v1, v1 );

			val = c2v_dot ( v, v1 );

			if ( val > 0.000 )
				*w = TRUE;
			else
				*w = FALSE;

			return ( bseg1item );
		}
	}

	return ( NULL );
}


/*-------------------------------------------------------------------------*/
void t2b_boolean_pass1 ( T2_BREG breg1, T2_BREG breg2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	T2_REGION r1, r2;
	T2_LOOP l;
	T2_EDGE e;
	DML_ITEM litem, eitem;

	r1 = T2_BREG_REGION(breg1);
	r2 = T2_BREG_REGION(breg2);

	DML_WALK_LIST ( T2_REGION_LOOP_LIST(r1), litem )
	{
		l = ( T2_LOOP ) dml_record ( litem );

		DML_WALK_LIST ( T2_LOOP_EDGE_LIST(l), eitem )
		{
			e = ( T2_EDGE ) dml_record ( eitem );
			
			t2b_boolean_pass1_edge ( e, r2, T2_BREG_CLIST(breg1) );
		}
	}

	DML_WALK_LIST ( T2_REGION_LOOP_LIST(r2), litem )
	{
		l = ( T2_LOOP ) dml_record ( litem );

		DML_WALK_LIST ( T2_LOOP_EDGE_LIST(l), eitem )
		{
			e = ( T2_EDGE ) dml_record ( eitem );
			
			t2b_boolean_pass1_edge ( e, r1, T2_BREG_CLIST(breg2) );
		}
	}
	
}

/*-------------------------------------------------------------------------*/
void t2b_boolean_pass1_edge ( T2_EDGE e, T2_REGION r, DML_LIST clist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	T2_EDGE e1;
	T2_LOOP l;
	DML_ITEM litem, eitem;
	C2_CURVE c, c1;
	DML_LIST intlist, intlist1;

	intlist = dml_create_list();
	intlist1 = dml_create_list();

	c = T2_EDGE_CURVE(e);

	DML_WALK_LIST ( T2_REGION_LOOP_LIST(r), litem )
	{
		l = ( T2_LOOP ) dml_record ( litem );

		DML_WALK_LIST ( T2_LOOP_EDGE_LIST(l), eitem )
		{
			e1 = ( T2_EDGE ) dml_record ( eitem );
			
			c1 = T2_EDGE_CURVE(e1);

			c2c_intersect ( c, c1, intlist );

			if ( dml_length ( intlist ) > 0 )
			{
				dml_append_list ( intlist1, intlist );
			}
		}
	}

	t2b_boolean_pass1_curve ( e, c, intlist1, clist );

	dml_free_list ( intlist1 );
}

/*-------------------------------------------------------------------------*/
BOOLEAN t2b_boolean_pass1_sortkey ( C2_INT_REC ci1, C2_INT_REC ci2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	if ( C2_INT_REC_T1(ci1) < C2_INT_REC_T1(ci2) )
		return ( TRUE );
	else
		return ( FALSE );
} 

/*-------------------------------------------------------------------------*/
void t2b_boolean_pass1_curve ( T2_EDGE e, C2_CURVE c, DML_LIST intlist1, DML_LIST clist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	DML_ITEM item;
	C2_INT_REC ci0 = NULL, ci1;
	PARM parm0 = NULL, parm1;
	REAL tol, length ;
	C2_CURVE c1;
	T2_BSEG bseg;

	dml_sort_list ( intlist1, ( PF_SORT ) t2b_boolean_pass1_sortkey );

	DML_WALK_LIST ( intlist1, item )
	{
		ci1 = ( C2_INT_REC ) dml_record ( item );
		parm1 = C2_INT_REC_PARM1(ci1);

		c1 = c2d_trim ( c, parm0, parm1 );

		if ( c1 != NULL )
		{
			if ( c2c_length ( c1 ) > bbs_get_tol() )
			{
				bseg = t2b_bseg_create ( e, c1 );
				dml_append_data ( clist, ( ANY ) bseg );
			}
			else
				c2d_free_curve ( c1 );
		} 
			 
		ci0 = ci1;
		parm0 = parm1;
	}

	ci1 = NULL;
	parm1 = NULL;

	c1 = c2d_trim ( c, parm0, parm1 );

	if ( c1 != NULL )
	{
		tol = bbs_get_tol() ;
		length = c2c_length(c1) ;
		if ( length > tol )
		//if ( c2c_length ( c1 ) > bbs_get_tol() )
		{
			bseg = t2b_bseg_create ( e, c1 );
			dml_append_data ( clist, ( ANY ) bseg );
		}
		else
			c2d_free_curve ( c1 );
	} 
}


/*-------------------------------------------------------------------------*/
static INT edge_count ( T2_REGION r )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	DML_ITEM item;
	T2_LOOP l;
	INT count = 0;

	DML_WALK_LIST ( T2_REGION_LOOP_LIST(r), item )
	{
		l = dml_record ( item );
		count += dml_length ( T2_LOOP_EDGE_LIST(l) );
	}

	return ( count );
}

/*-------------------------------------------------------------------------*/
static BOOLEAN edge_count_key ( T2_REGION r1, T2_REGION r2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	if ( edge_count ( r1 ) < edge_count ( r2 ) )
		return ( TRUE );
	else
		return ( FALSE );
}

/*-------------------------------------------------------------------------*/
DML_LIST t2b_union_l ( DML_LIST rlist, DML_LIST reslist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
		DML_ITEM item0, item1, item2; 
        DML_LIST ulist;
        T2_REGION r0, r1;

        ulist = dml_create_list();

		if ( reslist == NULL )
	        reslist = dml_create_list();

		while ( dml_length ( rlist ) > 1 )
        {
				dml_sort_list ( rlist, ( PF_SORT ) edge_count_key );

				item0 = dml_first ( rlist );
                r0 = ( T2_REGION ) dml_record ( item0 );

				for ( item1 = dml_next ( item0 ); item1 != NULL; 
					item1 = item2 )
				{
					item2 = dml_next ( item1 );
					r1 = ( T2_REGION ) dml_record ( item1 );

					if ( c2a_box_overlap ( T2_REGION_BOX(r0), 
						T2_REGION_BOX(r1) ) == FALSE )
					{
						continue;
					}

					t2b_union_rr ( r0, r1, ulist );

					if ( dml_length ( ulist ) > 0 )
					{
						if ( dml_length ( ulist ) == 2 )
						{
							dml_apply ( ulist, ( PF_ACTION ) t2d_free_region );
							dml_clear_list ( ulist );
						}
						else
						{
							dml_remove_item ( rlist, item0 );
							dml_remove_item ( rlist, item1 );

							t2d_free_region ( r0 );
							t2d_free_region ( r1 );

							item0 = dml_first ( ulist );
							r0 = dml_record ( item0 );

							dml_insert_after ( rlist, NULL, r0 );
							dml_clear_list ( ulist );

							break;
						}
					}
				}

				if ( item1 == NULL )
				{
					dml_append_data ( reslist, ( ANY ) r0 );
					dml_remove_item ( rlist, item0 );
				}
        }

		dml_free_list ( ulist );

		dml_append_list ( reslist, rlist );

        return ( reslist );
}

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
DML_LIST t2b_diff_rl ( T2_REGION r0, DML_LIST rlist, DML_LIST reslist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1996 Building Block Software, Inc.  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
	DML_ITEM item; 
    T2_REGION r;

	if ( reslist == NULL )
	    reslist = dml_create_list();

	if ( dml_length ( rlist ) > 1 )
    {
		dml_sort_list ( rlist, ( PF_SORT ) edge_count_key );

		DML_WALK_LIST ( rlist, item )
		{
			r = ( T2_REGION ) dml_record ( item );

			if ( c2a_box_overlap ( T2_REGION_BOX(r0), 
				T2_REGION_BOX(r) ) == FALSE )
			{
				continue;
			}

			t2b_diff_rr ( r0, r, reslist );
		}
    }

    return ( reslist );
}

#endif // NEW_CODE

#endif /* __BBS_MILL__>=2 || __BBS_TURN__>=2 */

