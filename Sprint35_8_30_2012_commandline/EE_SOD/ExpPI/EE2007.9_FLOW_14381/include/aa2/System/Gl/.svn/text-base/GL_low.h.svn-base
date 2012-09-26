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

// Name:				GL_low.h
// Date:				10.20.86
// Author:			Alexander Zabolotnikov	
// Description:	Scanner low crossing


#ifndef __SAS_LOW_H__
#define __SAS_LOW_H__


inline void NormGeomVect(GL_DPoint &v, double &len)
{
	if (v.x == 0.0) {
		if (v.y >= 0.0)	{len = v.y; v.y = 1;}
		else			{len = -v.y; v.y = -1;}
	} else if (v.y == 0.0) {
		if (v.x >= 0.0)	{len = v.x; v.x = 1;}
		else			{len = -v.x; v.x = -1;}
	} else {
		len = gl_sqrt(v.x*v.x + v.y*v.y);
		v.x /= len;
		v.y /= len;
	}
}

/*-----------------
	x*x + y*y - r*r = 0
	a*x + b*y + c   = 0
-----------------*/
class SYSGL_API glLow
{
public:
	glLow() { eps = 0.00001; ieps = 2; cross_count = 0;}

	void SetEps (const double deps) { eps =  deps;}
	void SetEps (const long ceps) { ieps =  ceps;}
	int PntPosition (const GL_Point *p, const GL_Point *s, const GL_Point *e);
	int PntRayPosition(const GL_Point *p,const GL_Point *s,const GL_Point *e);
	int PntPosition (const GL_Point *p, const GL_Point *s, const GL_Point *e,
		const GL_Point *c, const long r);
	int PntPositionD (const GL_Point *p, const GL_Point *s, const GL_Point *e,
		const GL_DPoint *c, const long r);
	int PntPosition0(const GL_Point *p, const GL_Point *s, const GL_Point *e);
	int PntPosition0(const GL_Point *p, const GL_Point *s, const GL_Point *e,
		const GL_Point *c, const long r);
	int PntPosition0(const GL_Point *p, const GL_Point *c, const long r);

	GL_Point Projection(const GL_Point *p, const GL_Point *s, const GL_Point *e);
	GL_Point Projection(const GL_Point *p, const GL_Point *c, const long rad);
	GL_DPoint ProjectionD(const GL_DPoint *p, const GL_DPoint *c, const long rad);
	GL_DPoint ProjectionD(const GL_DPoint *p, const GL_DPoint *s, const GL_DPoint *e);

	int IntPntLin (const GL_Point *p, const GL_Point *s, const GL_Point *e);
	int IntPntCir (const GL_Point *p, const GL_Point *c, const long rad);
	int IntPntSeg (const GL_Point *p, const GL_Point *s, const GL_Point *e);
	int IntPntArc (const GL_Point *p, const GL_Point *s, const GL_Point *e,
		const GL_Point *c, const long rad);

	int IntCirCir (const GL_Point *cen1, const long,
						const GL_Point *cen2, const long,
						const int k = 2, GL_Point *ip = NULL);
	int IntCirLin (const GL_Point *cen, const long rad,
						const GL_Point *beg, const GL_Point *end,
						const int k = 2, GL_Point *ip = NULL);
	int IntdCirLin (const GL_DPoint *cen, const long rad,
						const GL_Point *beg, const GL_Point *end,
						const int k = 2, GL_Point *ip = NULL);
	int IntdCirdLin(double cenx, double ceny, double rad, double x1, double y1, double x2, double y2);
	int IntdCirdVert(double cenx, double ceny, double rad, double x1);
	int IntdCirdHor(double cenx, double ceny, double rad, double y1);
	int IntLinLin (const GL_Point *s1, const GL_Point *e1,
                  const GL_Point *s2, const GL_Point *e2,
						GL_Point *ip = NULL);


	int IntArcArc (const GL_Point *s1, const GL_Point *e1, const GL_Point *c1, const long r1,
						const GL_Point *s2, const GL_Point *e2, const GL_Point *c2, const long r2,
						const int k = 2, GL_Point *ip = NULL);
	int IntArcSeg (const GL_Point *s, const GL_Point *e, const GL_Point *cen, const long rad,
						const GL_Point *beg, const GL_Point *end,
						const int k = 2, GL_Point *ip = NULL);
	int IntdArcSeg (const GL_Point *s, const GL_Point *e, const GL_DPoint *cen, const long rad,
						const GL_Point *beg, const GL_Point *end,
						const int k = 2, GL_Point *ip = NULL);
	int Intd2ArcSeg(const GL_DPoint *s, const GL_DPoint *e, const GL_DPoint *cen, const long rad, const GL_Point *beg, const GL_Point *end);

