/******************************** T2XRECM.H ********************************/
#ifndef T2XRECM
#define T2XRECM
#define     T2_EXTR_REC_EDGE(E)        ((E)->edge)
#define     T2_EXTR_REC_POSITION(E)    ((E)->position)
#define     T2_EXTR_REC_T(E)           ((E)->t)
#define     T2_EXTR_REC_TYPE(E)        ((E)->type)

#define     T2_ALLOC_EXTR_REC    ((T2_EXTR_REC) MALLOC (1, T2_EXTR_REC_S ) )
#define     T2_FREE_EXTR_REC(N)  FREE ( (N) )

/*### jak */
#define     T2_LOC_EXTR_REC_TYPE(E)     ((E)->type)
#define     T2_LOC_EXTR_REC_RANGE(E)    ((E)->range)
#define     T2_LOC_EXTR_REC_TPARM0(E)   (&((E)->tparm0))
#define     T2_LOC_EXTR_REC_TPARM1(E)   (&((E)->tparm1))
#define     T2_LOC_EXTR_REC_VALUE(E)    ((E)->value)

#define     T2_ALLOC_LOC_EXTR_REC    ((T2_LOC_EXTR_REC) MALLOC (1, T2_LOC_EXTR_REC_S ) )
#define     T2_FREE_LOC_EXTR_REC(N)  FREE ( (N) )

#define     T2_LOC_MIN      -1
#define     T2_LOC_MAX       1
#define     T2_LOC_INFL      0

#define     T2_UP       -1
#define     T2_DOWN      1
#define     T2_FLAT      0

#define     T2_POINT_EXTR       1
#define     T2_INTERVAL_EXTR    2

/*### jak */

#endif

