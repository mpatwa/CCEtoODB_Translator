/******************************** T2EDGEM.H ********************************/
#ifndef T2EDGEM
#define T2EDGEM
#define     T2_EDGE_LOOP(E)             ((E)->loop)
#define     T2_EDGE_CURVE(E)            ((E)->curve)
#define     T2_EDGE_BOX(E)              (C2_CURVE_BOX(T2_EDGE_CURVE(E)))
#define     T2_EDGE_DIR(E)              ((E)->dir)
#define     T2_EDGE_REVERSED(E)         (((E)->dir)==(-1))
#define     T2_EDGE_ATTR(E)             ((E)->attr)
#define     T2_EDGE_PARENT(E)           ((E)->parent)
#define     T2_EDGE_REF(E)              ((E)->ref)
#define     T2_EDGE_PARM0(E)            \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_PARM0((E)->curve)):(C2_CURVE_PARM1((E)->curve)))
#define     T2_EDGE_PARM1(E)            \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_PARM1((E)->curve)):(C2_CURVE_PARM0((E)->curve)))
#define     T2_EDGE_T0(E)               \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_T0((E)->curve)):(C2_CURVE_T1((E)->curve)))
#define     T2_EDGE_J0(E)               \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_J0((E)->curve)):(C2_CURVE_J1((E)->curve)))
#define     T2_EDGE_T1(E)               \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_T1((E)->curve)):(C2_CURVE_T0((E)->curve)))
#define     T2_EDGE_J1(E)               \
((T2_EDGE_DIR(E)==1)?(C2_CURVE_J1((E)->curve)):(C2_CURVE_J0((E)->curve)))
#define     T2_EDGE_T_MIN(E)            (C2_CURVE_T0((E)->curve))
#define     T2_EDGE_T_MAX(E)            (C2_CURVE_T1((E)->curve))
#define     T2_EDGE_VTX(E)              ((E)->vtx)
#define     T2_ALLOC_EDGE               ((T2_EDGE)MALLOC(1,T2_EDGE_S))
#define     T2_FREE_EDGE(E)             FREE((E))
#define     T2_EDGE_IS_LINE(E)          (C2_CURVE_IS_LINE((E)->curve))
#define     T2_EDGE_IS_ARC(E)           (C2_CURVE_IS_ARC((E)->curve))
#define     T2_EDGE_IS_ZERO_ARC(E)      (C2_CURVE_IS_ZERO_ARC((E)->curve))
#define     T2_EDGE_SC_PT(E)            ((E)->sc_pt)
#endif

