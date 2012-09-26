/******************************* C2LMCRS.H *********************************/
#ifndef C2LMCRS
#define C2LMCRS

#define     C2_LINE_PT0(L)      ((L)->pt[0])
#define     C2_LINE_PT1(L)      ((L)->pt[1])
#define     C2_LINE_DIR_VEC(L,DV) C2V_SUB (C2_LINE_PT1(L), C2_LINE_PT0(L), DV)
#endif


