/******************************* T2BDEFS.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2BDEFS
#define T2BDEFS

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>

typedef struct t2_bseg_s
{
	T2_EDGE edge;
	C2_CURVE curve;
	INT state; // 'i', 'o', 'w', 'a', 'u'


} T2_BSEG_S, *T2_BSEG;

#define T2_BSEG_EDGE(r)				((r)->edge)
#define T2_BSEG_CURVE(r)			((r)->curve)
#define T2_BSEG_STATE(r)			((r)->state)

#define T2_BSEG_SET_STATE_I(r)		{T2_BSEG_STATE(r)='i';}
#define T2_BSEG_SET_STATE_O(r)		{T2_BSEG_STATE(r)='o';}
#define T2_BSEG_SET_STATE_W(r)		{T2_BSEG_STATE(r)='w';}
#define T2_BSEG_SET_STATE_A(r)		{T2_BSEG_STATE(r)='a';}
#define T2_BSEG_SET_STATE_U(r)		{T2_BSEG_STATE(r)='u';}

#define T2_BSEG_IS_I(r)				(T2_BSEG_STATE(r)=='i')
#define T2_BSEG_IS_O(r)				(T2_BSEG_STATE(r)=='o')
#define T2_BSEG_IS_W(r)				(T2_BSEG_STATE(r)=='w')
#define T2_BSEG_IS_A(r)				(T2_BSEG_STATE(r)=='a')
#define T2_BSEG_IS_U(r)				(T2_BSEG_STATE(r)=='u')

typedef struct t2_breg_s
{
	T2_REGION region;
	DML_LIST clist;
	DML_LIST ilist;
	DML_LIST olist;
	DML_LIST wlist;
	DML_LIST alist;

} T2_BREG_S, *T2_BREG;

#define T2_BREG_REGION(r)			((r)->region)
#define T2_BREG_ILIST(r)			((r)->ilist)
#define T2_BREG_OLIST(r)			((r)->olist)
#define T2_BREG_WLIST(r)			((r)->wlist)
#define T2_BREG_ALIST(r)			((r)->alist)
#define T2_BREG_CLIST(r)			((r)->clist)

T2_BSEG t2b_bseg_create __(( T2_EDGE edge, C2_CURVE curveseg ));
void t2b_bseg_free __(( T2_BSEG bseg ));

T2_BREG t2b_breg_create __(( T2_REGION region ));
void t2b_breg_free __(( T2_BREG breg ));

void t2b_boolean_pass1 ( T2_BREG breg1, T2_BREG breg2 );
void t2b_boolean_pass1_edge ( T2_EDGE e, T2_REGION r1, DML_LIST clist );
void t2b_boolean_pass1_curve ( T2_EDGE e, C2_CURVE c, DML_LIST intlist1, DML_LIST clist );
BOOLEAN t2_boolean_pass2_sortkey ( C2_INT_REC ci1, C2_INT_REC ci2 );
void t2b_boolean_pass2 ( T2_BREG breg1, T2_BREG breg2 );
DML_ITEM t2b_boolean_pass2_x ( T2_BSEG bseg, DML_LIST clist, INT *w );


extern  DML_LIST t2b_offset_region __(( T2_REGION DUMMY0 , REAL DUMMY1 , 
            T2_OFFSET_DESC DUMMY2 , DML_LIST DUMMY3 )) ;
extern  DML_LIST t2b_union_rr __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 , 
            DML_LIST DUMMY2 )) ; 
extern  DML_LIST t2b_union_rl __(( T2_REGION DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_union_l __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
extern  DML_LIST t2b_common_rr __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 , 
            DML_LIST DUMMY2 )) ; 
extern  DML_LIST t2b_common_rl __(( T2_REGION DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_common_l __(( DML_LIST DUMMY0 , DML_LIST DUMMY1 )) ;
extern  DML_LIST t2b_diff_rr __(( T2_REGION DUMMY0 , T2_REGION DUMMY1 , 
            DML_LIST DUMMY2 )) ; 
extern  DML_LIST t2b_diff_rl __(( T2_REGION DUMMY0 , DML_LIST DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_offset_loop __(( T2_LOOP, REAL, 
            T2_OFFSET_DESC, DML_LIST )) ;
extern  INT t2b_offset_loop_array __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , T2_OFFSET_DESC DUMMY3 , T2_LOOP* DUMMY4 )) ;
extern  INT t2b_offset_loop_lists __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , T2_OFFSET_DESC DUMMY3 , DML_LIST* DUMMY4 )) ;
extern  DML_LIST t2b_fillet_loop __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  DML_LIST t2b_fillet_region __(( T2_REGION DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  T2_OD   t2b_od_lookup __(( T2_OFFSET_DESC DUMMY0 , ANY DUMMY1 , 
            REAL DUMMY2 , REAL* DUMMY3 )) ;
extern  void t2b_od_select_loop __(( T2_LOOP DUMMY0 , T2_OFFSET_DESC DUMMY1 , 
            REAL DUMMY2 )) ;
extern  void t2b_od_select_loop_list __(( T2_LOOP DUMMY0 , DML_LIST DUMMY1 , 
            REAL DUMMY2 )) ;
extern  DML_ITEM t2b_append_od __(( ANY DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 , 
            T2_OFFSET_DESC DUMMY3 )) ;
extern  DML_ITEM t2b_append_od_list __(( ANY DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 , DML_LIST DUMMY3 )) ;
extern  T2_OD t2b_create_od __(( ANY DUMMY0 , PT2 DUMMY1 , REAL DUMMY2 )) ;
extern  void t2b_free_od __(( T2_OD DUMMY0 )) ;
extern  T2_OFFSET_DESC t2b_create_offset_desc __(( void )) ;
extern  T2_OFFSET_DESC t2b_offset_set_mode __(( T2_OFFSET_DESC DUMMY0 , 
            T2_OFFSET_MODE DUMMY1 )) ;
extern  void        t2b_free_offset_desc __(( T2_OFFSET_DESC DUMMY0 )) ;
extern  void t2b_xform_od_list __(( DML_LIST DUMMY0 , C2_XFORM DUMMY1 )) ;
extern  T2_LOOP t2b_inflate_edge __(( T2_EDGE DUMMY0 , REAL DUMMY1 )) ;
extern  T2_LOOP t2b_inflate_line __(( PT2 DUMMY0 , PT2 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  T2_LOOP t2b_taper_loop __(( T2_LOOP DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 )) ;
#endif

