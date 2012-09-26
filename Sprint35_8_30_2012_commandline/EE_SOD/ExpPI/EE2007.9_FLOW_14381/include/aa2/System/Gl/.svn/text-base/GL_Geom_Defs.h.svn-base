#ifndef __GL_GEOM_DEFS_H__
#define __GL_GEOM_DEFS_H__

#define ASSERT_DEBUG 0
#if ASSERT_DEBUG
void SYSGL_API AssertProc(char *b);
#define ASSERT_TMP(a,b) if(!(a)) AssertProc((b))
#else
#define ASSERT_TMP(a,b)
#endif

#ifndef ASSERT_WRN
#ifdef UNIX
#define ASSERT_WRN(a,b)
#define ASSERT_ERR(a,b)
#else
#define ASSERT_WRN(a,b) ASSERT_TMP((a),(b))
#define ASSERT_ERR(a,b) ASSERT_TMP((a),(b))
#endif
#endif //ASSERT_WRN

#define	GL_BASIC_MILS 12700L	//38100L
// Design space
//#define	GL_MIN_COORD	(-28000 * (GL_BASIC_MILS))
//#define	GL_MAX_COORD	( 28000 * (GL_BASIC_MILS))
#define	GL_MAX_COORD	(0x3FFFFFFFL)
#define	GL_MIN_COORD	(-GL_MAX_COORD)
#define GL_MAX_DOUBLE_COORD ((double)GL_MAX_COORD)

//#define GL_ISVALIDCOORD(x)		((GL_MIN_COORD < (x)) && ((x) < GL_MAX_COORD))
#define IS_VALIDCOORD(x)	(true)

// NULL
#if !defined(NULL)
#define NULL	0
#endif

// SQRT2
#if !defined(_SQRT2)
#define _SQRT2	1.414213562373095048801688724209698078569671875376948
#endif

// PI
#if !defined(_PI)
#define _PI	3.14159265358979323846264338327950288419716939937510
#endif
#define _2PI			(2. * _PI)


// Basic math macros
#ifndef MIN
#define MIN(a, b)		(((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)			((x) < 0 ? -(x) : (x))
#endif

#define LROUND(x)		((long)((x) + ((x) < 0 ? -0.5 : 0.5)))
#define GL_ScanDir(a,b) (((a).x < (b).x)? 1 : ((a).x > (b).x)? -1 : ((a).y < (b).y)? 1 : ((a).y > (b).y)? -1 : 0)

#define GL_ANGLE_NORM(x)	(x)

//#define ISVALIDBANGLE(x)		((-GL_BA_360 < (x)) && ((x) < GL_BA_360))
#define GL_ISVALIDANGLE(x)	(true)
#define GL_ISVALIDCOORD(x) (true)

#define LEPS 4
#define L2_EPS 2
#define L1_EPS 1

#define GL_SGN_EPS 4.0
#define D_EPS 4.0
#define D2_EPS 2.0
#define D1_EPS 1.0
typedef void * DB_Handle;					/// from SDB_Poly


#define GL_AREA_COUNT 7
#define GL_SHFT_BITS 12	// corresponds to GL_APP_MASK

