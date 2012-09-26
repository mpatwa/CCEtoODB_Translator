/****************************** DXCODE.H ***********************************/ 
#ifndef     DXCODE 
#define     DXCODE 
#ifdef __ACAD11__
#define     DX_CODE_STRING(C)      \
(((C)>=0&&(C)<10)||((C)==999)||((C)>=1000&&(C)<1010))
#define     DX_CODE_POINT(C)       (((C)>=10&&(C)<19)||((C)>=210&&(C)<219)) 
#define     DX_CODE_REAL(C)        \
(((C)>=10&&(C)<60)||((C)>=210&&(C)<240)||((C)>=1010&&(C)<1060)) 
#define     DX_CODE_INT(C)         (((C)>=60&&(C)<80)||((C)>=1060&&(C)<1080))
#define     DX_CODE_COLOR(C)       ((C)==62) 
#else
#define     DX_CODE_STRING(C)      (((C)>=0&&(C)<10)||((C)==999))
#define     DX_CODE_POINT(C)       (((C)>=10&&(C)<19)||((C)>=210&&(C)<219)) 
#define     DX_CODE_REAL(C)        (((C)>=10&&(C)<60)||((C)>=210&&(C)<240)) 
#define     DX_CODE_INT(C)         ((C)>=60&&(C)<80) 
#define     DX_CODE_COLOR(C)       ((C)==62) 
#endif
#define     DX_CODE_ANGLE(C)       (((C)>=50)&&((C)<=58))
#define     DX_CODE_NEW_ITEM(C)    ((C)==0||(C)==9) 
#define     DX_CODE_MODE(C)        ((C)>= -1020&&(C)<-1000)
#endif

