/******************************* M2DEFS.H **********************************/
/*************** Include file for 2d definitions and macros ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     M2DEFS
#define     M2DEFS

#define     M2_PT_ON_CURVE      -1
#define     M2_END_PT_ON_RAY    -2

typedef struct m2_xh_rec {
    PT2     pt ;
    PARM_S  parm ;
    ANY     owner ;
    INT     status ;
    } M2_XH_REC_S ;
typedef     M2_XH_REC_S    *M2_XH_REC ;

#define     M2_XH_REC_PT(R)         ((R)->pt)
#define     M2_XH_REC_X(R)          (M2_XH_REC_PT(R)[0])
#define     M2_XH_REC_Y(R)          (M2_XH_REC_PT(R)[1])
#define     M2_XH_REC_PARM(R)       (&((R)->parm))
#define     M2_XH_REC_T(R)          (PARM_T(M2_XH_REC_PARM(R)))
#define     M2_XH_REC_J(R)          (PARM_J(M2_XH_REC_PARM(R)))
#define     M2_XH_REC_OWNER(R)      ((R)->owner)
#define     M2_XH_REC_STATUS(R)     ((R)->status)

#define     M2_ALLOC_XH_REC         ((M2_XH_REC)MALLOC(1,M2_XH_REC_S))
#define     M2_FREE_XH_REC(R)       FREE((R))

#endif /* M2DEFS*/

