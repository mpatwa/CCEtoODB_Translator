/****************************** DXHDRD.H ***********************************/ 
#ifndef     DXHDRD 
#define     DXHDRD 
typedef     struct  dx_entity_header {
            DX_STRING   layer ;
            char        handle[DX_HANDLE_SIZE] ;
            DX_STRING   ltype ;
            REAL        thickness ;
            INT         color ;
#ifdef  __ACAD11__
            INT         ent_space ;
#endif
            REAL        extrusion[3] ; 
            } DXF_ENTITY_HEADER_S ;
typedef     DXF_ENTITY_HEADER_S *DXF_ENTITY_HEADER ;
#endif

