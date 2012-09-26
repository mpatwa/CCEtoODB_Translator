/****************************** DXFLTD.H ***********************************/
#ifndef     DXFLTD 
#define     DXFLTD 
typedef     struct          dx_filter_struct {
            DXF_ENTITY_ID   filter_entity_type ;
            DX_STRING       filter_layer ;
            INT             filter_color_status ;
            INT             filter_color ;
#ifdef  __ACAD11__
            DML_LIST        filter_appl_list ;
            INT             filter_all_appls ;
#endif
            } DXF_FILTER_S ;

typedef     DXF_FILTER_S  *DXF_FILTER ;
#endif

