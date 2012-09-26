/******************************** T2PARMM.H ********************************/
#ifndef T2PARMM
#define T2PARMM

#define     T2_PARM_EDGE(P)     ((P)->edge)
#define     T2_PARM_CPARM(P)    (&((P)->cparm))
#define     T2_PARM_T(P)        (PARM_T(T2_PARM_CPARM(P)))
#define     T2_PARM_J(P)        (PARM_J(T2_PARM_CPARM(P)))
#define     T2_PARM_LOOP(P)     (T2_EDGE_LOOP(T2_PARM_EDGE(P)))
#define     T2_ALLOC_PARM       ((T2_PARM)MALLOC(1,T2_PARM_S))
#define     T2_FREE_PARM(P)     FREE((P))
#define     T2_COPY_PARM(P0,P1) {T2_PARM_EDGE(P1)=T2_PARM_EDGE(P0);\
                PARM_T(T2_PARM_CPARM(P1))=PARM_T(T2_PARM_CPARM(P0));\
                PARM_J(T2_PARM_CPARM(P1))=PARM_J(T2_PARM_CPARM(P0));}

#endif

