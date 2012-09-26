/****************************** DX3FCM.H ***********************************/
#ifndef     DX3FCM
#define     DX3FCM
#define     DXF_3DFACE_E1_INVIS      1
#define     DXF_3DFACE_E2_INVIS      2
#define     DXF_3DFACE_E3_INVIS      4
#define     DXF_3DFACE_E4_INVIS      8

#define     DXF_FACE3D_E1_INVIS      DXF_3DFACE_E1_INVIS
#define     DXF_FACE3D_E2_INVIS      DXF_3DFACE_E2_INVIS
#define     DXF_FACE3D_E3_INVIS      DXF_3DFACE_E3_INVIS
#define     DXF_FACE3D_E4_INVIS      DXF_3DFACE_E4_INVIS
#define     DXF_3DFACE_PT0(E)       (DX_ENTITY_3DFACE(E)->face3d_pt0)
#define     DXF_3DFACE_PT1(E)       (DX_ENTITY_3DFACE(E)->face3d_pt1)
#define     DXF_3DFACE_PT2(E)       (DX_ENTITY_3DFACE(E)->face3d_pt2)
#define     DXF_3DFACE_PT3(E)       (DX_ENTITY_3DFACE(E)->face3d_pt3)
#define     DXF_3DFACE_FLAGS(E)     (DX_ENTITY_3DFACE(E)->face3d_flags)

#define     DXF_FACE3D_PT0       DXF_3DFACE_PT0
#define     DXF_FACE3D_PT1       DXF_3DFACE_PT1
#define     DXF_FACE3D_PT2       DXF_3DFACE_PT2
#define     DXF_FACE3D_PT3       DXF_3DFACE_PT3
#define     DXF_FACE3D_FLAGS     DXF_3DFACE_FLAGS
#endif

