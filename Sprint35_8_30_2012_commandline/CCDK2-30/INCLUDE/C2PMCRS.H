/******************************* C2PMCRS.H *********************************/
#ifndef C2PMCRS
#define C2PMCRS

#define     C2_PCURVE_BUFFER(P) (&((P)->buffer))
#define     C2_PCURVE_BLIST(P)  ((P)->buffer_list)
#define     C2_WALK_PCURVE(P,ITEM) \
for ( ITEM=dml_first(C2_PCURVE_BLIST(P)) ; ITEM!=NULL ; ITEM=dml_next(ITEM) ) 
#endif

