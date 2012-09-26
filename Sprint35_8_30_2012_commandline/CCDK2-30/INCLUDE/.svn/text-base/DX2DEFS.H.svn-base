/***************************** DX2DEFS.H ***********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX2DEFS
#define     DX2DEFS

#include <dxfdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
extern  PT2*        dx2_entity_to_point __(( DXF_ENTITY DUMMY0 )) ;
extern  C2_CURVE    dx2_entity_to_curve __(( DXF_ENTITY DUMMY0 )) ;
#ifdef __ACAD11__
extern  DXF_ENTITY  dx2_point_to_entity __(( DXF_FILE DUMMY0 , PT2 DUMMY1 )) ;
extern  DXF_ENTITY  dx2_curve_to_entity __(( DXF_FILE DUMMY0 , 
            C2_CURVE DUMMY1 )) ;
#else
extern  DXF_ENTITY  dx2_point_to_entity __(( PT2 DUMMY0 )) ;
extern  DXF_ENTITY  dx2_curve_to_entity __(( C2_CURVE DUMMY0 )) ;
#endif
extern  PT2*        dx2_get_point __(( DXF_FILE DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  DML_LIST    dx2_get_points __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DML_LIST    dx2_read_points __(( STRING DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  C2_CURVE    dx2_get_curve __(( DXF_FILE DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  DML_LIST    dx2_get_curves __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DML_LIST    dx2_read_curves __(( STRING DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2 , DML_LIST DUMMY3 )) ;
extern  BOOLEAN     dx2_put_point __(( DXF_FILE DUMMY0 , PT2 DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx2_put_points __(( DXF_FILE DUMMY0 , DML_LIST DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx2_write_points __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            DML_LIST DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
extern  BOOLEAN     dx2_put_curve __(( DXF_FILE DUMMY0 , C2_CURVE DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx2_put_curves __(( DXF_FILE DUMMY0 , DML_LIST DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx2_write_curve __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            C2_CURVE DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
extern  BOOLEAN     dx2_write_curves __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            DML_LIST DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
extern  DML_LIST dx2_pface_to_curves __(( DXF_ENTITY DUMMY0 , 
            DML_LIST DUMMY1 )) ;
#endif     /*DX2DEFS*/

