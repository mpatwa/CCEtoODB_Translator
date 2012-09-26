/******************************** T2PUNCH.H ********************************/
/************** definitions and macros for punching and nesting ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef T2PUNCH
#define T2PUNCH

typedef struct t2_nest_part 
    {
    REAL width ;
    REAL height ;
    ANY ref_ptr ;
    INT n ;
    } T2_NEST_PART_S, *T2_NEST_PART ;

#define     T2_NEST_PART_WIDTH(P)   ((P)->width)
#define     T2_NEST_PART_HEIGHT(P)  ((P)->height)
#define     T2_NEST_PART_REF_PTR(P) ((P)->ref_ptr)
#define     T2_NEST_PART_N(P)       ((P)->n)

typedef struct t2_placement
    {
    T2_NEST_PART part ;
    PT2 pt ;
    } T2_PLACEMENT_S, *T2_PLACEMENT ;

#define     T2_PLACEMENT_PART(P)    ((P)->part)
#define     T2_PLACEMENT_PT(P)      ((P)->pt)
#define     T2_PLACEMENT_X(P)       (T2_PLACEMENT_PT(P)[0])
#define     T2_PLACEMENT_Y(P)       (T2_PLACEMENT_PT(P)[1])

#endif  /* T2PUNCH */

