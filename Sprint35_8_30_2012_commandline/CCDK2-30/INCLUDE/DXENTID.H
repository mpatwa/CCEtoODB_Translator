/****************************** DXENTID.H **********************************/
#ifndef     DXENTID 
#define     DXENTID 
typedef     unsigned long DXF_ENTITY_ID ;
#define     DXF_POINT         (DXF_ENTITY_ID)1L
#define     DXF_VERTEX        (DXF_ENTITY_ID)2L
#define     DXF_LINE          (DXF_ENTITY_ID)4L
#define     DXF_CIRCLE        (DXF_ENTITY_ID)8L
#define     DXF_ARC           (DXF_ENTITY_ID)16L
#define     DXF_TRACE         (DXF_ENTITY_ID)32L
#define     DXF_SOLID         (DXF_ENTITY_ID)64L
#define     DXF_POLYLINE      (DXF_ENTITY_ID)128L
#define     DXF_LINE3D        (DXF_ENTITY_ID)256L
#define     DXF_3DFACE        (DXF_ENTITY_ID)512L
#define     DXF_FACE3D        DXF_3DFACE
#define     DXF_TEXT          (DXF_ENTITY_ID)1024L
#define     DXF_SHAPE         (DXF_ENTITY_ID)2048L
#define     DXF_INSERT        (DXF_ENTITY_ID)4096L
#define     DXF_ATTDEF        (DXF_ENTITY_ID)8192L
#define     DXF_ATTRIB        (DXF_ENTITY_ID)16384L
#define     DXF_DIM           (DXF_ENTITY_ID)32768L
#ifdef __ACAD11__
#define     DXF_VIEWPORT      (DXF_ENTITY_ID)65536L
#endif
#define     DXF_CURVE         (DXF_LINE | DXF_ARC | DXF_CIRCLE | \
            DXF_SOLID | DXF_TRACE | DXF_POLYLINE | DXF_LINE3D | DXF_3DFACE )

#ifdef __ACAD11__
#define     DXF_ALL_TYPES     (DXF_CURVE | DXF_POINT | DXF_VERTEX | DXF_TEXT \
| DXF_SHAPE | DXF_INSERT | DXF_ATTDEF | DXF_ATTRIB | DXF_DIM | DXF_VIEWPORT )
#else
#define     DXF_ALL_TYPES     (DXF_CURVE | DXF_POINT | DXF_VERTEX | DXF_TEXT \
| DXF_SHAPE | DXF_INSERT | DXF_ATTDEF | DXF_ATTRIB | DXF_DIM )
#endif

#define     DXF_NONE_ID       (DXF_ENTITY_ID)0L
#endif
