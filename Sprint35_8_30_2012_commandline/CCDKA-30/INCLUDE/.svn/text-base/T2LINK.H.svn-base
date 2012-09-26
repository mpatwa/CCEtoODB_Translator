/******************************** T2LINK.H ********************************/
#ifndef T2LINK
#define T2LINK

typedef struct t2_link {
    PT2 pt ;
    struct t2_link *mate ;
    T2_PARM_S tparm ;
    INT status ;
    } T2_LINK_S, *T2_LINK ;

#include <t2linkm.h>
#include <t2linkd.h>

#ifdef __MCRSFTC__
typedef BOOLEAN (*PF_T2B_VALID) __(( T2_LINK, T2_LINK )) ; 
#else
#ifdef __BRLNDC__
typedef BOOLEAN (*PF_T2B_VALID) __(( T2_LINK, T2_LINK )) ; 
#else
#ifdef  __HIGHC__
typedef BOOLEAN (*PF_T2B_VALID) __(( T2_LINK, T2_LINK )) ; 
#else
typedef     PF_BOOLEAN  PF_T2B_VALID ; 
#endif
#endif  
#endif

#endif
