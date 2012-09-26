/*******************************  QGLCMPL.H ********************************/
/**************** include file for compilation conditions ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     QGLCMPL
#define     QGLCMPL
#define     BBSCMPL

#define     LINT_ARGS
#define     QGL_PRINT
#define     BBS_PRINT   QGL_PRINT
#define     QGLMACROS

#ifdef      __MCRSFTC__
#define     __PROTOTYPES__
#define     __MCRGRAPH__
#define     __REALMODE__
#endif

#ifdef      __TURBOC__
#define     __PROTOTYPES__
#define     __REALMODE__
#endif

#ifdef      __HIGHC__
/*
#define     __PROTOTYPES__
*/
#ifndef     __HIGHC30__
#define     __GFX__
#endif
#define     __PROTMODE__
#ifdef     DEBUG
#pragma On(Codeview)
#endif
#endif

/*
#ifdef      __HIGHC24__
#define     __PROTOTYPES__
#define     __GFX__
#define     __PROTMODE__
#ifdef     DEBUG
#pragma On(Codeview)
#endif
#endif
*/

#ifdef      __HIGHC30__
#define     __MCRGRAPH__
#endif

#ifdef      __WATCOM__
#define     __PROTOTYPES__
#define     __MCRGRAPH__
#define     __PROTMODE__
#endif

#ifdef      __NDP__
#ifdef      __PROTOTYPES__
#undef      __PROTOTYPES__
#endif
#define     __GFX__
#endif

#ifdef      __MPW__
#define     __PROTOTYPES__
#endif

#ifdef __PROTOTYPES__
#define   PROTOTYPES
#ifndef   __
#define   __(X)  X
#endif
#else
#ifndef   __
#define   __(X)  ()
#endif
#endif /* __PROTOTYPES__ */

#ifdef PROTARGS
#define     DUMMY0      dummy0
#define     DUMMY1      dummy1
#define     DUMMY2      dummy2
#define     DUMMY3      dummy3
#define     DUMMY4      dummy4
#define     DUMMY5      dummy5
#define     DUMMY6      dummy6
#define     DUMMY7      dummy7
#define     DUMMY8      dummy8
#define     DUMMY9      dummy9
#define     DUMMY10     dummy10
#define     DUMMY11     dummy11
#define     DUMMY12     dummy12
#define     DUMMY13     dummy13
#define     DUMMY14     dummy14
#define     DUMMY15     dummy15
#define     DUMMY16     dummy16
#define     DUMMY17     dummy17
#define     DUMMY18     dummy18
#define     DUMMY19     dummy19
#define     DUMMY20     dummy20
#define     DUMMY21     dummy21
#define     DUMMY22     dummy22
#define     DUMMY23     dummy23
#define     DUMMY24     dummy24
#define     DUMMY25     dummy25
#define     DUMMY26     dummy26
#define     DUMMY27     dummy27
#define     DUMMY28     dummy28
#define     DUMMY29     dummy29
#else
#define     DUMMY0
#define     DUMMY1
#define     DUMMY2
#define     DUMMY3
#define     DUMMY4
#define     DUMMY5
#define     DUMMY6
#define     DUMMY7
#define     DUMMY8
#define     DUMMY9
#define     DUMMY10
#define     DUMMY11
#define     DUMMY12
#define     DUMMY13
#define     DUMMY14
#define     DUMMY15
#define     DUMMY16
#define     DUMMY17
#define     DUMMY18
#define     DUMMY19
#define     DUMMY20
#define     DUMMY21
#define     DUMMY22
#define     DUMMY23
#define     DUMMY24
#define     DUMMY25
#define     DUMMY26
#define     DUMMY27
#define     DUMMY28
#define     DUMMY29
#endif

#ifdef      NEW_CODE
#undef      NEW_CODE
#endif    /*NEW_CODE*/

#endif /* QGLCMPL*/

