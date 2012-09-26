/******************************** T2SPRLM.H ********************************/
#ifndef T2SPRLM
#define T2SPRLM
#define     T2_SPIRAL_LOOP_LIST(S)  ((S)->loop_list)
#define     T2_SPIRAL_OPEN_LIST(S)  ((S)->open_list)
#define     T2_SPIRAL_LIST(S)       ((S)->spiral_list)
#define     T2_SPIRAL_PARENT(S)     ((S)->spiral_parent)
#define     T2_SPIRAL_LOOP_INIT(S)  ((S)->loop_init)
#define     T2_SPIRAL_INIT(S)       \
(T2_LOOP_SPIRAL((T2_LOOP)T2_SPIRAL_LOOP_INIT(S)))
#define     T2_SPIRAL_PT(S)         ((S)->pt)
#define     T2_SPIRAL_PARM0(S)      (&((S)->parm0))
#define     T2_SPIRAL_EDGE0(S)      (T2_PARM_EDGE(T2_SPIRAL_PARM0(S)))
#define     T2_SPIRAL_CPARM0(S)     (T2_PARM_CPARM(T2_SPIRAL_PARM0(S)))
#define     T2_SPIRAL_T0(S)         (PARM_T(T2_SPIRAL_CPARM0(S)))
#define     T2_SPIRAL_J0(S)         (PARM_J(T2_SPIRAL_CPARM0(S)))
#define     T2_SPIRAL_PARM1(S)      (&((S)->parm1))
#define     T2_SPIRAL_EDGE1(S)      (T2_PARM_EDGE(T2_SPIRAL_PARM1(S)))
#define     T2_SPIRAL_CPARM1(S)     (T2_PARM_CPARM(T2_SPIRAL_PARM1(S)))
#define     T2_SPIRAL_T1(S)         (PARM_T(T2_SPIRAL_CPARM1(S)))
#define     T2_SPIRAL_J1(S)         (PARM_J(T2_SPIRAL_CPARM1(S)))
#endif