	int IntSegSeg (const GL_Point *s1, const GL_Point *e1,
						const GL_Point *s2, const GL_Point *e2,	GL_Point *ip = NULL);
	int IntRayRay (const GL_Point *s1, const GL_Point *e1,
						const GL_Point *s2, const GL_Point *e2,	GL_Point *ip = NULL);
	short IntLinLin (double &x, double &y, double a1, double b1, double c1, double a2, double b2, double c2);
	short IntLinCir (double &x1, double &y1, double &x2, double &y2, double a, double b, double c, double xc, double yc, double r);
	short IntCirCir (double &x1, double &y1, double &x2, double &y2, double a1, double b1, double r1, double a2, double b2, double r2);
//	int IntCirCir (SAS_Seg *arc1, SAS_Seg *arc2); // To Be Implemented
//	int IntCirLin (SAS_Seg *arc,  SAS_Seg *seg); // To Be Implemented
						
	int GetAngles (double  &f0,  double &fa, const GL_Point *cen, const long rad,
						const GL_Point *s, const GL_Point *e);

	double GetLength(const GL_Point *s, const GL_Point *e);
	double GetLength(const GL_Point *s, const GL_Point *e, const GL_Point *c, const long rad);
	void	GetOrthoLine (double x1, double y1, double A, double B);
	void	Normalize(double *A, double *B, double *C);


	int GetCrossPnt ();
	void GetTangent(const GL_Circle &c1, const GL_Circle &c2, GL_Point *p1, GL_Point *p2);
	void ShiftLine(const GL_Point &c1,const GL_Point &c2, double w);
	void GetLine (const GL_Point &a_p1,const GL_Point &a_p2,double *a_a, double *a_b, double *a_c) const;
	int GetCenterLine(const GL_Point &a_p1, const GL_Point &a_p2, double *a_a, double *a_b, double *a_c);   
	int GetArc(const GL_Point &s_p, const GL_Point &e_p, const GL_Point &i_p, GL_Point *c_p, long *r);   
	int CenterByRadius(GL_Point *s, GL_Point *e, int arc, long &rad, GL_Point &cent);
	int AdjustCentRad (GL_Point *s, GL_Point *e, long	&rad, GL_Point &cent);
	int AdjustStartEnd(long rad, GL_Point *cent, GL_Point &s, GL_Point &e);
	
	static void		GetBox(const GL_Point *s, const GL_Point *e, GL_Box &box);
	static void		GetBox(const GL_Point *s, const GL_Point *e, const GL_Point *c, long rad, GL_Box &box);
	static void		GetBoxByCentLine(const GL_Point	*s, const GL_Point	*e, double w, GL_Point *pt);
	static double	sas_atan2(double dy, double dx);
//	static GeomHdr *MergeTwoPolylines(GeomHdr *g1, GeomHdr *g2, int dir2 = 1); // To Be Implemented
	static void		GetTngDir( const GL_Point &p,
					const GL_Point &s,	const GL_Point &e, const GL_Point &c, long	r,
					double *diff1,	double *diff2);
	static void		GetTngVec( const GL_Point &p, const GL_Point &s, const GL_Point &e, const GL_Point &c, long	r,
					GL_DPoint &v);
	static int		GetOrder0(const GL_Point &p,
					const GL_Point &s, const GL_Point	&e, const GL_Point	&c, long	r);
	static int		GetOrder0(const GL_Point &s1, const GL_Point &e1, const GL_Point	&c1, long r1,
					const GL_Point &s2, const GL_Point &e2, const GL_Point	&c2, long r2, const GL_Point &p);
	static int		GetOrder(const GL_Point &s1,const GL_Point &e1,const GL_Point &c1,long r1,
					const GL_Point &s2,const GL_Point &e2,const GL_Point &c2,long r2);
	static int		GetMonoPiece(const GL_Point *f, const GL_Point *s, const GL_Point *c, const long r, GL_Point &pt);

	void GetTangentD(const GL_DPoint &c1, double r1, const GL_DPoint &c2, double r2, GL_Point *p1, GL_Point *p2);
	void ShiftLineD(const GL_DPoint &c1,const GL_DPoint &c2, double w);
public:
	long ieps;
	double eps;
	double xc;
	double yc;
	double a;
	double b;
	double c;
	double r;
	double x[2];
	double y[2];
	GL_DPoint dp;
	GL_Point p_int[2];
	GL_Point *v;
	int cross_count;
	int rc;
};
/////////////////////////////////////////////////////////////////////
// scal_p -scalar product
/////////////////////////////////////////////////////////////////////
inline double scal_p
(
	const GL_Point	*a,	// start of first vector
	const GL_Point	*b,	// end of first vector
	const GL_Point	*c,	// start of second vector
	const GL_Point	*d		// end of second vector
)
{
	return (double)(b->x - a->x) * (double)(d->x - c->x)
		  + (double)(b->y - a->y) * (double)(d->y - c->y);
}
inline double scal_dp
(
	const GL_DPoint	*a,	// start of first vector
	const GL_DPoint	*b,	// end of first vector
	const GL_DPoint	*c,	// start of second vector
	const GL_DPoint	*d	// end of second vector
)
{
	return (b->x - a->x)*(d->x - c->x) + (b->y - a->y)*(d->y - c->y);
}
/////////////////////////////////////////////////////////////////////
// vert_p - vector product
/////////////////////////////////////////////////////////////////////
inline double vect_p
(
	const GL_Point	*a,	// start of first vector
	const GL_Point	*b,	// end of first vector
	const GL_Point	*c,	// start of second vector
	const GL_Point	*d		// end of second vector
)
{
	return (double)(b->x - a->x) * (double)(d->y - c->y)
		  - (double)(b->y - a->y) * (double)(d->x - c->x);
}

