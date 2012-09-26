/******************************* C2VMCRS.H *********************************/
/*******************Euclidean vector manipulation macros *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C2VMCRS
#define     C2VMCRS
#ifdef      BBSMACROS

#define     C2V_IS_ZERO(A)  (IS_ZERO((A)[0])&&IS_ZERO((A)[1]))
#define     C2V_IS_SMALL(A)  (IS_SMALL((A)[0])&&IS_SMALL((A)[1]))
#define     C2V_IDENT_PTS(A,B)  \
(IS_SMALL((A)[0]-(B)[0])&&IS_SMALL((A)[1]-(B)[1]))
#define     C2V_SET_ZERO(A) {(A)[0]=0.0;(A)[1]=0.0;}
#define     C2V_SET(X,Y,A) {(A)[0]=(X);(A)[1]=(Y);}
#define     C2V_COPY(A,B)   {(B)[0]=(A)[0];(B)[1]=(A)[1];}
#define     C2V_ADD(A,B,C)  {(C)[0]=(A)[0]+(B)[0];(C)[1]=(A)[1]+(B)[1];}
#define     C2V_ADDT(A,B,T,C)  \
            {(C)[0]=(A)[0]+(T)*(B)[0];(C)[1]=(A)[1]+(T)*(B)[1];}
#define     C2V_ADDW(A,P,B,Q,C) \
            {(C)[0]=(P)*(A)[0]+(Q)*(B)[0];(C)[1]=(P)*(A)[1]+(Q)*(B)[1];}
#define     C2V_ADDC(D,A,P,B,Q,C) \
            {(C)[0]=(D)[0]+(P)*(A)[0]+(Q)*(B)[0];\
            (C)[1]=(D)[1]+(P)*(A)[1]+(Q)*(B)[1];}
#define     C2V_ADDU(A,B,U,C) {(C)[0]=(1.0-(U))*(A)[0]+(U)*(B)[0];\
            (C)[1]=(1.0-(U))*(A)[1]+(U)*(B)[1];}
#define     C2V_OFFSET(A,T,W,B) {(B)[0]=(A)[0]+(W)*(T)[1];\
                               (B)[1]=(A)[1]-(W)*(T)[0];}
#define     C2V_NORMAL(A,B) {(B)[0]=(A)[1];(B)[1]=(-(A)[0]);}
#define     C2V_SUB(A,B,C)  {(C)[0]=(A)[0]-(B)[0];(C)[1]=(A)[1]-(B)[1];}
#define     C2V_NEGATE(A,B)  {(B)[0]=(-(A)[0]);(B)[1]=(-(A)[1]);}
#define     C2V_MID_PT(A,B,C) {(C)[0]=0.5*((A)[0]+(B)[0]);\
            (C)[1]=0.5*((A)[1]+(B)[1]);}
#define     C2V_SCALE(A,T,B) {(B)[0]=(T)*(A)[0];(B)[1]=(T)*(A)[1];}
#define     C2V_DOT(A,B)    ((A)[0]*(B)[0]+(A)[1]*(B)[1])
#define     C2V_CROSS(A,B)  ((A)[0]*(B)[1]-(A)[1]*(B)[0])
#define     C2V_NORM(A)     ((REAL)HYPOT((double)(A)[0],(double)(A)[1]))
#define     C2V_NORM_SQUARED(A) ((A)[0]*(A)[0]+(A)[1]*(A)[1])
#define     C2V_NORML1(A)   ((REAL)fabs((double)(A)[0])+(REAL)fabs((double)(A)[1]))
#define     C2V_DIST(A,B)   ((REAL)(HYPOT((double)((A)[0]-B[0]),(double)((A)[1]-(B)[1]))))
#define     C2V_DISTL1(A,B) ((REAL)fabs((double)((A)[0]-(B)[0]))+(REAL)fabs((double)((A)[1]-(B)[1])))
#define     C2V_ROTATE_VEC_CS(A,C,S,B)   \
            {(B)[0]=(A)[0]*(C)-(A)[1]*(S);(B)[1]=(A)[0]*(S)+(A)[1]*(C);}
#define     C2V_AREA(A,B,C) ((A)[0]*(B)[1]-(A)[1]*(B)[0] + \
            (B)[0]*(C)[1]-(B)[1]*(C)[0] + (C)[0]*(A)[1]-(C)[1]*(A)[0])

#else       /* !BBSMACROS */

#include    <c2vdefs.h>

#define     C2V_IS_ZERO           c2v_is_zero 
#define     C2V_IS_SMALL          c2v_is_small    
#define     C2V_IDENT_PTS         c2v_ident_pts   
#define     C2V_SET_ZERO    (void)c2v_set_zero    
#define     C2V_SET         (void)c2v_set         
#define     C2V_COPY        (void)c2v_copy        
#define     C2V_ADD         (void)c2v_add         
#define     C2V_ADDT        (void)c2v_addt        
#define     C2V_ADDW        (void)c2v_addw        
#define     C2V_ADDU        (void)c2v_addu        
#define     C2V_OFFSET      (void)c2v_offset      
#define     C2V_NORMAL      (void)c2v_normal      
#define     C2V_SUB         (void)c2v_sub     
#define     C2V_NEGATE      (void)c2v_negate      
#define     C2V_MID_PT      (void)c2v_mid_pt      
#define     C2V_SCALE       (void)c2v_scale       
#define     C2V_DOT               c2v_dot     
#define     C2V_CROSS             c2v_cross       
#define     C2V_NORM              c2v_norm        
#define     C2V_NORM_SQUARED       c2v_norm_squared        
#define     C2V_NORML1            c2v_norml1      
#define     C2V_DIST              c2v_dist        
#define     C2V_DISTL1            c2v_distl1      
#define     C2V_ROTATE_VEC_CS     (void)c2v_rotate_vec_cs
#define     C2V_AREA              c2v_area        

#endif /* BBSMACROS*/

#endif /* C2VMCRS*/


