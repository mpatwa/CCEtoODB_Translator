/****************************** DXVTXD.H ***********************************/
#ifndef     DXVTXD
#define     DXVTXD 
typedef     struct  dx_vertex_struct {
            DXF_ENTITY_HEADER_S    vertex_entity_header ;
            REAL    vertex_pt[3] ;
            REAL    vertex_w0 ;
            REAL    vertex_w1 ;
            REAL    vertex_bulge ;
            INT     vertex_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   vertex_filler ;
#endif
#ifdef __ACAD11__
            INT     vertex_m_cnt ;
            INT     vertex_n_cnt ;
            INT     vertex_m_dens ;
            INT     vertex_n_dens ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER   vertex_filler ;
#endif
            REAL    vertex_tan ;
            } DX_VERTEX_S ;
typedef     DX_VERTEX_S *DX_VERTEX ;
#endif

