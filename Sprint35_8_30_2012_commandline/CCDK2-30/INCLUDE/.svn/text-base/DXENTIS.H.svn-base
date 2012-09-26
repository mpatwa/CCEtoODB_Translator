/****************************** DXENTIS.H **********************************/
/*****************  Include file for DXF file manipulation  ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DXENTIS 
#define     DXENTIS 
#define     DXF_ENTITY_IS_POINT(E)      (DX_ENTITY_ID(E)==DXF_POINT)
#define     DXF_ENTITY_IS_LINE(E)       (DX_ENTITY_ID(E)==DXF_LINE)
#define     DXF_ENTITY_IS_CIRCLE(E)     (DX_ENTITY_ID(E)==DXF_CIRCLE)
#define     DXF_ENTITY_IS_ARC(E)        (DX_ENTITY_ID(E)==DXF_ARC)
#define     DXF_ENTITY_IS_TRACE(E)      (DX_ENTITY_ID(E)==DXF_TRACE)
#define     DXF_ENTITY_IS_SOLID(E)      (DX_ENTITY_ID(E)==DXF_SOLID)
#define     DXF_ENTITY_IS_TEXT(E)       (DX_ENTITY_ID(E)==DXF_TEXT)
#define     DXF_ENTITY_IS_SHAPE(E)      (DX_ENTITY_ID(E)==DXF_SHAPE)
#define     DXF_ENTITY_IS_INSERT(E)     (DX_ENTITY_ID(E)==DXF_INSERT)
#define     DXF_ENTITY_IS_ATTDEF(E)     (DX_ENTITY_ID(E)==DXF_ATTDEF)
#define     DXF_ENTITY_IS_ATTRIB(E)     (DX_ENTITY_ID(E)==DXF_ATTRIB)
#define     DXF_ENTITY_IS_VERTEX(E)     (DX_ENTITY_ID(E)==DXF_VERTEX)
#define     DXF_ENTITY_IS_PLINE(E)      (DX_ENTITY_ID(E)==DXF_POLYLINE)
#define     DXF_ENTITY_IS_LINE3D(E)     (DX_ENTITY_ID(E)==DXF_LINE3D)
#define     DXF_ENTITY_IS_3DFACE(E)     (DX_ENTITY_ID(E)==DXF_3DFACE)
#define     DXF_ENTITY_IS_DIM(E)        (DX_ENTITY_ID(E)==DXF_DIM)
#define     DXF_ENTITY_IS_CURVE(E)      (DX_ENTITY_ID(E)&DXF_CURVE)
#define     DXF_ENTITY_IS_FACE3D        DXF_ENTITY_IS_3DFACE
#ifdef __ACAD11__
#define     DXF_ENTITY_IS_VIEWPORT(E)    (DX_ENTITY_ID(E)==DXF_VIEWPORT)
#endif

#define     DXF_ENTITY_LTYPE_STATUS(E)   ((E)->entity_ltype_status)
#define     DXF_ENTITY_COLOR_STATUS(E)   ((E)->entity_color_status)
#define     DXF_ENTITY_APPDAT_LIST(E)    ((E)->entity_appdat_list)
#define     DXF_ENTITY_LAYER(E)  (DX_ENTITY_LINE(E)->line_entity_header.layer)
#define     DXF_ENTITY_HANDLE(E) (DX_ENTITY_LINE(E)->line_entity_header.handle)
#define     DXF_ENTITY_LTYPE(E)  (DX_ENTITY_LINE(E)->line_entity_header.ltype)
#define     DXF_ENTITY_THICKNESS(E)  \
                (DX_ENTITY_LINE(E)->line_entity_header.thickness)
#define     DXF_ENTITY_COLOR(E)   (DX_ENTITY_LINE(E)->line_entity_header.color)
#ifdef __ACAD11__
#define     DXF_ENTITY_SPACE(E)   \
                (DX_ENTITY_LINE(E)->line_entity_header.ent_space)
#endif
#define     DXF_ENTITY_EXTRUSION(E)  \
                (DX_ENTITY_LINE(E)->line_entity_header.extrusion)
#endif

