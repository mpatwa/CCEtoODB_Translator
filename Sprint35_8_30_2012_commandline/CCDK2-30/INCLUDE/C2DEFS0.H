/******************************* C2DEFS0.H *********************************/
/*************** Include file for 2d definitions and macros ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include    <bbsdefs.h>
#include    <dmldefs.h>
#include    <fndefs.h>

#define     C2_DISPLAY_BUF_SIZE     50
#include    <c2pt2.h>
#include    <c2hpt2.h>
typedef     void            (*PF_PROP2) __(( PT2*, REAL* )) ; 
                /* pointer to a mass property function */
typedef     void        (*PF_EVAL2) __(( ANY, PT2 )) ; 
                /* pointer to an evaluation function */
#define C2_CURVE_TYPE_ENUM  { C2_LINE_TYPE, C2_ARC_TYPE, C2_PCURVE_TYPE, \
    C2_ELLIPSE_TYPE, C2_NURB_TYPE, C2_BEZIER_TYPE, C2_ZERO_ARC_TYPE, \
    C2_UNKNOWN_TYPE  } 
#ifdef __cplusplus
enum C2_CURVE_TYPE_ENUM ;
typedef char C2_CURVE_TYPE_DEF ;
#else
typedef     enum C2_CURVE_TYPE_ENUM  C2_CURVE_TYPE_DEF ;
#endif

#ifdef SPLINE
#define C2_TAN_OPTIONS_ENUM { C2_NDEF_TAN, C2_DEF_TAN_MAGN, C2_DEF_TAN } 
#define C2_KNOT_OPTIONS_ENUM { C2_DEFAULT, C2_UNIFORM, C2_CLSC_UNI, \
            C2_NONUNI, C2_GIVEN } 

#ifdef __cplusplus
enum C2_TAN_OPTIONS_ENUM ;
typedef char C2_TAN_OPTIONS ;
enum C2_KNOT_OPTIONS_ENUM ;
typedef char C2_KNOT_OPTIONS ;
#else
typedef enum C2_TAN_OPTIONS_ENUM C2_TAN_OPTIONS ;
typedef enum C2_KNOT_OPTIONS_ENUM C2_KNOT_OPTIONS ;

#endif 
#endif /* SPLINE */

typedef     REAL            C2_XFORM[2][3] ;
#define     C2_TRANSFORM    C2_XFORM

#define     C2_EXTR_REC_S   FN_EXTR_REC_S
#define     C2_EXTR_REC     FN_EXTR_REC
#define C2_EXTR_REC_PARM    FN_EXTR_REC_PARM
#define C2_EXTR_REC_T       FN_EXTR_REC_T
#define C2_EXTR_REC_J       FN_EXTR_REC_J
#define C2_EXTR_REC_F       FN_EXTR_REC_F
#define C2_EXTR_REC_TYPE    FN_EXTR_REC_TYPE

typedef     struct          c2_bndbox {
            PT2             minpt ;
            PT2             maxpt ;
            } C2_BOX_S ;
typedef     C2_BOX_S    *C2_BOX ;

#include <c2defsd.h>

typedef     struct          c2_curve {
            C2_CURVE_TYPE_DEF   curve_type ;
            union {
#ifdef  SPLINE
                C2_NURB         nurb ;
#endif  /* SPLINE */
                C2_ARC          arc ;
                C2_ZERO_ARC     zero_arc ;
                C2_LINE         line ;
                C2_PCURVE       pcurve ;
            } curve_geom ;
            C2_BOX_S        box ;
            PARM_S          parm0 ;
            PARM_S          parm1 ;
            } C2_CURVE_S ;
typedef     C2_CURVE_S    *C2_CURVE ;

EXTERN  INT                 C2_CURVE_BUF_SIZE ;
EXTERN  INT                 C2_CURVES_NUMBER ;
EXTERN  C2_CURVE            *C2_CURVE_BUFFER ;

typedef     struct  c2_curve_int_rec {
            PT2             int_pt ;
            REAL            dist ;
            PARM_S          parm[2] ;
            INT             type ;
            BOOLEAN         near_tan ;
            } C2_INT_REC_S ;
typedef     C2_INT_REC_S    *C2_INT_REC ;

