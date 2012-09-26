/******************************** T2LINKM.H ********************************/
#ifndef T2LINKM
#define T2LINKM

#define     T2_LINK_PARM(L)         (&((L)->tparm))
#define     T2_LINK_EDGE(L)         ((L)->tparm.edge)
#define     T2_LINK_LOOP(L)         (T2_EDGE_LOOP(T2_LINK_EDGE(L)))
#define     T2_LINK_REGION(L)       (T2_LOOP_REGION(T2_LINK_LOOP(L))) 
#define     T2_LINK_PT(L)           ((L)->pt)
#define     T2_LINK_CPARM(L)        (&((L)->tparm.cparm))
#define     T2_LINK_T(L)            (PARM_T(&((L)->tparm.cparm)))
#define     T2_LINK_J(L)            (PARM_J(&((L)->tparm.cparm)))
#define     T2_LINK_STATUS(L)       ((L)->status)
#define     T2_LINK_POLARITY(L)     ((L)->status)
#define     T2_LINK_MATE(L)         ((L)->mate)
#define     T2_LINK_MATE_LOOP(L)    (T2_LINK_LOOP(T2_LINK_MATE(L)))
#define     T2_ALLOC_LINK           ((T2_LINK)MALLOC(1,T2_LINK_S))
#define     T2_FREE_LINK(L)         FREE((L))
#endif
