/****************************** DXBLCM.H ***********************************/ 
#ifndef     DXBLCM
#define     DXBLCM 
#define     DXF_BLOCK_ENAME(B)       ((B)->block_ename)
#define     DXF_BLOCK_HEADER(B)      (&((B)->block_entity_header))
#define     DXF_BLOCK_LAYER(B)       ((B)->block_entity_header.layer)
#define     DXF_BLOCK_HANDLE(B)      ((B)->block_entity_header.handle)
#define     DXF_BLOCK_LTYPE(B)       ((B)->block_entity_header.ltype)
#define     DXF_BLOCK_THICKNESS(B)   ((B)->block_entity_header.thickness)
#define     DXF_BLOCK_COLOR(B)       ((B)->block_entity_header.color)
#define     DXF_BLOCK_EXTRUSION(B)   ((B)->block_entity_header.extrusion)
#define     DXF_BLOCK_NAME(B)        ((B)->block_name)
#define     DXF_BLOCK_FLAGS(B)       ((B)->block_flags)
#define     DXF_BLOCK_BASE_PT(B)     ((B)->block_base_pt)
#define     DX_CREATE_BLOCK          (MALLOC(1,DXF_BLOCK_S))
#define     DX_FREE_BLOCK(B)         FREE(B)
#ifdef  __ACAD11__
#define     DXF_BLOCK_NAME3(B)       ((B)->block_name3)
#define     DXF_BLOCK_XREF_PATHNAME(B)       ((B)->block_xref_pathname)
#endif

#define     DXF_BLOCK_IS_ANON           1
#define     DXF_BLOCK_HAS_ATTR          2
#ifdef  __ACAD11__
#define     DXF_BLOCK_IS_XREF           4
#define     DXF_BLOCK_EXT_DEPT          16
#define     DXF_BLOCK_EXT_REF           32
#define     DXF_BLOCK_DEF_IS_REFD       64
#endif

#endif

