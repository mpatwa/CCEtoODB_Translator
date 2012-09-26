/******************************** C2HPT2.H *********************************/
#ifndef     C2HPT2
#define     C2HPT2
#include    <bbsdefs.h>
typedef     REAL            HPT2[3];    /* 2d homogeneous point */
#define     HPT2_X(P)       ((P)[0]/(P)[2]) 
#define     HPT2_Y(P)       ((P)[1]/(P)[2]) 
#define     HPT2_W(P)       ((P)[2]) 
#define     HPT2_INF(P)     ((P)[2]<=BBS_ZERO) 
#endif  /* C2HPT2 */

