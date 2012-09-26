/****************************** DXTBLM.H ***********************************/ 
#ifndef     DXTBLM 
#define     DXTBLM 

#define     DX_TE_ENAME(E) ((E)->te_ename)
#define     DX_TE_ID(E)    ((E)->te_id)
#define     DX_TE_ANY(E)   (&((E)->te_data.te_anytype))
#define     DX_TE_LTYPE(E) (&((E)->te_data.te_ltype))
#define     DX_TE_LAYER(E) (&((E)->te_data.te_layer))
#define     DX_TE_STYLE(E) (&((E)->te_data.te_style))
#define     DX_TE_VIEW(E)  (&((E)->te_data.te_view))
#define     DX_TE_UCS(E)   (&((E)->te_data.te_ucs))
#define     DX_TE_VPORT(E) (&((E)->te_data.te_vport))
#ifdef __ACAD11__
#define     DX_TE_APPID(E) (&((E)->te_data.te_appid))
#define     DX_TE_DIMSTYLE(E) (&((E)->te_data.te_dimstyle))
#endif
#endif
