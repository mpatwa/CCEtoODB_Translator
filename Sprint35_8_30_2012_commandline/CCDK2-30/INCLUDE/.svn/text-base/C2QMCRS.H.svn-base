/******************************* C2QMCRS.H *********************************/
#ifndef     C2QMCRS
#define     C2QMCRS
#define     C2_PC_BUFFER_N(A)      ((A)->n)
#define     C2_PC_BUFFER_SIZE(A)   ((A)->a_size)
#define     C2_PC_BUFFER_A(A)      ((A)->a)
#define     C2_PC_BUFFER_ARC(A,I)  ((C2_ASEG)(C2_PC_BUFFER_A(A)+3*(I)))
#define     C2_PC_BUFFER_ARC0(A)   (C2_PC_BUFFER_ARC(A,0))
#define     C2_PC_BUFFER_ARC1(A)   (C2_PC_BUFFER_ARC(A,C2_PC_BUFFER_N(A)-2))
#define     C2_PC_BUFFER_PT(A,I)   (C2_PC_BUFFER_A(A)+3*(I))
#define     C2_PC_BUFFER_D(A,I)    (C2_PC_BUFFER_A(A)[3*(I)+2])
#define     C2_PC_BUFFER_PT0(A)    (C2_PC_BUFFER_PT(A,0))
#define     C2_PC_BUFFER_PT1(A)    (C2_PC_BUFFER_PT(A,C2_PC_BUFFER_N(A)-1))
#endif

