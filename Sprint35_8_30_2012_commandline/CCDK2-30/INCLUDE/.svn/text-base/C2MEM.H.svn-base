/******************************* C2MEM.H ***********************************/
/************ Include file for 2d alloc definitions and macros *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2MEM
#define     C2MEM

#ifdef      SPLINE 
#define     C2_ALLOC_CURV_EXTR   (MALLOC(1,C2_CURV_EXTR_S))
#define     C2_FREE_CURV_EXTR(R) FREE(R)
#define     C2_ALLOC_NURB    ((C2_NURB) MALLOC (1, C2_NURB_S ) )
#define     C2_FREE_NURB(N)  FREE ( (N) )
#endif  /* SPLINE */

#define     C2_ALLOC_LINE    ((C2_LINE) MALLOC (1, C2_LINE_S ) )
#define     C2_FREE_LINE(L)  FREE((L))

#define     C2_ALLOC_ARC    ((C2_ARC) MALLOC (1, C2_ARC_S ) )
#define     C2_FREE_ARC(A)  FREE((A))

#define     C2_ALLOC_CURVE  ((C2_CURVE) MALLOC (1, C2_CURVE_S ) )
#define     C2_FREE_CURVE(C) FREE ( (C) )

#define     C2_ALLOC_INT_REC    ((C2_INT_REC) MALLOC(1, C2_INT_REC_S ))
#define     C2_FREE_INT_REC(L)  FREE((L))

#endif /* C2MEM */