#define     C2_INT_REC_PARM1(R) (&((R)->parm[0]))
#define     C2_INT_REC_T1(R)    (PARM_T(C2_INT_REC_PARM1(R))) 
#define     C2_INT_REC_J1(R)    (PARM_J(C2_INT_REC_PARM1(R))) 
#define     C2_INT_REC_PARM2(R) (&((R)->parm[1]))
#define     C2_INT_REC_T2(R)    (PARM_T(C2_INT_REC_PARM2(R))) 
#define     C2_INT_REC_J2(R)    (PARM_J(C2_INT_REC_PARM2(R))) 
#define     C2_INT_REC_PARM(R,I) (&((R)->parm[I-1]))
#define     C2_INT_REC_T(R,I)   (PARM_T(C2_INT_REC_PARM(R,I))) 
#define     C2_INT_REC_J(R,I)   (PARM_J(C2_INT_REC_PARM(R,I))) 
#define     C2_INT_REC_PT(R)    ((R)->int_pt)
#define     C2_INT_REC_DIST(R)  ((R)->dist)
#define     C2_INT_REC_TYPE(R)  ((R)->type)
#define     C2_INT_REC_TRANS(R) ((C2_INT_REC_TYPE(R))%2)
#define     C2_INT_REC_TANGENT(R)   (!((C2_INT_REC_TYPE(R))%2))
#define     C2_INT_REC_NEAR_TAN(R)   ((R)->near_tan)

#include <c2defsm.h>

#define     C2_MIN_PT(B)    ((B)->minpt)
#define     C2_MAX_PT(B)    ((B)->maxpt)
#define     C2_MIN_X(B)     ((C2_MIN_PT(B))[0])
#define     C2_MIN_Y(B)     ((C2_MIN_PT(B))[1])
#define     C2_MAX_X(B)     ((C2_MAX_PT(B))[0])
#define     C2_MAX_Y(B)     ((C2_MAX_PT(B))[1])

#define     C2_CURVE_TYPE(C)    ((C)->curve_type)
#define     C2_CURVE_IS_LINE(C) (C2_CURVE_TYPE(C)==C2_LINE_TYPE)
#define     C2_CURVE_IS_ARC(C)  (C2_CURVE_TYPE(C)==C2_ARC_TYPE)
#define     C2_CURVE_IS_ZERO_ARC(C)  (C2_CURVE_TYPE(C)==C2_ZERO_ARC_TYPE)
#define     C2_CURVE_IS_EXT_ARC(C)  \
        (C2_CURVE_IS_ARC(C)&&C2_CURVE_T1(C)>=1.0+BBS_ZERO)
#define     C2_CURVE_IS_PCURVE(C)  (C2_CURVE_TYPE(C)==C2_PCURVE_TYPE)
#ifdef  SPLINE
#define     C2_CURVE_IS_ELLIPSE(C)  (C2_CURVE_TYPE(C)==C2_ELLIPSE_TYPE)
#define     C2_CURVE_IS_SPLINE(C)  (C2_CURVE_TYPE(C)==C2_NURB_TYPE)
#define     C2_CURVE_IS_BEZIER(C)  (C2_CURVE_TYPE(C)==C2_BEZIER_TYPE)
#endif  /* SPLINE */

#define     C2_CURVE_PARM0(C)   (&((C)->parm0))
#define     C2_CURVE_PARM1(C)   (&((C)->parm1))
#define     C2_CURVE_T0(C)      (PARM_T(C2_CURVE_PARM0(C)))
#define     C2_CURVE_J0(C)      (PARM_J(C2_CURVE_PARM0(C)))
#define     C2_CURVE_T1(C)      (PARM_T(C2_CURVE_PARM1(C)))
#define     C2_CURVE_J1(C)      (PARM_J(C2_CURVE_PARM1(C)))

#define     C2_CURVE_BOX(C)     (&((C)->box))
#define     C2_CURVE_X_MIN(C)   (C2_MIN_X(C2_CURVE_BOX(C)))
#define     C2_CURVE_X_MAX(C)   (C2_MAX_X(C2_CURVE_BOX(C)))
#define     C2_CURVE_Y_MIN(C)   (C2_MIN_Y(C2_CURVE_BOX(C)))
#define     C2_CURVE_Y_MAX(C)   (C2_MAX_Y(C2_CURVE_BOX(C)))

