/*************************** FNDEFS.H **************************************/
/********* definitions and macros for fn routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     FNDEFS
#define     FNDEFS
#include <bbsdefs.h>

typedef struct extr_record {
    PARM_S  parm ;      /* parameter */
    REAL    f ;         /* function */
    INT     type ;      /* +1, if max, -1, if min, 0 otherwise(+-2if inf) */
    }       FN_EXTR_REC_S ;

typedef     FN_EXTR_REC_S *FN_EXTR_REC ;

/*********** Access macros ********/

#define FN_EXTR_REC_PARM(R) (&((R)->parm))
#define FN_EXTR_REC_T(R)    (PARM_T(FN_EXTR_REC_PARM(R)))
#define FN_EXTR_REC_J(R)    (PARM_J(FN_EXTR_REC_PARM(R)))
#define FN_EXTR_REC_F(R)    ((R)->f)
#define FN_EXTR_REC_TYPE(R) ((R)->type)
#define FN_ALLOC_EXTR_REC   (MALLOC(1,FN_EXTR_REC_S))
#define FN_FREE_EXTR_REC(R) FREE(R)

/* Homogeneous numbers manipulation macros */

#define     FNH_IS_ZERO(A)  (IS_ZERO((A)[0])&&(!IS_ZERO((A)[1])))
#define     FNH_SET_ZERO(A) {(A)[0]=0.0;(A)[1]=1.0;}
#define     FNH_COPY(A,B)   {(A)[0]=(B)[0];(A)[1]=(B)[1];}
#define     FNH_ADD(A,B,C)  {(C)[0]=(A)[0]+(B)[0];(C)[1]=(A)[1]+(B)[1];}
#define     FNH_ADDT(A,B,T,C)  \
            {(C)[0]=(A)[0]+(T)*(B)[0];(C)[1]=(A)[1]+(T)*(B)[1];}
#define     FNH_ADDW(A,P,B,Q,C) \
            {(C)[0]=(P)*(A)[0]+(Q)*(B)[0];(C)[1]=(P)*(A)[1]+(Q)*(B)[1];}
#define     FNH_ADDU(A,B,C) {(C)[0]=(1.0-(U))*(A)[0]+(U)*(B)[0];\
            (C)[1]=(1.0-(U))*(A)[1]+(U)*(B)[1];}
#define     FNH_SUB(A,B,C)  {(C)[0]=(A)[0]-(B)[0];(C)[1]=(A)[1]-(B)[1];}
#define     FNH_MID(A,B,C) {(C)[0]=0.5*((A)[0]+(B)[0]);\
            (C)[1]=0.5*((A)[1]+(B)[1]);}
#define     FNH_MULT(A,T,B) {(B)[0]=(T)*(A)[0];(B)[1]=(T)*(A)[1];}

#endif /* FNDEFS*/

