/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// This is a part of the PowerSYS Public Declarations
// Copyright (C) 1996 PADS Software Inc.
// All rights reserved.

// Name:				
// Date:				
// Author:			
// Description:

#ifndef __SGU_H__
#define __SGU_H__

#include "GL_GeomIter.h"
#include "GL_Geom.h"		//dbg
#include "GL_PolyIter.h"	//dbg

#define PTOFFARCPOLY 0
#define PTONARCPOLY 2
#define PTINARCPOLY 1


const unsigned long PT_OFF_EXT_BOUND	=	0x00000001;
const unsigned long PT_ON_EXT_BOUND	=	0x00000002;
const unsigned long PT_IN_EXT_BOUND	=	0x00000004;
const unsigned long PT_ON_INT_BOUND	=	0x00000008;
const unsigned long PT_IN_INT_BOUND	=	0x00000010;

const unsigned long PT_OFF_AREA  = (PT_OFF_EXT_BOUND | PT_IN_INT_BOUND);
const unsigned long PT_IN_AREA   =  PT_IN_EXT_BOUND;
const unsigned long PT_ON_BOUND  = (PT_ON_EXT_BOUND | PT_ON_INT_BOUND);

const unsigned long TNG_CLOCKWS			= 1;
const unsigned long TNG_CNTRCLOCKWS		= 2;
const unsigned long TNG_CRS_CLOCKWS		= 4;
const unsigned long TNG_CRS_CNTRCLOCKWS	= 8;
const unsigned long TNG_CONVEX			= 16;
const unsigned long TNG_EXTERNAL		= 32;

// Flags for modes of straightening polyline data
const unsigned long PLN_MERGE_U_TURN	=	0x00000001;
const unsigned long PLN_MERGE_END_SEGS	=	0x00000002;
const unsigned long PLN_CENT_BY_RAD		=	0x00000004;
const unsigned long PLN_CENT_BY_CENT	=	0x00000008;

#define COUNTER_CLOCKWISE_ORDER  2
#define CLOCKWISE_ORDER			-2
#define SCALE_COORD				(1.0/GL_BASIC_MILS)

class glPoly;				//dbg
class GeomHdr;
class IterPolyGeom;
class GL_Array;
class DrawPolyline;

struct PolyRef {																	// monotonous segment reference structure
	GeomHdr *poly;																	// parent polyline
	double		t1, t2;																// double coordinates of first and last points (t=index+dx/l)
	GL_Point	p1, p2;																// first and last points 
};

class TngPnt : public GL_DPoint														// GL_DPoint - coordinates of tangent point
{
public:
	GeomHdr *poly;																	// source polyline
	long	polyId;																	// ID of tangent point
}; 

