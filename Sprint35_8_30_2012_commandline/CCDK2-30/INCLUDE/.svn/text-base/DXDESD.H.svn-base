/****************************** DXDESD.H ***********************************/ 
#ifndef     DXDESD 
#define     DXDESD 
typedef     struct  dx_entity_desc {
            DX_STRING   layer ;
            char        handle[DX_HANDLE_SIZE] ;
            INT         ltype_status ;
#ifdef __UNIX__
            DX_INT_FILLER   desc_filler ;
#endif
            DX_STRING   ltype ;
#ifdef __MSC32__
            DX_INT_FILLER   desc_filler ;
#endif
            REAL        elevation ;
            REAL        thickness ;
            INT         color_status ;
            INT         color ;
            REAL        extrusion[3] ; 
            } DXF_ENTITY_DESC_S ;
typedef     DXF_ENTITY_DESC_S *DXF_ENTITY_DESC ;
#endif

