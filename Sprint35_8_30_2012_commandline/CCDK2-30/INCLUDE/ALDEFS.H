/******************************** ALDEFS.H *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     ALDEFS
#define     ALDEFS
#include <bbsdefs.h>

#define     AL_SWAP(A,B)    {REAL C;C=(A);(A)=(B);(B)=C;}

typedef struct root_record {
    REAL    t ;         /* Root or start parameter of an interval */
    REAL    t1 ;        /* End parameter of an interval */
    INT     mult ;      /* multiplicity of a root */
                        /* if mult=-1, then the whole interval is a root */
    }       AL_ROOT_REC_S ;
typedef  AL_ROOT_REC_S *AL_ROOT_REC ;
/*********** Access macros ********/

#define AL_ROOT_REC_T(R)        ((R)->t)
#define AL_ROOT_REC_T1(R)       ((R)->t1)
#define AL_ROOT_REC_MULT(R)     ((R)->mult)

#endif /* ALDEFS*/

