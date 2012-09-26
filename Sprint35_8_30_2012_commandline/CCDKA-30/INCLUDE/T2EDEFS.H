/******************************* T2EDEFS.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2EDEFS
#define T2EDEFS

#include <bbsdefs.h>
#include <dmdefs.h>
#include <c2defs.h>
#include <t2defs.h>

extern DML_LIST t2e_build_regions __(( DML_LIST, INT, DML_LIST, INT, 
            REAL, DML_LIST )) ;
extern DML_LIST t2e_build_loops __(( DML_LIST, INT, DML_LIST, INT, 
            REAL, DML_LIST )) ;
extern T2_LOOP t2e_build_loop __(( T2_REGION, DML_LIST, REAL )) ;
extern void t2e_split_loop __(( T2_LOOP, T2_LOOP*, T2_LOOP* )) ;
extern T2_LOOP t2e_join_loops __(( T2_LOOP, T2_LOOP )) ;
#endif