///////// GeomData::flags (Geometry Data Flags) ///////
const unsigned long GL_CIRCLE		=	0x00000001;	
const unsigned long GL_POLY			=	0x00000002;
const unsigned long GL_SDB_DDYNAMIC =	0x00000004;
const unsigned long GL_TYPE_MASK	=	0x00000003;	// Geometry type mask
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_SAS_HIDE		=   0x00000010;
const unsigned long GL_SAS_DELETE	=   0x00000020;
const unsigned long GL_SDB_DELETE	=   0x00000040;
const unsigned long GL_SAS_MARK		=	0x00000080;	// Data marked by SAS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_SAS_PROCS	=   0x00000100;
const unsigned long GL_SAS_CROSS	=   0x00000200;
const unsigned long GL_SAS_BELOW	=   0x00000400;
const unsigned long GL_SAS_ABOVE	=   0x00000800;
const unsigned long GL_SAS_CLEAR   =	0xfffff07f;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_APPL_1		=	0x00001000;
const unsigned long GL_APPL_2		=	0x00002000;
const unsigned long GL_APPL_4		=	0x00004000;
const unsigned long GL_APPL_8		=	0x00008000;
const unsigned long GL_APP_MASK		=	0x0000f000;	// Application flags mask
const unsigned long GL_APP_NEG		=	0xffff0fff;	// Application negative mask
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_ON_AREA		=	0x00010000;	// visible on free area
const unsigned long GL_ON_SOFT		=	0x00020000;	// visible on soft area
const unsigned long GL_ON_SW3		=	0x00040000;	// visible on sw3 area
const unsigned long GL_ON_WIN		=	0x00080000;	// visible on win area ???
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_AREA_BELOW	=	0x00100000;
const unsigned long GL_AREA_ABOVE	=	0x00200000;
const unsigned long GL_SOFT_BELOW	=	0x00400000;
const unsigned long GL_SOFT_ABOVE	=	0x00800000;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long GL_SW3_BELOW	=	0x01000000;
const unsigned long GL_SW3_ABOVE	=	0x02000000;
const unsigned long GL_WIND_BELOW	=	0x04000000;
const unsigned long GL_WIND_ABOVE	=	0x08000000;
const unsigned long GL_CENT_LINE	=	0x10000000;
//const unsigned long GL_SAS_CLEAR   =	0x0000ffff;	// SAS flags cleaning mask
const unsigned long GL_AREA_CLEAR	=	0x0000ffff;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const unsigned long GL_INIT			=	GL_SDB_DELETE;	// initialization
const unsigned long GL_SAS_IGNORE	=	(GL_SAS_HIDE | GL_SAS_DELETE); // ignore for proc
const unsigned long GL_SAS_NEAR		=	(GL_SAS_ABOVE | GL_SAS_BELOW); // ignore for proc

const unsigned long GL_RIGID_AREA 	= (GL_ON_AREA | GL_ON_SOFT | GL_ON_SW3) ;
const unsigned long GL_APP_RC		= (GL_APP_MASK | GL_RIGID_AREA | GL_CENT_LINE);

const unsigned long GL_APPL_24		=	(GL_APPL_2 | GL_APPL_4);

const unsigned long GL_BELOW		=	(GL_AREA_BELOW | GL_SOFT_BELOW | GL_WIND_BELOW | GL_SW3_BELOW);
const unsigned long GL_ABOVE		=	(GL_AREA_ABOVE | GL_SOFT_ABOVE | GL_WIND_ABOVE | GL_SW3_ABOVE);

const unsigned long GL_AREA			=	(GL_AREA_BELOW | GL_AREA_ABOVE);
const unsigned long GL_ARE_WIN		=	(GL_AREA_BELOW | GL_WIND_ABOVE);
const unsigned long GL_ARE_SFT		=	(GL_AREA_BELOW | GL_SOFT_ABOVE);
const unsigned long GL_ARE_SW3		=	(GL_AREA_BELOW | GL_SW3_ABOVE);

const unsigned long GL_SOFT			=	(GL_SOFT_BELOW | GL_SOFT_ABOVE);
const unsigned long GL_SFT_WIN		=	(GL_SOFT_BELOW | GL_WIND_ABOVE);
const unsigned long GL_SFT_ARE		=	(GL_SOFT_BELOW | GL_AREA_ABOVE);
const unsigned long GL_SFT_SW3		=	(GL_SOFT_BELOW | GL_SW3_ABOVE);

const unsigned long GL_WINDOW		=	(GL_WIND_BELOW | GL_WIND_ABOVE);
const unsigned long GL_WIN_ARE		=	(GL_WIND_BELOW | GL_AREA_ABOVE);
const unsigned long GL_WIN_SFT		=	(GL_WIND_BELOW | GL_SOFT_ABOVE);
const unsigned long GL_WIN_SW3		=	(GL_WIND_BELOW | GL_SW3_ABOVE);

