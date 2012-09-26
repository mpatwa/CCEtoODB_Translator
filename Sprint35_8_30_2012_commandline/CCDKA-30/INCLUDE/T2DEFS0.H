/******************************** T2DEFS0.H ********************************/
/******************* definitions and macros for 2d topology ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmdefs.h>
#include <c2defs.h>

EXTERN      REAL        T2_GAP_COEFF ;           

/*************** Topological object declarations **************/

#define T2_PT_POSITION_ENUM { T2_PT_INSIDE, T2_PT_OUTSIDE, \
        T2_PT_ON_BOUNDARY, T2_PT_UNKNOWN } 
#define T2_VTX_STATUS_ENUM  { T2_VTX_UNDEF, T2_VTX_CONVEX, \
            T2_VTX_TANGENT, T2_VTX_CONCAVE, T2_VTX_SHARP } 
#define T2_LEAD_MODE_ENUM { T2_LEAD_NONE, T2_LEAD_ARC, T2_LEAD_NORMAL, \
            T2_LEAD_TANGENT, T2_LEAD_EXT_ARC, T2_LEAD_EXT_NORMAL } 
#define T2_SIDE_ENUM { T2_SIDE_UNDEF, T2_SIDE_RIGHT, T2_SIDE_LEFT, \
            T2_SIDE_BIAS_PT }
#define T2_DIR_ENUM { T2_DIR_CCW, T2_DIR_CW, T2_DIR_UNDEF } 
#define T2_OFFSET_MODE_ENUM { T2_FILLET, T2_SHARP, T2_CHAMFER } 
#ifdef __cplusplus
enum T2_PT_POSITION_ENUM ;
typedef char T2_PT_POSITION ;
enum T2_VTX_STATUS_ENUM ;
typedef char T2_VTX_STATUS ;
enum T2_LEAD_MODE_ENUM ;
typedef char T2_LEAD_MODE ;
enum T2_SIDE_ENUM ;
typedef char T2_SIDE ;
enum T2_DIR_ENUM ;
typedef char T2_DIR ;
enum T2_OFFSET_MODE_ENUM ;
typedef char T2_OFFSET_MODE ;
#else
typedef enum T2_PT_POSITION_ENUM T2_PT_POSITION ;
typedef enum T2_VTX_STATUS_ENUM T2_VTX_STATUS ;
typedef enum T2_LEAD_MODE_ENUM T2_LEAD_MODE ;
typedef enum T2_SIDE_ENUM T2_SIDE ;
typedef enum T2_DIR_ENUM T2_DIR ;
typedef enum T2_OFFSET_MODE_ENUM T2_OFFSET_MODE ;
#endif

typedef struct t2_parm {
    struct t2_edge *edge ;
    PARM_S cparm ;
    } T2_PARM_S ;
typedef     T2_PARM_S    *T2_PARM ;

typedef struct t2_region {
    DML_LIST    loop_list;
    struct t2_region *parent ;
    ATTR        attr ;
    } *T2_REGION, T2_REGION_S;

typedef struct spiral {
    DML_LIST        loop_list ;
    DML_LIST        spiral_list ;
    DML_LIST        open_list ;
    struct spiral  *spiral_parent ;
    ANY             loop_init ;
    PT2             pt ;
    T2_PARM_S       parm0 ;
    T2_PARM_S       parm1 ;
    } T2_SPIRAL_S ;
typedef     T2_SPIRAL_S    *T2_SPIRAL ;

typedef struct t2_loop {
    union { 
        T2_REGION   region ;
        T2_SPIRAL   spiral ;
    } owner ;
    DML_LIST    edge_list;
    DML_LIST    link_list ;
    struct t2_loop *parent ;
    C2_BOX_S    loop_box ;
    BOOLEAN     closed;
    ATTR        attr ;
    } *T2_LOOP, T2_LOOP_S;

typedef struct t2_edge {   
    T2_LOOP     loop;   
    C2_CURVE    curve;
    struct t2_edge *parent ;
    struct t2_edge *ref ;
    INT         dir ;
    ATTR        attr ;
    T2_VTX_STATUS vtx;
    PT2         sc_pt ;
    } *T2_EDGE, T2_EDGE_S;

typedef struct t2_xhatch {
    PT2*    xhatch_pt ;
    PARM    xhatch_parm ;
    ANY*    xhatch_owner ;
    INT     xlines_no ;
    } T2_XHATCH_S ;
typedef     T2_XHATCH_S    *T2_XHATCH ;

#ifndef MALLOC_CHECK
EXTERN  INT                 T2_EDGE_BUF_SIZE ;
EXTERN  INT                 T2_EDGES_NUMBER ;
EXTERN  T2_EDGE             *T2_EDGE_BUFFER ;
#else
EXTERN  INT                 T2_EDGES_ALLOC ;
#endif

typedef struct t2_extr_rec_s {
    T2_EDGE     edge ;
    REAL        t ;
    INT         position ;
    INT         type ;
    } T2_EXTR_REC_S, *T2_EXTR_REC ;

/*### jak */

typedef struct t2_loc_extr_rec_s {
    INT         type ; /* point or interval */
    INT         range ; /* min, max or interval */
    T2_PARM_S   tparm0;
    T2_PARM_S   tparm1;
    REAL        value ;
    } T2_LOC_EXTR_REC_S, *T2_LOC_EXTR_REC ;

/*### jak */


typedef struct t2_fragment {   
    T2_PARM_S   parm0 ;
    T2_PARM_S   parm1 ;
    PT2         pt0 ;
    PT2         pt1 ;
}   T2_FRAGMENT_S ;
typedef     T2_FRAGMENT_S    *T2_FRAGMENT ;

typedef     DML_ITEM    T2_REGION_WALK[2] ;

#ifdef __MCRSFTC__
typedef BOOLEAN (*PF_T2C_EVAL) __(( ANY, PT2 )) ; 
#else
#ifdef  __HIGHC__
typedef BOOLEAN (*PF_T2C_EVAL) __(( ANY, PT2 )) ; 
#else
typedef     PF_BOOLEAN  PF_T2C_EVAL ; 
#endif 
#endif  

typedef struct t2_od {
    ANY owner ; 
    PT2 pt ; 
    REAL dist ;
} T2_OD_S, *T2_OD ;

#include <t2edgem.h>
#include <t2loopm.h>
#include <t2regnm.h>
#include <t2parmm.h>
#include <t2xhm.h>

typedef struct offset_struct {
    DML_LIST        distlist ;
    T2_OFFSET_MODE  offset_mode ;
}   T2_OFFSET_DESC_S, *T2_OFFSET_DESC ;
#define     T2_OFFSET_DESC_DLIST(D) ((D)->distlist)
#define     T2_OFFSET_DESC_MODE(D)  ((D)->offset_mode)

#ifdef __BBS_PUNCH__ 
#if ( __BBS_PUNCH__ >= 1 ) 
#include <t2punch.h>
#endif
#endif

