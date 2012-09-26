/******************************* C2NMCRS.H *********************************/
#ifndef C2NMCRS
#define C2NMCRS

#ifdef  SPLINE
#define     C2_NURB_N(B)        ((B)->nctpts)
#define     C2_NURB_D(B)        ((B)->order)
#define     C2_NURB_J(B)        ((B)->index)
#define     C2_NURB_CTPT_PTR(B) ((B)->ctl_pt)
#define     C2_NURB_CTPT(B)     ((HPT2*)(C2_NURB_CTPT_PTR(B)))
#define     C2_NURB_KNOT_PTR(B) ((B)->knot)
#define     C2_NURB_KNOT(B)     ((REAL*)(C2_NURB_KNOT_PTR(B)))
#define     C2_NURB_CURV_EXTR_PTR(B)    ((B)->curv_extr_array)
#define     C2_NURB_CURV_EXTR(B) ((C2_CURV_EXTR_S*)(C2_NURB_CURV_EXTR_PTR(B)))
#define     C2_NURB_CURV_EXTR_NO(B) ((B)->curv_extr_no)
#define     C2_NURB_W(B)        ((B)->offset)
#endif  /* SPLINE */
#endif


