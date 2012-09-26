/******************************** T2REGNM.H ********************************/
#ifndef T2REGNM
#define T2REGNM

#define     T2_REGION_LOOP_LIST(R)      ((R)->loop_list)
#define     T2_REGION_EXT_LOOP(R)  \
((T2_LOOP)DML_FIRST_RECORD(T2_REGION_LOOP_LIST(R)))
#define     T2_REGION_ATTR(R)           ((R)->attr)
#define     T2_REGION_PARENT(R)         ((R)->parent)
#define     T2_REGION_BOX(R)            (T2_LOOP_BOX(T2_REGION_EXT_LOOP(R)))
#define     T2_ALLOC_REGION             ((T2_REGION)MALLOC(1,T2_REGION_S))
#define     T2_FREE_REGION(R)           FREE((R))
#endif
