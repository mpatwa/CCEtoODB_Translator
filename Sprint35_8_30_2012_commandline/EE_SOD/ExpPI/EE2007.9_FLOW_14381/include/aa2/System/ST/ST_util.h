/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// This is a part of the PowerSYS System C++ library.
// Copyright (C) 1996 PADS Software Inc.
// All rights reserved.

// Name:				ST_util.h
// Date:				10.20.86
// Author:			Alexander Zabolotnikov	
// Description:	scanner & geometry utility

#ifndef __SAS_UTIL_H__
#define __SAS_UTIL_H__

class IterGeom;
class SAS_Seg;
class XEvent;
class stScan;
class AreaTreeNode;
/////////////////// SAS_chain functions //////////////////////////////////////
extern int	SegOrdX		(SAS_Seg &dns, SAS_Seg &ups);
extern int	NextTwoSeg	(SAS_Seg &dns, SAS_Seg &ups);
extern int	CrossFind	(SAS_Seg &dns, SAS_Seg &ups, GL_Point *cross_arr, const int cross_num = 1);
extern int	CrossSegSeg (SAS_Seg &dns, SAS_Seg &ups, GL_Point *p, int &ind, int max_p = 1);
extern int	CrossSegSeg (SAS_Seg &dns, SAS_Seg &ups, GL_Point *pt, GL_Point *p, int &ind, int max_p);
extern int	CrossSegCir (SAS_Seg &dns, SAS_Seg &ups, int ord, GL_Point *p, int &ind, int max_p = 1);
extern int	GetOrder		(SAS_Seg &sn, SAS_Seg &st);
extern int	GetOrderAlg	(SAS_Seg	&sn, SAS_Seg &st);
extern int	GetOrder		(const GL_Point &p, SAS_Seg &seg);
extern int	GetOrder0	(const GL_Point &p, const SAS_Seg &seg);
extern int	GetOrder0	(const SAS_Seg &sn,  const SAS_Seg &st,  const GL_Point &p);
extern int	PntSegPosition( const GL_Point *p,  const SAS_Seg *la);
extern int	GetOrder1	(SAS_Seg &sn,  SAS_Seg &st,  GL_Point &p);	// sas_int.cpp
extern int	CompareMono	(GeomHdr *g1, GeomHdr *g2);
/////////////////// chain functions /////////////////////////////////////
extern int  CrossFind   (const IterGeom *crn1, const IterGeom  *crn2,
									GL_Point *p, const int cross_num);
extern int  GetOrder    (const IterGeom *crn1, const IterGeom  *crn2);
extern int  GetOrder    (const GL_Point &p, const IterGeom *crn);
extern int 	GetOrder(const XEvent *itm1,  const XEvent *itm2);
extern int  GetLeftPntOrder( const XEvent *itm1,  const XEvent *itm2);
extern int	GetAreaId2(const long flag, const int dir = 1);
/////////////////// segment functions /////////////////////////////////////
// moved to sas_low
int GetOrder(const GL_Point &s1,const GL_Point &e1,const GL_Point &c1,long r1,
				  const GL_Point &s2,const GL_Point &e2,const GL_Point &c2,long r2);
/////////////////// point functions /////////////////////////////////////
extern void SYSGL_API GetYRange(const IterPolyGeom *crn, long &ymin, long &ymax);
extern void GetYRange(const GL_Point *cent, const long rad,
					long &ymin, long &ymax, const long eps = 0);
extern void GetYRange(const GL_Point *beg, const GL_Point *end,
					long &ymin, long &ymax, const long eps = 0);
extern void GetYRange(const GL_Point *beg, const GL_Point *end, const GL_Point *cent,
							 const long rad, long &ymin, long &ymax, const long eps = 0);
extern int CheckDirSeg(const long &s1, const long &e1,
							  const long &s2, const long &e2, int &i1, int &i2);
extern void CheckPoly(const IterGeom *crn1);
extern int RemoveSpoke(GeomHdr	*gh, int	flag);
extern int PntOnVertPoly(XEvent	*xi, GL_Point	*pnt);
extern int RotatePoly0(GeomHdr *gh,const double &cs,const double &sn,const int flag = 0);
extern int AdjustArcs( GeomHdr *gh, char *mess = NULL);
//extern int IntCirCir(SAS_Seg	*arc1, SAS_Seg	*arc2);
//extern int IntCirLin(SAS_Seg	*cir, SAS_Seg	*lin);

