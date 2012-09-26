/******************************* C2CXTRM.H *********************************/
#ifndef C2CXTRM
#define C2CXTRM

#ifdef  SPLINE

#define     C2_CURV_EXTR_PARM(R) (&((R)->parm))
#define     C2_CURV_EXTR_T(R)    (PARM_T(C2_CURV_EXTR_PARM(R)))
#define     C2_CURV_EXTR_J(R)    (PARM_J(C2_CURV_EXTR_PARM(R)))
#define     C2_CURV_EXTR_CURV(R) ((R)->curv)

#define     C2_COPY_CURV_EXTR(R0,R1) {\
C2_CURV_EXTR_T(((C2_CURV_EXTR_S*)R1))=C2_CURV_EXTR_T(((C2_CURV_EXTR_S*)R0));\
C2_CURV_EXTR_J(((C2_CURV_EXTR_S*)R1))=C2_CURV_EXTR_J(((C2_CURV_EXTR_S*)R0));\
C2_CURV_EXTR_CURV(((C2_CURV_EXTR_S*)R1))=\
C2_CURV_EXTR_CURV(((C2_CURV_EXTR_S*)R0));}

#endif  /* SPLINE */
#endif  /* C2CXTRM */
