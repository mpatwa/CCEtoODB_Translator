/***************************** DX0DEFS0.H **********************************/
/*******************  Prototypes for dxf_get routines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX0DEFS0
#define     DX0DEFS0
extern  DXF_ENTITY  dx0_alloc_entity __(( DXF_ENTITY_ID DUMMY0 )) ;
extern  DXF_TABLE_ENTRY dx0_alloc_table_entry __(( 
            DXF_TABLE_ENTRY_ID DUMMY0 )) ;
extern  int*        dx0_table_entry_template __(( DXF_TABLE_ENTRY_ID DUMMY0 )) ;
extern  int*        dx0_entity_template __(( DXF_ENTITY_ID DUMMY0 )) ;
extern  void        dx0_set_entity_descriptor __(( DXF_ENTITY DUMMY0 ,
            DXF_ENTITY_DESC DUMMY1 )) ;
extern  DXF_ENTITY  dx0_create_attrib __(( REAL DUMMY0 [3], REAL DUMMY1 ,
            STRING DUMMY2 , STRING DUMMY3 , INT DUMMY4 ,  INT DUMMY5 ,
            REAL DUMMY6 , REAL DUMMY7 , STRING DUMMY8 )) ;
extern  DXF_ENTITY  dx0_create_vertex __(( REAL DUMMY1 [3], 
            REAL DUMMY2 , INT DUMMY3 )) ;
extern  DXF_BLOCK   dx0_alloc_block __(( void )) ;
#endif     /*DX0DEFS0*/


