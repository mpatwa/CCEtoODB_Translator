/******************************** T2IDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2IDEFS
#define T2IDEFS

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>

typedef struct t2_int_rec {
    T2_EDGE edge1 ;
    T2_EDGE edge2 ;
    C2_INT_REC int_rec ;
    } *T2_INT_REC, T2_INT_REC_S ;

#define     T2_INT_REC_EDGE1(R)         ((R)->edge1)
#define     T2_INT_REC_EDGE2(R)         ((R)->edge2)
#define     T2_INT_REC_CI(R)            ((R)->int_rec)
#define     T2_INT_REC_PARM1(R)         (C2_INT_REC_PARM1((R)->int_rec))
#define     T2_INT_REC_PARM2(R)         (C2_INT_REC_PARM2((R)->int_rec))
#define     T2_INT_REC_T1(R)            (C2_INT_REC_T1((R)->int_rec))
#define     T2_INT_REC_T2(R)            (C2_INT_REC_T2((R)->int_rec))
#define     T2_INT_REC_J1(R)            (C2_INT_REC_J1((R)->int_rec))
#define     T2_INT_REC_J2(R)            (C2_INT_REC_J2((R)->int_rec))
#define     T2_INT_REC_PT(R)            (C2_INT_REC_PT((R)->int_rec))
#define     T2_INT_REC_TRANS(R)         (C2_INT_REC_TRANS((R)->int_rec))
#define     T2_INT_REC_TANGENT(R)       (C2_INT_REC_TANGENT((R)->int_rec))
#define     T2_INT_REC_TYPE(R)          (C2_INT_REC_TYPE((R)->int_rec))
#define     T2_ALLOC_INT_REC            ((T2_INT_REC)MALLOC(1,T2_INT_REC_S))
#define     T2_FREE_INT_REC(I)          FREE((I))

extern INT t2i_intersect_edge __(( T2_EDGE, T2_EDGE, BOOLEAN, BOOLEAN, 
            DML_LIST )) ;
extern INT t2i_intersect_loop __(( T2_LOOP, T2_LOOP, DML_LIST )) ;
extern INT t2i_intersect_loop_self __(( T2_LOOP, DML_LIST )) ;
extern void t2i_free_int_rec __(( T2_INT_REC )) ;
extern INT t2i_intersect_ray_region __(( C2_CURVE, T2_REGION, BOOLEAN, 
            BOOLEAN, DML_LIST )) ;
extern INT t2i_intersect_ray_loop __(( C2_CURVE, T2_LOOP, BOOLEAN, BOOLEAN, 
            BOOLEAN, DML_LIST )) ;
extern INT t2i_intersect_ray_edge __(( C2_CURVE, T2_EDGE, BOOLEAN, BOOLEAN, 
            BOOLEAN, DML_LIST )) ;
extern BOOLEAN t2i_region_ray __(( T2_REGION, PT2, REAL, PT2, T2_PARM )) ;
extern BOOLEAN t2i_loop_ray __(( T2_LOOP, PT2, REAL, PT2, T2_PARM )) ;
extern INT     t2i_region_curve __(( T2_REGION, C2_CURVE, BOOLEAN, 
            DML_LIST )) ;
extern INT     t2i_loop_edge __(( T2_LOOP, T2_EDGE, DML_LIST )) ;
extern INT     t2i_loop_curve __(( T2_LOOP, C2_CURVE, BOOLEAN, DML_LIST )) ;
extern INT     t2i_edge_curve __(( T2_EDGE, C2_CURVE, BOOLEAN, DML_LIST )) ;
extern BOOLEAN t2i_region_infline __(( T2_REGION, PT2, REAL, 
            PT2, T2_PARM, PT2, T2_PARM )) ; 
extern BOOLEAN t2i_loop_infline __(( T2_LOOP, PT2, REAL, 
            PT2, T2_PARM, PT2, T2_PARM )) ; 
extern BOOLEAN t2i_loop_infline __(( T2_LOOP, PT2, REAL, 
            PT2, T2_PARM, PT2, T2_PARM )) ; 
BBS_PUBLIC BOOLEAN t2i_loop_circle __(( T2_LOOP, PT2, REAL, 
            PT2, T2_PARM, PT2, T2_PARM )) ; 
extern T2_INT_REC t2i_create_int_rec __(( T2_EDGE, T2_EDGE, C2_INT_REC )) ;
extern BOOLEAN t2i_trim_2loops __(( T2_LOOP, T2_LOOP, BOOLEAN )) ;
#endif