const unsigned long GL_SW3			=	(GL_SW3_BELOW | GL_SW3_ABOVE);
const unsigned long GL_SW3_ARE		=	(GL_SW3_BELOW | GL_AREA_ABOVE);
const unsigned long GL_SW3_SFT		=	(GL_SW3_BELOW | GL_SOFT_ABOVE);
const unsigned long GL_SW3_WIN		=	(GL_SW3_BELOW | GL_WIND_ABOVE);

const unsigned long GL_WIN_AREA		=	(GL_WINDOW | GL_AREA);
const unsigned long GL_AW_BELOW		=	(GL_AREA_BELOW | GL_WIND_BELOW);
const unsigned long GL_AW_ABOVE		=	(GL_AREA_ABOVE | GL_WIND_ABOVE);
const unsigned long GL_ASW			=	(GL_AREA | GL_SOFT | GL_WINDOW | GL_SW3);
const unsigned long GL_ASW_CLEAR	=	~(GL_AREA | GL_SOFT | GL_WINDOW | GL_SW3);


/////////////// GeomHdr::hdr_flags ////////////////////////////////////
const unsigned long HDR_FLAG_FREE_MASK  = 0x00000001;	// memory slot is free
const unsigned long HDR_FLAG_CLOCKWISE  = 0x00000002;	// geometry direction
const unsigned long HDR_FLAG_FILLED     = 0x00000004;	// filled geometry
const unsigned long HDR_FLAG_ARC        = 0x00000008;	// at least one arc present in geometry
const unsigned long HDR_FLAG_ERASE      = 0x00000010;	// erased
const unsigned long HDR_FLAG_POLY_SEL   = 0x00000020;	// selected
const unsigned long HDR_FLAG_PART_SEL   = 0x00000040;	// partially selected
const unsigned long HDR_FLAG_BASE	    = 0x00000080;	// under base control
const unsigned long HDR_FLAG_TCS	    = 0x00000100;	// TCS object
const unsigned long HDR_FLAG_UP_PES	    = 0x00000200;	// PES object
const unsigned long HDR_FLAG_DN_PES	    = 0x00000400;	// PES object
const unsigned long HDR_FLAG_AUTO_PES   = 0x00000800;	// PES object
const unsigned long HDR_FLAG_THR_PULE   = 0x00001000;	// THermal's poly
const unsigned long HDR_FLAG_THR_EXT	= 0x00002000;	// thermal extension
const unsigned long HDR_FLAG_SHL_NET	= 0x00004000;	// thermal extension
const unsigned long HDR_FLAG_SECT_LIST	= 0x00008000;	// thermal extension
const unsigned long HDR_FLAG_PES		= (HDR_FLAG_UP_PES | HDR_FLAG_DN_PES | HDR_FLAG_AUTO_PES);
const unsigned long HDR_FLAG_SELECT		= (HDR_FLAG_POLY_SEL | HDR_FLAG_PART_SEL);

#define DISCARDED_BY_PES 107

inline void InvertAreaFlags (unsigned long &flags)
{
	if (flags & GL_AREA) flags ^= GL_AREA;
	if (flags & GL_SOFT) flags ^= GL_SOFT;
	if (flags & GL_SW3) flags ^= GL_SW3;
	if (flags & GL_WINDOW) flags ^= GL_WINDOW;
}
inline unsigned long GetAreaFlags (unsigned long flags, int inv = 0)
{
	flags &= GL_ASW_CLEAR;
	if (inv) InvertAreaFlags(flags);
	return flags;
}

inline double gl_sqrt (double xx)
{
   static double x0;
	if (xx) {
		x0 = sqrt(xx);
	   x0 = (x0 * x0 + xx) * 0.5 / x0;
	} else {
		x0 = 0.0;
	}
   return x0;
}
//#ifndef GL_MEM_DEBUG
//#define GL_MEM_DEBUG 1
//#endif

#endif // __GL_GEOM_DEFS_H__

//
// End of file
//

