/******************************** T2IPRIV.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2IPRIV
#define T2IPRIV

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>
#include <t2idefs.h>
#include <t2link.h>

extern T2_INT_REC t2i_flip_int_rec __(( T2_INT_REC )) ;
extern T2_LINK t2i_create_link __(( T2_EDGE, PARM, INT, PT2, 
            T2_EDGE, PARM, INT, PT2, BOOLEAN, BOOLEAN )) ;
extern void t2i_delete_link __(( T2_LINK )) ;
extern void t2i_free_link __(( T2_LINK )) ;
extern void t2i_clear_links_region __(( T2_REGION )) ;
extern void t2i_clear_links_loop __(( T2_LOOP )) ;
extern T2_LINK t2i_next_link __(( T2_LINK )) ;
extern T2_LINK t2i_prev_link __(( T2_LINK )) ;
extern INT t2i_reg_hor_ray_int_no __(( T2_REGION, PT2, BOOLEAN )) ;
extern INT t2i_reg_ray_int_no __(( T2_REGION, PT2, REAL, REAL, BOOLEAN )) ;
extern INT t2i_loop_hor_ray_int_no __(( T2_LOOP, PT2, BOOLEAN )) ;
extern INT t2i_loop_ray_int_no __(( T2_LOOP, PT2, REAL, REAL, BOOLEAN )) ;
extern INT t2i_edge_hor_ray_int_no __(( T2_EDGE, PT2, BOOLEAN )) ;
extern INT t2i_edge_ray_int_no __(( T2_EDGE, PT2, REAL, REAL, BOOLEAN )) ;
extern BOOLEAN t2i_edgelist_ray __(( DML_LIST, PT2, REAL, PT2, T2_PARM )) ;
extern INT t2i_intersect_ray_edgelist __(( C2_CURVE, DML_LIST, BOOLEAN, 
            BOOLEAN, BOOLEAN, DML_LIST )) ;
#endif

