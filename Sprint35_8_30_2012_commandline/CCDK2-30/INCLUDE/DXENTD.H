/****************************** DXENTD.H ***********************************/
#ifndef     DXENT 
#define     DXENT 
typedef     struct  dx_entity_struct {
            DXF_ENAME           entity_name ;
            DXF_ENTITY_ID       entity_id ;
            INT                 entity_ltype_status ;
            INT                 entity_color_status ;
#ifdef  __ACAD11__
            DML_LIST            entity_appdat_list ;
/*            INT                 entity model_space ; jak ### */
#endif
            union {
                STRING          entity_anytype ;
#ifdef DX_ENT_ON
                DX_LINE_S       entity_line ;
                DX_POINT_S      entity_point ;
                DX_CIRCLE_S     entity_circle ;
                DX_ARC_S        entity_arc ;
                DX_TRACE_S      entity_trace ;
                DX_SOLID_S      entity_solid ;
                DX_TEXT_S       entity_text ;
                DX_SHAPE_S      entity_shape ;
                DX_INSERT_S     entity_insert ;
                DX_ATTDEF_S     entity_attdef ;
                DX_ATTRIB_S     entity_attrib ;
                DX_VERTEX_S     entity_vertex ;
                DX_POLYLINE_S   entity_pline ;
                DX_LINE3D_S     entity_line3d ;
                DX_3DFACE_S     entity_3dface ;
                DX_DIM_S        entity_dim ;
#ifdef __ACAD11__
                DX_VIEWPORT_S   entity_vp ;
#endif  /*__ACAD11__*/
#endif  /* DX_ENT_ON */
            }   entity_data ;
            } DXF_ENTITY_S ;
typedef     DXF_ENTITY_S *DXF_ENTITY ;
#endif

