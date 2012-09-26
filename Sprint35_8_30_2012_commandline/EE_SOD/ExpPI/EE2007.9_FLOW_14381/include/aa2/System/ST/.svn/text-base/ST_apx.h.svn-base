/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
#ifndef __SAS_APX_H__
#define __SAS_APX_H__

//#include "ST_temp.h"

class stHatch;

class ArcAproximation
{
public:
	ArcAproximation() {};
	~ArcAproximation(){};
	virtual void Init(const long eps = 38100, const int max = 0) {};
	virtual void Init(const GL_Point &s, const GL_Point &e, const GL_Point &cent,	const long rad){};
	virtual int GetNext(GL_Point &p)	{return 0;}
};

class ArcApx1 : public ArcAproximation
{
public:
	ArcApx1();
	~ArcApx1();

	virtual void Init(const long eps = 38100, const int max = 0);
	virtual void Init(const GL_Point &s, const GL_Point &e, const GL_Point &cent,
												const long rad);
	virtual int GetNext(GL_Point &p);
	GL_Point a_c, a_e;
	double cs, sn, x1, y1, x2, y2, deps;
	int k;
};

///////////////////////////////////////////////
class ArcApx2 : public ArcAproximation
{
public:
	ArcApx2();
	~ArcApx2();

	void Clear();
	virtual void Init(const long eps = 38100, const int max = 0);
	virtual void Init(const GL_Point &s, const GL_Point &e, const GL_Point &cent,
												const long rad);
	virtual inline int GetNext(GL_Point &p);
	GL_Point a_c;
	long a_rx;
	long a_ry;
	int a_s;
	int a_e;
	int a_i;
	double delta_y;
};
/////////////////////////////////////////////////
/////////////////////////////////////////////////
class BarIter: public IterPolyGeom
{
public:	// data
	////////////// Seg Info
	GL_Point	beg, end, cen;
	long	rad, ra;
	int			dir1, dir2, dir3;
	////////////// Bar Info
	long	xl, xr;
	////////////// Transform Info
	double x0, x1, x2;
	double y0, y1, y2;
	double t, d;
	////////////// Status Info
	int	is_out_pnt;
	int	is_in_pnt;
	int	first_in;
	long r_safe;
	int	is_closed;	// is true for closed polyline (it will be used by child classes)
	int	need_closing_seg;	// start point is outside the bar for closed polyline
	GL_Point enter_pnt;	// point: first enter to bar from outside
	int	need_first_enter_pnt; // is true until first enter point is stored
public:	////////////// Methods
	BarIter();
	int				GetFirst ();
	inline int		GetNext ();
	void			SetRegion (GL_Box &box);
	void			SetRegion (long a_xmin, long a_xmax);
	void			SetTransform (stHatch *hh);
	inline void		Transform(GL_Point *a, GL_Point *b);

//private:
	inline void	GetEndPoint();
	inline void	GetArc();
	void	Refresh();
	int	NextSeg();
	inline int	CheckVert(long xx, int dir);
	int	CheckSeg();
	int	CheckIn();
	inline void	GetY(GL_Point &pnt, int dir);
	inline long GetDy(long x);
// debugging
	int	First1();
	int OutSrcPoly(char *file_name, char *mode);
	int OutClpPoly(char *file_name, char *mode);
};

inline void BarIter::Transform(GL_Point *a, GL_Point *b)
{
	t = x0 + x1*b->x + x2*b->y;
	d = y0 + y1*b->x + y2*b->y;
	a->x = LROUND(t);
	a->y = LROUND(d);
}
inline void BarIter::GetEndPoint()
{
	end = IterPolyGeom::GetPoint();
	Transform(&end, &end);
}
// calculate cen, rad, ra
inline void BarIter::GetArc()
{
	if (IterPolyGeom::GetArc(&cen, &rad)) {
		Transform(&cen, &cen);
		ra = ABS(rad);
	} else {
		rad = ra = 0;
	}
}
// calculate beg, end, rad (cen)
inline int BarIter::NextSeg()
{
	GetArc();
	if (IterPolyGeom::GetNext()) {
		beg = end;
		GetEndPoint();
		return 1;
	}
	return 0;
}
inline long BarIter::GetDy(long x)
{
	t = (double)(x - cen.x);
	d = (double)rad;
	d = gl_sqrt(d*d - t*t);
	return LROUND(d);
}

