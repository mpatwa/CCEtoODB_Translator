/******************************* T2EPRIV.H *********************************/
/***************************  2-D Topology *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2EPRIV
#define T2EPRIV

#include <bbsdefs.h>
#include <dmdefs.h>
#include <t2defs.h>
extern void t2e_decomp_edgelist __(( DML_LIST, DML_LIST, INT, 
            DML_LIST, INT )) ;
extern BOOLEAN t2e_append_edgelist __(( T2_LOOP, DML_LIST, REAL )) ; 
extern DML_LIST t2e_append_curvelist __(( DML_LIST, ATTR, DML_LIST )) ; 
#endif

