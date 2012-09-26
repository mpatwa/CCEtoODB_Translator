/****************************** DXENTM.H ***********************************/
#ifndef     DXENTM 
#define     DXENTM 
#define     DX_ENTITY_ENAME(E)      ((E)->entity_name)
#define     DX_ENTITY_ID(E)         ((E)->entity_id)
#define     DX_ENTITY_ANY(E)        (&((E)->entity_data.entity_anytype))
#ifdef  __ACAD11__
#define     DX_ENTITY_APPDAT_LIST(E) ((E)->entity_appdat_list)
#endif
#define     DX_ENTITY_LINE(E)       (&((E)->entity_data.entity_line))
#define     DX_ENTITY_POINT(E)      (&((E)->entity_data.entity_point))
#define     DX_ENTITY_CIRCLE(E)     (&((E)->entity_data.entity_circle))
#define     DX_ENTITY_ARC(E)        (&((E)->entity_data.entity_arc))
#define     DX_ENTITY_TRACE(E)      (&((E)->entity_data.entity_trace))
#define     DX_ENTITY_SOLID(E)      (&((E)->entity_data.entity_solid))
#define     DX_ENTITY_TEXT(E)       (&((E)->entity_data.entity_text))
#define     DX_ENTITY_SHAPE(E)      (&((E)->entity_data.entity_shape))
#define     DX_ENTITY_INSERT(E)     (&((E)->entity_data.entity_insert))
#define     DX_ENTITY_ATTDEF(E)     (&((E)->entity_data.entity_attdef))
#define     DX_ENTITY_ATTRIB(E)     (&((E)->entity_data.entity_attrib))
#define     DX_ENTITY_VERTEX(E)     (&((E)->entity_data.entity_vertex))
#define     DX_ENTITY_PLINE(E)      (&((E)->entity_data.entity_pline))
#define     DX_ENTITY_LINE3D(E)     (&((E)->entity_data.entity_line3d))
#define     DX_ENTITY_3DFACE(E)     (&((E)->entity_data.entity_3dface))
#define     DX_ENTITY_DIM(E)        (&((E)->entity_data.entity_dim))
#ifdef __ACAD11__
#define     DX_ENTITY_VIEWPORT(E)   (&((E)->entity_data.entity_vp))
#endif
#endif

