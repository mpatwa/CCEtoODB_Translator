/******************************** T2XMAX.H ********************************/
#ifndef T2XMAX
#define T2XMAX
typedef struct x_max_struct { 
    REAL x_max_value ;
    T2_PARM_S x_max_parm ; 
    BOOLEAN x_max_ortn ;
    BOOLEAN x_max_pos ;
} T2_X_MAX_S, *T2_X_MAX ;
#define T2_X_MAX_VALUE(X)  ((X)->x_max_value)
#define T2_X_MAX_ORTN(X)   ((X)->x_max_ortn)
#define T2_X_MAX_POS(X)    ((X)->x_max_pos)
#define T2_X_MAX_PARM(X)   (&((X)->x_max_parm))
#define T2_X_MAX_LOOP(X)   (T2_EDGE_LOOP(T2_PARM_EDGE(T2_X_MAX_PARM(X))))
#endif