/////////////////// debugging /////////////////////////////////////
GeomHdr SYSGL_API *glInPoly(char *fil_name, int &num_poly);
int SYSGL_API F_Poly(GeomHdr  *gh, char *out_file, char *mode = "w");
GeomHdr SYSGL_API *npoly(int &num);
void SYSGL_API F_Box(GL_Point &ll, GL_Point &ur, char *file_name, char *mode);
void SYSGL_API F_Iter(GL_Array &arr, GL_Box *b, char *fname, int i0 = -1, int num = -1);

extern int	F_Scan(stScan *scan, char *file_name, char *mode = "w");
extern void F_Xarr(unsigned long	fl, XEvent *x0,	XEvent	*xl, char *file_name, char *mode, char *pref = NULL);
extern int	F_Tree(AreaTreeNode *node, int	level, char *file_name, char *mode);
extern FILE *F_Open(char *file_name, char *mode);

extern int CheckGeomHdr(GeomHdr	*gh, char *flags);
extern int CheckDataHdrLink(GeomHdr *hdr, unsigned long flag);
extern int CheckScanHdr(stScan *scan);

/////////////////////////////////////////////////////////////////////
//		SnapPnt - pt snaps p if p is near to pt
//$$ Return Value:
//		1 - if  pt snaps p
//		0 - else
/////////////////////////////////////////////////////////////////////
inline int SnapPnt
(
	const	GL_Point	*pt,	//  solid point
	GL_Point			*p		//  checked point
)
{
	if (p->x >= pt->x - LEPS && p->y >= pt->y - LEPS &&
		 p->x <= pt->x + LEPS && p->y <= pt->y + LEPS ) {
		p->x = pt->x;
      p->y = pt->y;
		return 1;
   }
   return 0;
}

/////////////////////////////////////////////////////////////////////
//		SnapPnt - pt snaps p if p is near to pt
//$$ Return Value:
//		1 - if  pt snaps p
//		0 - else
/////////////////////////////////////////////////////////////////////
inline int SnapPnt
(
	const GL_Point	*pt,	// solid point
	GL_Point			*p,	// checked point
	const long	eps	// the distance for snapping
)
{
	if (p->x >= pt->x - eps && p->y >= pt->y - eps &&
		 p->x <= pt->x + eps && p->y <= pt->y + eps ) {
		p->x = pt->x;
      p->y = pt->y;
		return 1;
   }
   return 0;
}

/////////////////////////////////////////////////////////////////////
//		PntCmp - point compare
//$$ Return Value:
//		point order {-1, 0, 1}
/////////////////////////////////////////////////////////////////////
inline int PntCmp
(
	const GL_Point	*u,	// first point
	const GL_Point	*v		// second point
)
{
	if		  (u->x < v->x) return -1;
	else if (u->x > v->x) return +1;
	else if (u->y < v->y) return -1;
	else if (u->y > v->y) return +1;
	return  (0);
}

/////////////////////////////////////////////////////////////////////
//		PntCmp - point compare
//$$ Return Value:
//		point order {-1, 0, 1}
/////////////////////////////////////////////////////////////////////
inline int PntCmp
(
	const GL_Point	*u,	// first point
	const GL_Point	*v,	// second point
	const long	eps	// precision
)
{
	if		  (u->x < v->x - eps) return -1;
	else if (u->x > v->x + eps) return +1;
	else if (u->y < v->y - eps) return -1;
	else if (u->y > v->y + eps) return +1;
	return  (0);
}

/////////////////////////////////////////////////////////////////////
//		PntCmp - point compare
//$$ Return Value:
//		point order {-1, 0, 1}
/////////////////////////////////////////////////////////////////////
inline int PntCmp
(
	const GL_Point	&u,	// first point
	const GL_Point	&v		// second point
)
{
	if		  (u.x < v.x) return -1;
	else if (u.x > v.x) return +1;
	else if (u.y < v.y) return -1;
	else if (u.y > v.y) return +1;
	return  (0);
}

