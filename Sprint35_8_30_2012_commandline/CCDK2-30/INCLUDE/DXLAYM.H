/****************************** DXLAYM.H ***********************************/ 
#ifndef     DXLAYM 
#define     DXLAYM 
#define     DXF_LAYER_ON_THAWED     0
#define     DXF_LAYER_FROZEN        1
#define     DXF_LAYER_NOT_USED      2
#define     DXF_LAYER_HAS_VP_EXCL   3
#define     DXF_LAYER_NAME(E)       (DX_TE_LAYER(E)->layer_name)
#define     DXF_LAYER_FLAGS(E)      (DX_TE_LAYER(E)->layer_flags)
#define     DXF_LAYER_COLOR(E)      (DX_TE_LAYER(E)->layer_color)
#define     DXF_LAYER_LTYPE(E)      (DX_TE_LAYER(E)->layer_ltype)
#endif