class	SYSGL_API SGU 
{

public:

static	int		GetDistFromNodeToPoint (const GL_Node	&node, const GL_Point &p,double* dist,GL_Point *nearest);
static	bool	CheckDistFromNodeToPoint (const GL_Node	&node, const GL_Point &p,double* dist,GL_Point *nearest);
static	void	SQGetDistP_P(const GL_Point &p1,const GL_Point &p2,double *dist);
static	bool	SQCheckDistP_P(const GL_Point &p1,const GL_Point &p2,double *dist);
static	void	SQGetDistP_L(const GL_Point &v,const GL_Point &a,const GL_Point &b,double *dist,GL_Point *nearest);
static	bool	SQCheckDistP_L(const GL_Point &v,const GL_Point &a,const GL_Point &b,double *dist,GL_Point *nearest);
static	void	SQGetDistP_A(const GL_Point &v, const GL_Point &s,const GL_Point &e,const GL_Point &c,long r,double *dist,GL_Point *nearest);
static	bool	SQCheckDistP_A(const GL_Point &v, const GL_Point &s,const GL_Point &e,const GL_Point &c,long r,double *dist,GL_Point *nearest);
static	void	SQGetDistL_L(const GL_Point &s1, const GL_Point &e1,const GL_Point &s2, const GL_Point &e2,double *dist,GL_Point *nearest1,GL_Point *nearest2);
static	bool	SQCheckDistL_L(const GL_Point &s1, const GL_Point &e1,const GL_Point &s2, const GL_Point &e2,double *dist,GL_Point *nearest1,GL_Point *nearest2);
static	void	SQGetDistA_A(const GL_Point &s1, const GL_Point &e1, const GL_Point &c1, long r1,
										 const GL_Point &s2, const GL_Point &e2, const GL_Point &c2, long r2,
										 double *dist,GL_Point *nearest1,GL_Point *nearest2);
static	bool	SQCheckDistA_A(const GL_Point &s1, const GL_Point &e1, const GL_Point &c1, long r1,
										 const GL_Point &s2, const GL_Point &e2, const GL_Point &c2, long r2,
										 double *dist,GL_Point *nearest1,GL_Point *nearest2);

static	void	SQGetDistL_A(const GL_Point &s1, const GL_Point &e1,
										 const GL_Point &s2, const GL_Point &e2, const GL_Point &c2, long r2,
										 double *dist,GL_Point *nearest1,GL_Point *nearest2);
static	bool	SQCheckDistL_A(const GL_Point &s1, const GL_Point &e1,
										 const GL_Point &s2, const GL_Point &e2, const GL_Point &c2, long r2,
										 double *dist,GL_Point *nearest1,GL_Point *nearest2);

static	void	GetBoxForArc (const GL_Point &s, const GL_Point &e, const GL_Point &c, long r,GL_Box *box);
static	bool	IsPointProectToArc (const GL_Point &v, const GL_Point &s, const GL_Point &e, const GL_Point &c, long r);
static	double	VectorProduct(const GL_Point &p1,const GL_Point &p2,const GL_Point &p3,const GL_Point &p4);
static	double	ScalarProduct(const GL_Point &p1,const GL_Point &p2,const GL_Point &p3,const GL_Point &p4);
static	bool	IsPointInNode(const GL_Node &node,const GL_Point &p,long epsilon = 4);


static	void	PolyDel(glPoly *poly);
static	void	PolyCopy(const glPoly &poly_from, glPoly *poly_to);
static	void	PolyGetLeftRight(const glPoly &poly, GL_Point *pntl, GL_Point *pntr);
static	void	PolyGetMinMax(const glPoly &poly, long *y_min, long *y_max);
static	void	PolyXMinMax(const glPoly &poly, long x, long *ymin, long *y_max);
static	void	PolyCut(long xl, long xr, glPoly *poly);
static	void	CopyPolyToPoly(long from_start,long from_end,long to_start,const glPoly &poly_from,glPoly *poly_to);
static	int		GetMonoPart(IterPolyGeom *p,glPoly *poly);
static	void	JoinPoly(const glPoly &p1,bool not_inverted1, glPoly *poly);
static	void	PolyAnimate(const GL_Node &poly); /// Draw().
static	void	RemoveSmallPieces (IterPolyGeom	*iter,long	eps);
static	bool	GetReplacePoint(IterPolyGeom *iter,GL_Point *p,long eps);
static	void	LinConvPoly(IterPolyGeom *a_iter,const GL_Transform &conv);
static	void	PolyAnimate(const glPoly &poly);
static	void	PolyReduceAligned(IterPolyGeom *a_iter);
static	int		PtInPoly(const GL_Point *p, const GeomHdr *gh, const long eps = LEPS);
static	int		GetPntId(const GL_Point *p, const GeomHdr *gh, const long eps = LEPS);
static	unsigned long	PtInArea(const GL_Point *p, const GeomHdr *gh, const long eps = LEPS);
static	int		PntInElm(const GL_Point *p,GL_Point *f,GL_Point *s,GL_Point *cent,long r);
static	int		VerticesOrder(const GeomHdr *gh);
static	double	GetArea(const GeomHdr *gh, int add_width = 0);
static	double	GetLength(const GeomHdr *gh);
static  int		GetPolyCenter(GeomHdr *gh, GL_DPoint &cent, double &poly_area, int use_list = 0); 

static	void	SetEps(long	a_eps);
static	long	GetEps();
static	long	GetMinShape(const GeomHdr &a_poly );
static	bool	GetMaxShape(const GeomHdr	* a_poly, double * a_size);
static	long	GetDistance0(const GeomHdr &h1, const GeomHdr &h2, GL_Point *p1, GL_Point *p2, bool chk_w = false);
static	bool	SQCheckDistance0(	const GeomHdr &a_hdr1,const GeomHdr &h2, GL_Point *p1, GL_Point *p2, double *a_sqdist, bool chk_w = false);
static	long	GetDistance0( IterPolyGeom &a_geom1, IterPolyGeom &a_geom2, GL_Point *p1, GL_Point *p2, bool chk_w = false);
static	bool	SQCheckDistance0(	IterPolyGeom &a_geom1, IterPolyGeom &a_geom2, GL_Point *p1, GL_Point *p2, double *a_sqdist, bool chk_w = false);

static	int		IsMono (const	IterPolyGeom	&a_iter);
static	void	GetMonoContours( GeomHdr *a_src, GeomHdr *a_top, GeomHdr *a_bottom,int a_flgs=0);
static	void	GetMonoContours( GeomHdr **a_src, int a_sz, GeomHdr *a_top, GeomHdr *a_bottom,int a_flgs=0);
static	GeomHdr	*GetTopMonoContour( GeomHdr *a_src,int a_flgs=0);
static	GeomHdr	*GetBottomMonoContour( GeomHdr *a_src,int a_flgs=0);
static	GeomHdr	*GetTopMonoContour ( GeomHdr **a_src, int a_arrsz,int a_flgs=0);
static	GeomHdr	*GetBottomMonoContour( GeomHdr **a_src, int a_arrsz,int a_flgs=0);

static	void	ConvertToMono(	glPoly **a_src_chain, const int a_src_count, glPoly ***a_mono_chain, int *a_mono_count);

static	int	 	IsConvex    ( const GeomHdr &a_poly );
static	GeomHdr	*GetConvexContour( GeomHdr **a_src, int a_arrsz,int a_flgs=0);
static	GeomHdr	*GetConvexContour( GeomHdr *a_src,int a_flgs=0);


static	GeomHdr* ConvertArcApproximations(const GeomHdr *a_poly, long a_eps);
static	int		ApproximateArc(const GL_Point &start, const GL_Point &end, const GL_Point &center, long r, int a_exterior, GL_Array *arr, double eps);
static	int		ApproximateAllArcs(GeomHdr *a_gh, int a_exterior, GL_Array *a_arr, double a_eps);
static	void	ApproximateArc(const GL_Point &start, const GL_Point &end, const GL_Point &center, long r, int a_exterior, DrawPolyline *drw_poly, double eps);
static	int		IsPolyInBox( const GeomHdr *a_gh, GL_Box *a_box);

static	bool	GetTangents( GL_Point &a_pnt, GeomHdr &a_poly, GL_Point *a_tng1, GL_Point *a_tng2, long *a_id1=NULL, long *a_id2=NULL,
										 unsigned long a_flgs=TNG_CLOCKWS|TNG_CNTRCLOCKWS, int a_ieps=-1);
static	bool	GetTangents( double a_dirx, double a_diry, GeomHdr	&a_poly, GL_Point *a_tng1, GL_Point *a_tng2,
										 long *a_id1=NULL, long *a_id2=NULL);
static	int		GetTangents( GeomHdr &a_poly1, GeomHdr &a_poly2, GL_Point *a_tng1, GL_Point *a_tng2, long *a_id1=NULL, long *a_id2=NULL,
											 unsigned long	a_flgs=TNG_CLOCKWS|TNG_CNTRCLOCKWS|TNG_CRS_CLOCKWS|TNG_CRS_CNTRCLOCKWS, int	a_ieps=-1);
static	int		GetOrthoTangents( GeomHdr &a_poly1,GeomHdr &a_poly2,GL_Point *a_ort,GL_Point *a_tng,long *a_id1,long *a_id2, int a_ieps=-1);
static	int		GetRubberLines( TngPnt	&a_beg,	TngPnt &a_end, TngPnt &a_p1, TngPnt &a_p2, GL_Array *a_tangents, int a_i1, int a_i2);

static	int		AdjustArcs( GeomHdr *gh, char *mess);
static	void	GetExtents(const GeomHdr *gh, GL_Box *ext_box, bool a_add_width = true);
static	int		StraightenPoly(GeomHdr *gh,	double dist, unsigned int flag = 0);
static	void	MoveGeom(GeomHdr *poly, GL_Point &v);
static GeomHdr	*MergeTwoPolylines(GeomHdr *g1, GeomHdr *g2, int dir2 = 1);
static GeomHdr	*CopyGeom(GeomHdr *gh);
static GeomHdr	*CopyGeomList(GeomHdr *first);
static GeomHdr	*BoxPolygon(long x1, long y1, long x2, long y2);
static GeomHdr	*SegPolygon(long x1, long y1, long x2, long y2, double rad);
static GeomHdr	*CirPolygon(long x1, long y1, double rad, int is_filled = 0);
static GeomHdr  *CopyWithoutCircles(GeomHdr *gh);
static GeomHdr  *CopyWithExtend(GeomHdr *gh, double dist = D_EPS);
static GeomHdr	*OctagonPolygon(long x1, long y1, long x2, long y2, long d1, long d2);
static GeomHdr	*SegPoly(long x1, long y1, long x2, long y2, long w);
static int		IsPolyCircle(GeomHdr *gh, GL_Point &c1, long &r1);
static int		IsBox(const GL_Point *p1, const GL_Point *p2, const GL_Point *p3, const GL_Point *p4);
static int		IsBox(const GL_Point *p1, const GL_Point *p2, const GL_Point *p3, const GL_Point *p4, GL_DPoint &c, GL_DPoint &v, GL_DPoint &k);
static int		IsBox(GeomHdr *poly);
static int		IsBox(GeomHdr *poly, GL_DPoint &c, GL_DPoint &v, GL_DPoint &k);
static	int	 	IsClosed(GeomHdr *poly);
static GeomHdr  *SmoothPoly(GeomHdr *a_src, long a_height, int a_sign, bool	a_allcross=true);
static GeomHdr	*CreateCentPoly(GL_Node *n);
static int		CreateArrayOfExpandLines(GeomHdr *gh, long eps, long dist, GL_Array &exp);
static void CutChamfers
(	GeomHdr *a_poly1,	// left contour of a expand
	GeomHdr *a_poly2,	// right contour of a expand
	double a_maxdist,	// maximum  distance from chamfer to the contour
	double a_maxangle	// max processed angle of the corner
);
static GeomHdr	*ConnectPolylines(GeomHdr *poly_list);
static void		SortHoles(GeomHdr **a_poly, int a_sz);//dbg
static int		GetPointParam(GeomHdr *poly, double &x, double &y, long id, double &t);
static int		SetPointParam(GeomHdr *poly, double t, double &x, double &y, long &id);
static void		MoveToHead(GeomHdr *fromPoly, GeomHdr *toPoly);
static int		GetVertPnt(GeomHdr *a, GL_Point &cent, double &y1, double &y2);
static int		GetProjParam(GeomHdr *a, double x0, double y0, double &x1, double &y1, double &t);
//static GeomHdr	*SGU::GetPath(GL_Point *beg, GL_Point *end, GeomHdr *poly1, GeomHdr *poly2, int i1 = 1, int i2 = 1);
static GeomHdr *DelPolyLoops(GeomHdr *a);
static int	IsCrossPolyPoly(GeomHdr *a, GeomHdr *b, int check_inside = 0);
static int	IsCrossCirPoly(GL_Point *cent, long rad, GeomHdr *a);
static int	IsCrossSegPoly(GL_Point *p1, GL_Point *p2, GeomHdr *a);
static int	IsCrossArcPoly(GL_Point *p1, GL_Point *p2, GL_Point *cent, long rad, GeomHdr *a);

};

