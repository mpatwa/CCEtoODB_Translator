/****************************** DXAPDATM.H *********************************/ 
#ifndef     DXAPDATM
#define     DXAPDATM 
#ifdef  __ACAD11__
#define     DX_APPDAT_TEMPLATE(A)       ((A)->appl_template)
#define     DX_APPDAT_DATA(A)           ((A)->appl_data)
#define     DX_CREATE_APPDAT            (MALLOC(1,DX_APPDAT_S))
#define     DX_FREE_APPDAT(A)           FREE(A)
#endif
#endif