inline int BarIter::GetNext()
{
	if (is_out_pnt) {	// end point is outside
		if (!CheckIn()) {
			if (need_closing_seg) {
				if (!need_first_enter_pnt) { // there was intersection with clipping bar
				// set data for last closing segment
					end = enter_pnt;
					rad = 0;
					return 1;
				}
			// there were NO intersections with clipping bar
			}
			return 0;
		}
		if (need_first_enter_pnt) {
			enter_pnt = beg;	// store first enter point that will be nesessary data for last closing segment
			need_first_enter_pnt = 0;	// there was first intersection point with clipping bar
		}
		if (beg.y != end.y) return 1;	// vertical segment {beg, end}
	}
	if (is_in_pnt) {
		is_in_pnt = 0;
		beg = end;
		rad = r_safe;
		//////////// Refresh();
		if (IterPolyGeom::IsFirst()) {
			NextSeg();
		} else {
			GetEndPoint();
			ra = ABS(rad);
		}
	} else if (!NextSeg()) {	// end point is in bar
		return 0;
	}
	CheckSeg();
	return 1;
}

//============================================================================
//============================= RectIter =====================================
//============================================================================

/////////////////////////////////////////////////////////////////////
// ArcYDir - calculate  y-direction of arc in the given point
/////////////////////////////////////////////////////////////////////
inline int ArcYDir
(
	const GL_Point	*p,	// point in question
	const GL_Point	*c,	// center
	const long	r		// radius
)
{
	if(p->x < c->x){
		return (r > 0)? -1 : 1;
	}else if(p->x > c->x){
		return (r < 0)? -1 : 1;
	}else{
		return (p->y < c->y)? 1 : -1;
	}
}

/////////////////////////////////////////////////////////////////////
// ArcYDirs - calculate  y-directions of arc in the given point
/////////////////////////////////////////////////////////////////////
inline void ArcYDirs
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
	dir1 = ArcYDir(s, c, r);
	dir2 = ArcYDir(e, c, r);
	if (dir1 == dir2) {
		if((e->y > s->y) == (dir1 > 0)) {
			dir2 = 0;
			dir3 = 0;
		} else {
			dir2 = -dir1;
			if (e->x == c->x) {
				dir3 = 0;
			} else {
				dir3 = dir1;
			}
		}
	} else {
		dir3 = 0;
		if (e->x == c->x) {
			dir2 = 0;
		}
	}
}


class RectIter: public BarIter
{
public:	// data
	////////////// Seg Info
	GL_Point	beg, end;
	////////////// Bar Info
	long	yb, yt;
	////////////// Status Info
	int	is_out_pnt;
	int	is_in_pnt;
	long r_safe;
	int	seg_input_count;
	int	need_closing_seg;	// start point is outside the bar for closed polyline
	GL_Point enter_pnt;	// point: first enter to bar from outside
	int	need_first_enter_pnt; // is true until first enter point is stored
public:	////////////// Methods
	RectIter();
	int			GetFirst ();
	int			GetNext ();
	void			SetRegion (GL_Box &box);
	void			SetRegion (long a_xmin, long a_xmax, long a_ymin, long a_ymax);

//private:
	inline void	GetEndPoint();
	inline int	NextSeg();
	int	CheckSeg();
	int	CheckIn();
	inline long GetDx(long y);
};

inline void RectIter::GetEndPoint()
{
	end = BarIter::end;
}

// calculate beg, end, rad (cen)
inline int RectIter::NextSeg()
{
	if (seg_input_count ++) {
		if (!BarIter::GetNext()) {
			return 0;
		}
	}
	beg = end;
	GetEndPoint();
	return 1;
}

inline long RectIter::GetDx(long y)
{
	t = (double)(y - cen.y);
	d = (double)rad;
	d = gl_sqrt(d*d - t*t);
	return LROUND(d);
}


#endif	//__SAS_APX_H__

