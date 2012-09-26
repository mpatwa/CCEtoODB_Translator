/****************************** DXTRAD.H ***********************************/ 
#ifndef     DXTRAD
#define     DXTRAD 
typedef     struct  dx_trace_struct {
            DXF_ENTITY_HEADER_S    trace_entity_header ;
            REAL    trace_pt0[3] ;
            REAL    trace_pt1[3] ;
            REAL    trace_pt2[3] ;
            REAL    trace_pt3[3] ;
            } DX_TRACE_S ;
typedef     DX_TRACE_S *DX_TRACE ;
#endif