// Debug functions
extern void dp( const GL_Point &Ori);
extern void dp( const GL_Point *Ori);
extern void dp( double x, double y);
extern void dp( const GL_Point &Ori, GL_Transform &lc);
extern void dp( double x, double y, GL_Transform &lc);
extern void dp( const GeomHdr *gh, int i);

//~~*~~~~~~~~~~~~~PolySec ~~~~~~~~~~~~~~~~~~~~~
class PolySec
{
public:
	PolySec();
	void InitPolySec(double a, double b, GeomHdr *h, PolySec *next);
	int	IsOvl(PolySec *ps, double &a, double &b, double eps = 0.0);
	int CmpMerge(PolySec *ps, double &a, double &b, double eps = 0.0);
	int DumpPolySec(char *fname, char *mode);
	int GetSecAngle(GL_Point &ori, double &f1, double &fa);
public:
	PolySec *next_psec;
	double t1, t2;
	GeomHdr *poly_ps;
	void *thr_ps;
};
inline int CmpPolySec(PolySec *a, PolySec *b)
{
	if (a->poly_ps < b->poly_ps) return -1;
	if (a->poly_ps > b->poly_ps) return 1;
	if (a->t1 < b->t1) return -1;
	if (a->t1 > b->t1) return 1;
	if (a->t2 < b->t2) return -1;
	if (a->t2 > b->t2) return 1;
	return 0;
}

//~~*~~~~~~~~~~~~ Utilities ~~~~~~~~~~~~~~~~~~~~~

#endif // __SGU_H__

//
// End of file
//

