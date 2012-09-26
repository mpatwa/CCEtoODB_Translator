/****************************** DXAPPM.H ***********************************/ 
#ifndef     DXAPPM 
#define     DXAPPM 
#ifdef __ACAD11__
#define     DXF_APPID_NAME(E)        (DX_TE_APPID(E)->appid_name)
#define     DXF_APPID_FLAGS(E)       (DX_TE_APPID(E)->appid_flags)
#endif
#endif

