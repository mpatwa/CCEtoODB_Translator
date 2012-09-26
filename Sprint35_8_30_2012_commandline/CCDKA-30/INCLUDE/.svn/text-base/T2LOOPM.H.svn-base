/******************************** T2LOOPM.H ******************************/
#ifndef T2LOOPM
#define T2LOOPM
#define     T2_LOOP_REGION(L)           ((L)->owner.region)
#define     T2_LOOP_SPIRAL(L)           ((L)->owner.spiral)
#define     T2_LOOP_EDGE_LIST(L)        ((L)->edge_list)
#define     T2_LOOP_ATTR(L)             ((L)->attr)
#define     T2_LOOP_PARENT(L)           ((L)->parent)
#define     T2_LOOP_CLOSED(L)           ((L)->closed)
#define     T2_LOOP_BOX(L)              (&((L)->loop_box))
#define     T2_LOOP_MIN_X(L)            (C2_MIN_X(T2_LOOP_BOX(L)))
#define     T2_LOOP_MAX_X(L)            (C2_MAX_X(T2_LOOP_BOX(L)))
#define     T2_LOOP_MIN_Y(L)            (C2_MIN_Y(T2_LOOP_BOX(L)))
#define     T2_LOOP_MAX_Y(L)            (C2_MAX_Y(T2_LOOP_BOX(L)))
#define     T2_LOOP_MIN_PT(L)           (C2_MIN_PT(T2_LOOP_BOX(L)))
#define     T2_LOOP_MAX_PT(L)           (C2_MAX_PT(T2_LOOP_BOX(L)))
#define     T2_LOOP_LINK_LIST(L)        ((L)->link_list)
#define     T2_ALLOC_LOOP               ((T2_LOOP)MALLOC(1,T2_LOOP_S))
#define     T2_FREE_LOOP(L)             FREE((L))
#endif

