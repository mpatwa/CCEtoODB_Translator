/****************************** DX3FCD.H ***********************************/
#ifndef     DX3FCD 
#define     DX3FCD 
typedef     struct  dx_3dface_struct {
            DXF_ENTITY_HEADER_S    face3d_entity_header ;
            REAL    face3d_pt0[3] ;
            REAL    face3d_pt1[3] ;
            REAL    face3d_pt2[3] ;
            REAL    face3d_pt3[3] ;
            INT     face3d_flags ;
            } DX_3DFACE_S ;

typedef     DX_3DFACE_S *DX_3DFACE ;
#define     DX_FACE3D_S DX_3DFACE_S
#define     DX_FACE3D   DX_3DFACE
#endif