/////////////////////////////////////////////////////////////////////
//		PntCmp - point compare
//$$ Return Value:
//		point order {-1, 0, 1}
/////////////////////////////////////////////////////////////////////
inline int PntCmp
(
	const GL_Point	&u,	// first point
	const GL_Point	&v,	// second point
	const long	eps	// precision
)
{
	if		  (u.x < v.x - eps) return -1;
	else if (u.x > v.x + eps) return +1;
	else if (u.y < v.y - eps) return -1;
	else if (u.y > v.y + eps) return +1;
	return  (0);
}

inline int	PntEqu
(
	const GL_Point &u,
	const GL_Point &v
)
{
	return (u.x == v.x && u.y == v.y);
}
inline int	PntEqu
(
	const GL_Point *u,
	const GL_Point *v
)
{
	return (u && v && u->x == v->x && u->y == v->y);
}

/////////////////////////////////////////////////////////////////////
//		PntCmp - point compare
//$$ Return Value:
//		point order {-1, 0, 1}
/////////////////////////////////////////////////////////////////////
inline int DPntCmp
(
	const GL_DPoint	&u,
	const GL_DPoint	&v
)
{
	if		(u.x < v.x) return -1;
	else if (u.x > v.x) return +1;
	else if (u.y < v.y) return -1;
	else if (u.y > v.y) return +1;
	return  (0);
}

/////////////////////////////////////////////////////////////////////
// ArcXDir - calculate  x-direction of arc in the given point
/////////////////////////////////////////////////////////////////////
inline int ArcXDir
(
	const GL_Point	*p,	// point in question
	const GL_Point	*c,	// center
	const long	r		// radius
)
{
	if(p->y < c->y){
		return (r > 0)? 1 : -1;
	}else if(p->y > c->y){
		return (r < 0)? 1 : -1;
	}else{
		return (p->x < c->x)? 1 : -1;
	}
}

/////////////////////////////////////////////////////////////////////
// ArcXDirs - calculate  x-directions of arc in the given point
/////////////////////////////////////////////////////////////////////
inline void ArcXDirs
(
	const GL_Point	*s,	// start
	const GL_Point	*e,	// end
	const GL_Point	*c,	// center
	const long	r,		// radius
	int &dir1,					// direction from first point
	int &dir2,					// next direction
	int &dir3					// next direction
)
{
	dir1 = ArcXDir(s, c, r);
	dir2 = ArcXDir(e, c, r);
	if (dir1 == dir2) {
		if((e->x > s->x) == (dir1 > 0)) {
			dir2 = 0;
			dir3 = 0;
		} else {
			dir2 = -dir1;
			if (e->y == c->y) {
				dir3 = 0;
			} else {
				dir3 = dir1;
			}
		}
	} else {
		dir3 = 0;
		if (e->y == c->y) {
			dir2 = 0;
		}
	}
}

const unsigned long DEL_SAS_FLAGS	= (GL_SAS_CLEAR & (~GL_SAS_MARK));

//-----------------------
inline unsigned long IsDataFlag(GeomHdr *d, unsigned long flg) 
{
	return (d->flags & flg);
}
inline unsigned long GetDataFlag(GeomHdr *d) 
{
	return d->flags;
}
inline void SetDataFlag(GeomHdr *d, unsigned long flg) 
{
	d->flags |= flg;
}
inline void DelDataFlag(GeomHdr *d, unsigned long flg)
{
	d->flags &= (~flg);
}
inline void DelSASFlags(GeomHdr *d)
{
	d->flags &= DEL_SAS_FLAGS;
}
inline int IsSDBDeleteFlag(GeomHdr *d)
{
	return (d->flags & GL_SDB_DELETE)? 1 : 0;
}
//~~*~~~~~~~~~~~ hdr_flags ~~~~~~~~~~~~~~~~~
inline unsigned long IsHdrFlag(GeomHdr *d, unsigned long flg)
{
	return (d->hdr_flags & flg);
}
inline void SetHdrFlag(GeomHdr *d, unsigned long flg) 
{
	d->hdr_flags |= flg;
}
inline void DelHdrFlag(GeomHdr *d, unsigned long flg)
{
	d->hdr_flags &= (~flg);
}

inline int IncludeFlags(unsigned long a, unsigned long b)
{
	return ((a & b) == b)? 1 : 0;
}
#endif

