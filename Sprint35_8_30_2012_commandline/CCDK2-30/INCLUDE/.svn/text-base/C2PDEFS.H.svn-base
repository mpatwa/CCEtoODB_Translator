/******************************** C2PDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C2PDEFS
#define     C2PDEFS
#include <c2defs.h>
#include <dmdefs.h>

extern  BOOLEAN     c2p_eval __(( C2_PCURVE DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT2* DUMMY3 )) ;
extern  INT         c2p_n __(( C2_PCURVE DUMMY0 )) ;
extern  BOOLEAN     c2p_pt_tan __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            PT2 DUMMY2 , PT2 DUMMY3 )) ;
extern  BOOLEAN     c2p_ept0 __(( C2_PCURVE DUMMY0 , PT2 DUMMY2 )) ;
extern  BOOLEAN     c2p_ept1 __(( C2_PCURVE DUMMY0 , PT2 DUMMY2 )) ;
extern  void        c2p_box __(( C2_PCURVE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , C2_BOX DUMMY3 )) ;
extern  void        c2p_box_append __(( C2_PCURVE DUMMY0 , C2_BOX DUMMY1 )) ;
extern  C2_ASEG     c2p_segment __(( C2_PCURVE DUMMY0 , INT DUMMY1 )) ;
extern  BOOLEAN     c2p_segment_epts_d __(( C2_PCURVE DUMMY0 , INT DUMMY1 , 
            PT2 DUMMY2 , PT2 DUMMY3 , REAL* DUMMY4 )) ;
extern  C2_ASEG     c2p_first_segm __(( C2_PCURVE DUMMY0 )) ;
extern  C2_ASEG     c2p_last_segm __(( C2_PCURVE DUMMY0 )) ;
extern  C2_PC_BUFFER c2p_first_buffer __(( C2_PCURVE DUMMY0 )) ;
extern  C2_PC_BUFFER c2p_last_buffer __(( C2_PCURVE DUMMY0 )) ;
extern  INT         c2p_coord_extrs __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , DML_LIST DUMMY4 )) ;
extern  REAL        c2p_length __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c2p_select __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 , REAL DUMMY4 ,  REAL* DUMMY5 ,
            REAL* DUMMY6 )) ;
extern  BOOLEAN     c2p_project __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT2 DUMMY3 , REAL* DUMMY4 , PT2 DUMMY5 )) ;
extern  INT         c2p_intersect __(( C2_PCURVE DUMMY0 , C2_PCURVE DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  INT         c2p_inters_line __(( C2_PCURVE DUMMY0 , C2_LINE DUMMY1 ,
            DML_LIST DUMMY2 )) ;
extern  INT         c2p_inters_arc __(( C2_PCURVE DUMMY0 , C2_ARC DUMMY1 ,
            BOOLEAN DUMMY2 , DML_LIST DUMMY3 )) ;
extern  INT         c2p_self_inters __(( C2_PCURVE DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN     c2p_approx __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , BOOLEAN DUMMY4 ,  PT2* DUMMY5 ,
            PARM DUMMY6 , INT DUMMY7 , INT* DUMMY8 , REAL* DUMMY9 )) ;
extern  BOOLEAN     c2p_approx_zoomed __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 ,
            REAL DUMMY6 , PT2* DUMMY7 , PARM DUMMY8 , INT DUMMY9 ,
            INT* DUMMY10 , REAL* DUMMY11 )) ;
extern  C2_PCURVE   c2p_create __(( INT DUMMY0 , INT DUMMY1 )) ;
extern  C2_PCURVE   c2p_trim __(( C2_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  void        c2p_init __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  void        c2p_free __(( C2_PCURVE DUMMY0 )) ;
extern  C2_PCURVE   c2p_through __(( PT2* DUMMY0 , INT DUMMY1 )) ;
extern  C2_PCURVE   c2p_create_copy __(( C2_PCURVE DUMMY0 )) ;
extern  C2_PCURVE   c2p_add __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  C2_PCURVE   c2p_add_arc_2pts __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  C2_PCURVE   c2p_add_arc_ctr_pt __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 ,
            PT2 DUMMY2 )) ;
extern  C2_PCURVE   c2p_add_arc_tan __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_PCURVE   c2p_add_arc_tan0 __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 , 
            PT2 DUMMY2 )) ;
extern  C2_PCURVE   c2p_add_line __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_PCURVE   c2p_add_line_tan __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  C2_PCURVE   c2p_remove_last __(( C2_PCURVE DUMMY0 )) ;
extern  C2_PCURVE   c2p_close __(( C2_PCURVE DUMMY0 )) ;
extern  DML_LIST    c2p_offset __(( C2_PCURVE DUMMY0 , REAL DUMMY1 )) ;
extern  REAL        c2p_curvature __(( C2_PCURVE DUMMY0 , REAL DUMMY1 )) ;
extern  void        c2p_rotate_cs __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , C2_PCURVE DUMMY4 )) ;
extern  void        c2p_translate __(( C2_PCURVE DUMMY0 , PT2 DUMMY1 )) ;
extern  void        c2p_scale __(( C2_PCURVE DUMMY0 , PT2 DUMMY1, 
            REAL DUMMY2 )) ;
extern  void        c2p_mirror_dir __(( C2_PCURVE DUMMY0 , PT2 DUMMY1, 
            PT2 DUMMY2 )) ;
extern  void        c2p_transform __(( C2_PCURVE DUMMY0 , C2_TRANSFORM DUMMY1 ,
            C2_PCURVE DUMMY2 )) ;
extern  BOOLEAN     c2p_closed __(( C2_PCURVE DUMMY0 )) ;
extern  void        c2p_info __(( C2_PCURVE DUMMY0 , FILE* DUMMY1 )) ;
extern  C2_PCURVE   c2p_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN     c2p_put __(( FILE* DUMMY0 , C2_PCURVE DUMMY1 , 
            PARM DUMMY2 , PARM DUMMY3 )) ;
extern  BOOLEAN c2p_get_data __(( C2_PCURVE DUMMY0 , INT DUMMY1 , PT2 DUMMY2 ,
            PT2 DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 , REAL* DUMMY6 , 
            INT* DUMMY7 )) ;
#endif /* C2PDEFS*/

