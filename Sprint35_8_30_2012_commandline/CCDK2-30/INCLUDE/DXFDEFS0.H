/***************************** DXFDEFS0.H **********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DXFDEFS0
#define     DXFDEFS0
extern  DXF_TABLE_ENTRY dxf_create_layer __(( STRING DUMMY0 , INT DUMMY1 ,
            BOOLEAN DUMMY2 , STRING DUMMY3 )) ;
extern  DXF_TABLE_ENTRY dxf_create_ltype __(( STRING DUMMY0 , STRING DUMMY1 ,
            INT DUMMY2 , REAL DUMMY3 , INT DUMMY4 , REAL* DUMMY5 )) ;
extern  DXF_TABLE_ENTRY dxf_create_style __(( STRING DUMMY0 , INT DUMMY1 , 
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , INT DUMMY5 , 
            REAL DUMMY6 , STRING DUMMY7 , STRING DUMMY8 )) ;
extern  DXF_TABLE_ENTRY dxf_create_view __(( STRING DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 [3],  REAL DUMMY4 [3], 
            REAL DUMMY5 [3] )) ;
extern  DXF_TABLE_ENTRY dxf_create_ucs __(( STRING DUMMY0 , REAL DUMMY1 [3],
            REAL DUMMY2 [3], REAL DUMMY3 [3] )) ;
extern  DXF_TABLE_ENTRY dxf_create_vport __(( STRING DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 [2],
            REAL DUMMY6 [3], REAL DUMMY7 [3], REAL DUMMY8 , REAL DUMMY9 )) ;
extern  void            dxf_set_entity_elevation __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 )) ;
extern  REAL            dxf_get_entity_elevation __(( DXF_ENTITY DUMMY0 )) ;
extern  DXF_ENTITY      dxf_create_line __(( REAL DUMMY0 [3], 
            REAL DUMMY1 [3] )) ;
extern  DXF_ENTITY      dxf_create_point __(( REAL DUMMY0 [3] )) ;
extern  DXF_ENTITY      dxf_create_circle __(( REAL DUMMY0 [3], REAL DUMMY1 )) ;
extern  DXF_ENTITY      dxf_create_arc __(( REAL DUMMY0 [3], REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 )) ;
extern  DXF_ENTITY      dxf_create_trace __(( REAL DUMMY0 [3], REAL DUMMY1 [3],
            REAL DUMMY2 [3],  REAL DUMMY3 [3] )) ;
extern  DXF_ENTITY      dxf_create_solid __(( REAL DUMMY0 [3], REAL DUMMY1 [3],
            REAL DUMMY2 [3],  REAL DUMMY3 [3] )) ;
extern  DXF_ENTITY      dxf_create_text __(( REAL DUMMY0 [3], REAL DUMMY1 ,
            STRING DUMMY2 , REAL DUMMY3 , REAL DUMMY4 ,  STRING DUMMY5 )) ;
extern  DXF_ENTITY      dxf_create_shape __(( REAL DUMMY0 [3], REAL DUMMY1 ,
            STRING DUMMY2 ,  REAL DUMMY3 , REAL DUMMY4 )) ;
extern  DXF_ENTITY      dxf_create_insert __(( STRING DUMMY0 , REAL DUMMY1 [3],
            REAL DUMMY2 , REAL DUMMY3 ,  INT DUMMY4 , INT DUMMY5 ,
            REAL DUMMY6 , REAL DUMMY7 )) ;
extern  DXF_ENTITY      dxf_insert_add_attrib __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 [3], REAL DUMMY2 ,  STRING DUMMY3 , STRING DUMMY4 ,
            INT DUMMY5 , INT DUMMY6 , REAL DUMMY7 , REAL DUMMY8 ,
            STRING DUMMY9 )) ;
extern  DXF_ENTITY      dxf_create_attdef __(( REAL DUMMY0 [3], REAL DUMMY1 ,
            STRING DUMMY2 , STRING DUMMY3 ,  STRING DUMMY4 , INT DUMMY5 ,
            INT DUMMY6 , REAL DUMMY7 , REAL DUMMY8 , STRING DUMMY9 )) ;
extern  DXF_ENTITY      dxf_create_polyline __(( INT DUMMY0 , INT DUMMY1 )) ;
extern  DXF_ENTITY      dxf_polyline_add_vertex __(( DXF_ENTITY DUMMY0 ,
             REAL DUMMY1 [3], REAL DUMMY2 , INT DUMMY3 )) ;
extern  DXF_ENTITY      dxf_create_line3d __(( REAL DUMMY0 [3], 
            REAL DUMMY1 [3] )) ;
extern  DXF_ENTITY      dxf_create_3dface __(( REAL DUMMY0 [3], REAL DUMMY1 [3],
             REAL DUMMY2 [3], REAL DUMMY3 [3], INT DUMMY4 )) ;
extern  DXF_ENTITY      dxf_create_dimension __(( STRING DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3],  REAL DUMMY3 [3], INT DUMMY4 , 
            STRING DUMMY5 , REAL DUMMY6 [3], REAL DUMMY7 [3],  REAL DUMMY8 [3],
            REAL DUMMY9 [3], REAL DUMMY10 , REAL DUMMY11 )) ;
extern  DXF_ENTITY      dxf_explode_entity __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 [3], DXF_ENTITY DUMMY2 ,  INT DUMMY3 , INT DUMMY4 )) ;
extern  BOOLEAN         dxf_explode_entities __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 [3],  DXF_ENTITY DUMMY2 , DML_LIST DUMMY3 )) ;
#endif     /*DXFDEFS*/


