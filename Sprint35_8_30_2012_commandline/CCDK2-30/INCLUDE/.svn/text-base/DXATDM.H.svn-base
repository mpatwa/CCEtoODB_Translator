/****************************** DXATDM.H ***********************************/
/* 06/18/93 ANN : Added ifdef "ACAD11" */

#ifndef     DXATDM 
#define     DXATDM 

#define     DXF_ATTDEF_INVIS            1
#define     DXF_ATTDEF_IS_CONST         2
#define     DXF_ATTDEF_VER_REQ          4
#define     DXF_ATTDEF_IS_PRESET        8
#define     DXF_ATTDEF_PT(E)        (DX_ENTITY_ATTDEF(E)->attdef_pt)
#define     DXF_ATTDEF_HEIGHT(E)    (DX_ENTITY_ATTDEF(E)->attdef_height)
#define     DXF_ATTDEF_VALUE(E)     (DX_ENTITY_ATTDEF(E)->attdef_value)
#define     DXF_ATTDEF_PROMPT(E)    (DX_ENTITY_ATTDEF(E)->attdef_prompt)
#define     DXF_ATTDEF_TAG(E)       (DX_ENTITY_ATTDEF(E)->attdef_tag)
#define     DXF_ATTDEF_AFLAGS(E)    (DX_ENTITY_ATTDEF(E)->attdef_aflags)
#define     DXF_ATTDEF_FLD_LEN(E)   (DX_ENTITY_ATTDEF(E)->attdef_fld_len)
#define     DXF_ATTDEF_ROT_ANG(E)   (DX_ENTITY_ATTDEF(E)->attdef_rot_ang)
#define     DXF_ATTDEF_X_SCALE(E)   (DX_ENTITY_ATTDEF(E)->attdef_x_scale)
#define     DXF_ATTDEF_OBL_ANG(E)   (DX_ENTITY_ATTDEF(E)->attdef_obl_ang)
#define     DXF_ATTDEF_STYLE(E)     (DX_ENTITY_ATTDEF(E)->attdef_style)
#define     DXF_ATTDEF_GFLAGS(E)    (DX_ENTITY_ATTDEF(E)->attdef_gflags)
#define     DXF_ATTDEF_AL_PT(E)     (DX_ENTITY_ATTDEF(E)->attdef_al_pt)
#ifdef __ACAD11__
#define     DXF_ATTDEF_HOR_JUST(E)    (DX_ENTITY_ATTDEF(E)->attdef_hor_just)
#define     DXF_ATTDEF_VERT_JUST(E)   (DX_ENTITY_ATTDEF(E)->attdef_vert_just)
#define     DXF_ATTDEF_JUST(E)        DXF_ATTDEF_HOR_JUST(E)
#else
#define     DXF_ATTDEF_JUST(E)        (DX_ENTITY_ATTDEF(E)->attdef_just)
#endif

#endif

