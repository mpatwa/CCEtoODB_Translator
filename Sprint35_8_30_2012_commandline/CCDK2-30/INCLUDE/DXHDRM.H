/****************************** DXHDRM.H ***********************************/ 
#ifndef     DXHDRM
#define     DXHDRM 
#define     DXF_HEADER_LAYER(H)       ((H)->layer)
#define     DXF_HEADER_HANDLE(H)      ((H)->handle)
#define     DXF_HEADER_LTYPE(H)       ((H)->ltype)
#define     DXF_HEADER_THICKNESS(H)   ((H)->thickness) 
#define     DXF_HEADER_COLOR(H)       ((H)->color) 
#ifdef __ACAD11__
#define     DXF_HEADER_ENT_SPACE(H)   ((H)->ent_space) 
#endif
#define     DXF_HEADER_EXTRUSION(H)   ((H)->extrusion) 
#define     DX_CREATE_HEADER          (MALLOC(1,DXF_ENTITY_HEADER_S))
#define     DX_FREE_HEADER(H)         FREE(H)
#endif

