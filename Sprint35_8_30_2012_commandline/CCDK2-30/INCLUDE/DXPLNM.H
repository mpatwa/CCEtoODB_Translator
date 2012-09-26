/****************************** DXPLNM.H ***********************************/
#ifndef     DXPLNM
#define     DXPLNM 
#define     DXF_PLINE_CLSD          1
#define     DXF_PLINE_FIT           2
#define     DXF_PLINE_SPL_FIT       4
#define     DXF_PLINE_3D            8
#define     DXF_PLINE_3D_MESH       16
#define     DXF_PLINE_CLSD_N        32
#ifdef  __ACAD11__
#define     DXF_PLINE_IS_PFACE      64
#endif

#define     DXF_PLINE_NSMOOTH        0
#define     DXF_PLINE_QUADR          5
#define     DXF_PLINE_CUBIC          6
#define     DXF_PLINE_BEZIER         8
#define     DXF_PLINE_VFLAG(E)      (DX_ENTITY_PLINE(E)->pline_vflag)
#define     DXF_PLINE_ORIGIN(E)     (DX_ENTITY_PLINE(E)->pline_origin)
#define     DXF_PLINE_FLAGS(E)      (DX_ENTITY_PLINE(E)->pline_flags)
#define     DXF_PLINE_W0(E)         (DX_ENTITY_PLINE(E)->pline_w0)
#define     DXF_PLINE_W1(E)         (DX_ENTITY_PLINE(E)->pline_w1)
#define     DXF_PLINE_M_CNT(E)      (DX_ENTITY_PLINE(E)->pline_m_cnt)
#define     DXF_PLINE_N_CNT(E)      (DX_ENTITY_PLINE(E)->pline_n_cnt)
#define     DXF_PLINE_M_DENS(E)     (DX_ENTITY_PLINE(E)->pline_m_dens)
#define     DXF_PLINE_N_DENS(E)     (DX_ENTITY_PLINE(E)->pline_n_dens)
#define     DXF_PLINE_TYPE(E)       (DX_ENTITY_PLINE(E)->pline_type)
#define     DXF_PLINE_SEQEND(E)     (DX_ENTITY_PLINE(E)->pline_seqend) 
// ###!!! #define     DXF_PLINE_VLIST(E)      (DX_ENTITY_PLINE(E)->pline_vlist)
#define     DXF_PLINE_VLIST(E)      (DX_ENTITY_PLINE(E)->pline_vlist)
#endif

