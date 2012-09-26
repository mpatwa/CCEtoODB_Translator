/****************************** DXDIMM.H ***********************************/
#ifndef     DXDIMM 
#define     DXDIMM 
#define     DXF_DIM_STANDARD            0
#define     DXF_DIM_ALIGNED             1
#define     DXF_DIM_ANGULAR             2
#define     DXF_DIM_DIAMETER            3
#define     DXF_DIM_RADIUS              4
#ifdef  __ACAD11__
#define     DXF_DIM_ANGULAR_3PT         5
#define     DXF_DIM_ORDINATE            6
#define     DXF_DIM_ORDINATE_XTYPE      64
#endif
#define     DXF_DIM_NAME(E)         (DX_ENTITY_DIM(E)->dim_name)
#ifdef __ACAD11__
#define     DXF_DIM_STYLE_NAME(E)   (DX_ENTITY_DIM(E)->dim_style_name)
#endif
#define     DXF_DIM_DEF_PT(E)       (DX_ENTITY_DIM(E)->dim_def_pt)
#define     DXF_DIM_MID_PT(E)       (DX_ENTITY_DIM(E)->dim_mid_pt)
#define     DXF_DIM_INSERT_PT(E)    (DX_ENTITY_DIM(E)->dim_insert_pt)
#define     DXF_DIM_TYPE(E)         (DX_ENTITY_DIM(E)->dim_type)
#define     DXF_DIM_TEXT(E)         (DX_ENTITY_DIM(E)->dim_text)
#define     DXF_DIM_DEF_PT3(E)      (DX_ENTITY_DIM(E)->dim_def_pt3)
#define     DXF_DIM_DEF_PT4(E)      (DX_ENTITY_DIM(E)->dim_def_pt4)
#define     DXF_DIM_DEF_PT5(E)      (DX_ENTITY_DIM(E)->dim_def_pt5)
#define     DXF_DIM_DEF_PT6(E)      (DX_ENTITY_DIM(E)->dim_def_pt6)
#define     DXF_DIM_LDR_LEN(E)      (DX_ENTITY_DIM(E)->dim_ldr_len)
#define     DXF_DIM_ANG(E)          (DX_ENTITY_DIM(E)->dim_ang)
#define     DXF_DIM_HORIZ(E)        (DX_ENTITY_DIM(E)->dim_horiz)
#ifdef __ACAD11__
#define     DXF_DIM_OBL_ANG(E)      (DX_ENTITY_DIM(E)->dim_obl_ang)
#define     DXF_DIM_ROT_ANG(E)      (DX_ENTITY_DIM(E)->dim_rot_ang)
#endif
#endif