/////////////////////////////////////////////////////////////////////
//	GetBox - get box of segment
/////////////////////////////////////////////////////////////////////
inline void glLow::GetBox
(
	const GL_Point	*s,	// first point
	const GL_Point	*e,	// second point
	GL_Box				&box	// box
)
{
	if(s->x < e->x) {
		box.ll.x = s->x;
		box.ur.x = e->x;
	} else {
		box.ll.x = e->x;
		box.ur.x = s->x;
	}

	if(s->y < e->y) {
		box.ll.y = s->y;
		box.ur.y = e->y;
	} else {
		box.ll.y = e->y;
		box.ur.y = s->y;
	}
}

/////////////////////////////////////////////////////////////////////
// GetLength - get length of segment
/////////////////////////////////////////////////////////////////////
inline double glLow::GetLength
(
	const GL_Point	*s,	// start point
	const GL_Point	*e		// end point
)
{
	if (s->x == e->x) {
		a = (double)(e->y - s->y);
		a = ABS(a);
	} else if (s->y == e->y) {
		a = (double)(e->x - s->x);
		a = ABS(a);
	} else {
		a = (double)(e->x - s->x);
		b = (double)(e->y - s->y);
		a = gl_sqrt( a*a + b*b);
	}
	return a;
}

/////////////////////////////////////////////////////////////////////
// GetLength - get length of arc
/////////////////////////////////////////////////////////////////////
inline double glLow::GetLength
(
	const GL_Point	*s,		// arc start
	const GL_Point	*e,		// arc end
	const GL_Point	*cent,	// arc center
	const long	rad		// arc radius
)
{
	GetAngles(a, b, cent, rad, s, e);
	b = b*rad;
	return ABS(b);
}
/////////////////////////////////////////////////////////////////////
// Get a,b,c of the line's equation.
/////////////////////////////////////////////////////////////////////
inline void glLow::GetLine (const GL_Point &a_p1,const GL_Point &a_p2,double *a_a, double *a_b, double *a_c) const
{
    *a_a = a_p2.y - a_p1.y;
    *a_b = a_p1.x - a_p2.x;
    *a_c = -(*a_a)*a_p1.x - (*a_b)*a_p1.y;
}

#define VAL_IN_RNG(b, x, c)		((b) <  (x) && (x) <  (c))
#define VAL_INB_RNG(b, x, c)	((b) <= (x) && (x) <= (c))

#define RNG_IN_RNG(a, b, c, d)	((c) <  (a) && (b) <  (d))
#define RNG_INB_RNG(a, b, c, d)	((c) <= (a) && (b) <= (d))
#define RNG_OV_RNG(a, b, c, d)	((a) <= (d) && (c) <= (b))

#define PNT_IN_BOX(x, y, lx, ly, rx, ry)		(VAL_IN_RNG(lx, x, rx) && VAL_IN_RNG(ly, y, ry))
#define PNT_INB_BOX(x, y, lx, ly, rx, ry)		(VAL_INB_RNG(lx, x, rx) && VAL_INB_RNG(ly, y, ry))

#define BOX_IN_BOX(lx,ly,rx,ry,lx2,ly2,rx2,ry2) (PNT_IN_BOX(lx,ly,lx2,ly2,rx2,ry2) && PNT_IN_BOX(rx,ry,lx2,ly2,rx2,ry2))
#define BOX_INB_BOX(lx,ly,rx,ry,lx2,ly2,rx2,ry2) (PNT_INB_BOX(lx,ly,lx2,ly2,rx2,ry2) && PNT_INB_BOX(rx,ry,lx2,ly2,rx2,ry2))
#define BOX_OV_BOX(lx,ly,rx,ry,lx2,ly2,rx2,ry2) (RNG_OV_RNG(lx,rx,lx2,rx2) && RNG_OV_RNG(ly,ry,ly2,ry2)) 

#endif	// _SAS_LOW_H_

