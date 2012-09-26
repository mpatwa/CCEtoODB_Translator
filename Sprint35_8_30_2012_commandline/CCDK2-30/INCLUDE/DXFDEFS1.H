/***************************** DXFDEFS1.H **********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DXFDEFS1
#define     DXFDEFS1
extern  BOOLEAN    dxf_put_appid __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 )) ;
extern  BOOLEAN    dxf_put_dimstyle __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , STRING DUMMY2 , STRING DUMMY3 , STRING DUMMY4 , 
            STRING DUMMY5 , STRING DUMMY6 , REAL DUMMY7 )) ;
extern  DXF_TABLE_ENTRY dxf_create_layer __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , INT DUMMY2 , BOOLEAN DUMMY3 , STRING DUMMY4 )) ;
extern  DXF_TABLE_ENTRY dxf_create_ltype __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , STRING DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  DXF_TABLE_ENTRY dxf_create_style __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , INT DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , 
            REAL DUMMY5 , INT DUMMY6 , REAL DUMMY7 , STRING DUMMY8 , 
            STRING DUMMY9 )) ;
extern  DXF_TABLE_ENTRY dxf_create_view __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 [3], 
            REAL DUMMY5 [3], REAL DUMMY6 [3] )) ;
extern  DXF_TABLE_ENTRY dxf_create_ucs __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1, REAL DUMMY2 [3],
            REAL DUMMY3 [3], REAL DUMMY4 [3] )) ;
extern  DXF_TABLE_ENTRY dxf_create_vport __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,  REAL DUMMY6 [2],
            REAL DUMMY7 [3], REAL DUMMY8 [3], REAL DUMMY9 , REAL DUMMY10 )) ;
extern  DXF_TABLE_ENTRY dxf_create_appid __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 )) ;
extern  DXF_TABLE_ENTRY dxf_create_dimstyle __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , STRING DUMMY2 , STRING DUMMY3 , STRING DUMMY4 , 
            STRING DUMMY5 , STRING DUMMY6 , REAL DUMMY7 )) ;
extern  void            dxf_set_entity_elevation __(( DXF_FILE DUMMY0 ,
            DXF_ENTITY DUMMY0 , REAL DUMMY1 )) ;
extern  REAL            dxf_get_entity_elevation __(( DXF_FILE DUMMY0 , 
            DXF_ENTITY DUMMY1 )) ;
extern  BOOLEAN    dxf_put_viewport __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3] , 
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 , INT DUMMY5 , 
            REAL DUMMY6 [3] , REAL DUMMY7 [3] )) ;
extern  DXF_ENTITY      dxf_create_line __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3], 
            REAL DUMMY2 [3] )) ;
extern  DXF_ENTITY      dxf_create_point __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3] )) ;
extern  DXF_ENTITY      dxf_create_circle __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 )) ;
extern  DXF_ENTITY      dxf_create_arc __(( DXF_FILE DUMMY0, REAL DUMMY1 [3], 
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  DXF_ENTITY      dxf_create_trace __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3] )) ;
extern  DXF_ENTITY      dxf_create_solid __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3] )) ;
extern  DXF_ENTITY      dxf_create_text __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 , REAL DUMMY4 , 
            REAL DUMMY5 , STRING DUMMY6 )) ;
extern  DXF_ENTITY      dxf_create_shape __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 ,  REAL DUMMY4 , 
            REAL DUMMY5 )) ;
extern  DXF_ENTITY      dxf_create_insert __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 , REAL DUMMY4 , 
            INT DUMMY5 , INT DUMMY6 , REAL DUMMY7 , REAL DUMMY8 )) ;
extern  DXF_ENTITY      dxf_insert_add_attrib __(( DXF_FILE DUMMY0 , 
            DXF_ENTITY DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 ,  
            STRING DUMMY4 , STRING DUMMY5 , INT DUMMY6 , INT DUMMY7 , 
            REAL DUMMY8 , REAL DUMMY9 , STRING DUMMY10 )) ;
extern  DXF_ENTITY      dxf_create_attdef __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 , STRING DUMMY4 , 
            STRING DUMMY5 , INT DUMMY6 , INT DUMMY7 , REAL DUMMY8 , 
            REAL DUMMY9 , STRING DUMMY10 )) ;
extern  DXF_ENTITY      dxf_create_polyline __(( DXF_FILE DUMMY0 , 
            INT DUMMY1 , INT DUMMY2 )) ;
extern  DXF_ENTITY      dxf_polyline_add_vertex __(( DXF_FILE DUMMY0 , 
            DXF_ENTITY DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 , INT DUMMY4 )) ;
extern  DXF_ENTITY      dxf_create_line3d __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3] )) ;
extern  DXF_ENTITY      dxf_create_3dface __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3], INT DUMMY5 )) ;
extern  DXF_ENTITY      dxf_create_dimension __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 [3], REAL DUMMY4 [3], 
            INT DUMMY5 , STRING DUMMY6 , REAL DUMMY7 [3], REAL DUMMY8 [3], 
            REAL DUMMY9 [3], REAL DUMMY10 [3], REAL DUMMY11 , REAL DUMMY12 )) ;
extern  DXF_ENTITY dxf_create_viewport __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3] , 
            REAL DUMMY2 , REAL DUMMY3 , INT DUMMY4 , INT DUMMY5 , 
            REAL DUMMY6 [3] , REAL DUMMY7 [3] )) ;
extern  DXF_ENTITY dxf_viewport_append_layer __(( DXF_ENTITY DUMMY0 , 
            STRING DUMMY1 )) ;
extern  DXF_ENTITY      dxf_explode_entity __(( DXF_FILE DUMMY0 ,
            DXF_ENTITY DUMMY1 , REAL DUMMY2 [3], DXF_ENTITY DUMMY3 , 
            INT DUMMY4 , INT DUMMY5 )) ;
extern  BOOLEAN         dxf_explode_entities __(( DXF_FILE DUMMY0 ,
            DXF_ENTITY DUMMY1 , REAL DUMMY2 [3],  DXF_ENTITY DUMMY3 , 
            DML_LIST DUMMY4 )) ;
extern  INT     dxf_get_file_release __(( DXF_FILE DUMMY0 )) ;
extern  void    dxf_set_file_release __(( DXF_FILE DUMMY0 , INT DUMMY1 )) ;
extern  DML_ITEM dxf_define_appl __(( DXF_FILE DUMMY0 , DX_STRING DUMMY1 )) ;
extern  BOOLEAN dxf_append_code __(( DXF_FILE DUMMY0 , DX_STRING DUMMY1 , 
            int DUMMY2 )) ;
extern  void dxf_filter_append_appl __(( DXF_FILTER DUMMY0 , STRING DUMMY1 )) ;
extern  void dxf_filter_append_all_appl __(( DXF_FILTER DUMMY0 )) ;
extern  void dxf_filter_cancel_all_appl __(( DXF_FILTER DUMMY0 )) ;
extern  void dxf_filter_cancel_appl __(( DXF_FILTER DUMMY0 , STRING DUMMY1 )) ;
extern  int* dxf_appl_template __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  BOOLEAN dxf_set_entity_field __(( DXF_ENTITY DUMMY0 , STRING DUMMY1 ,
            int DUMMY2 , int DUMMY3 , ANY DUMMY4 )) ;
extern  ANY dxf_get_entity_field __(( DXF_ENTITY DUMMY0 , STRING DUMMY1 ,
            int DUMMY2 , int DUMMY3 )) ;
#endif     /*DXFDEFS*/


