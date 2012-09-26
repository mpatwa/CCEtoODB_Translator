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

// Name:				ST_bars.cpp
// Date:				02.25.98
// Author:			Alexander Zabolotnikov	
// Description:	bars processing

#ifndef __SAS_BARS_H__
#define __SAS_BARS_H__

#include "ST_xitm.h"

#define NEA_EPS 6
#define MAX_MAIN_SCAN 64

class LocOrds
{
public:
	int ky;
	int my;
	short *ords;
};

class SYSGL_API ScnOrds
{
public:
	int num_scan;
	LocOrds lo[MAX_MAIN_SCAN];
	ScnOrds();
	~ScnOrds();
	void	FreeArr();
	short	*GetOrdPtr( int len, int id);
	short	*NewOrdPtr( int len, int *id);
};
extern ScnOrds SYSGL_API st_loc_ord;

class VertEvent
{
public:
	GL_Point pnt;
	GeomHdr *gh;	//for testing
	XEvent *xi;
};

class BarItem
{
public:
	unsigned int flag;
	int	ky;
};

class SYSGL_API BarHdr
{
public:
	int			num_bars, del_idx, ins_idx, max_ky;
	int			clear_flag;
	int			kvy, max_kvy;
	BarItem		*first_bar;
	BarItem		*last_bar;
	VertEvent	*ve;
	double		sn, cs;
	long	dy, min_val, max_val;
	stScan	*scan1;
	stScan	*scan2;
	/////////////////
	XEvent		*x1, *x2, *first0, *last0, *first1, *last1, *first2, *last2;
	GL_Point	p1, p2;
	int			id1, id2;
	unsigned int flag1, flag2;
	GL_Transform		lc;

	BarHdr();
	~BarHdr();
	int	Init();
	int	SetBar(double csn, double sns, long dy, GL_Point &p0, int num);
	int	SetMainScan(stScan *scan);
	int	SetLocalScan(stScan *scan);
	void	SetBarFlags(int bar_id1, int bar_id2, unsigned int scan_flag);
	void	SetBarFlag(int bar_id, unsigned int scan_flag);
	int	SetLocalOrds(stScan *scan);
	int	GeomInBar(GeomHdr *gh, unsigned int flag);

	int	BarsJoint();

	int	GetNext2( XEvent **xi, int &bar_id, unsigned int &scan_flag);
	XEvent	*GetNext1(XEvent **xi, XEvent *last_xi, int &idx, GL_Point &px);

	void	WriteXi(XEvent *xi);
	void	WriteVericalBars();
	void	WriteToBuff(XEvent *xi, GL_Point &p1, int buf_id);
	void	WriteBuf();
};
inline int GetBarIdx(XEvent *xi)
{
	if (xi && xi->node) {
		return (int)(xi->node->prev_geom);
	}
	return -1;
}


inline int ToTheLeft(XEvent *xi, long xr)
{
	return (((GL_Point *)((PolyPoint *)(xi->node + 1)))->x < xr);
}

inline int CloseToGrid
(
	const long x0,
	const long dx,
	const long x,
	long &xx
)
{
	// 10/09/01 DmitryV. Temp fix for grid hatch. To be reviewed by AlexZ.
	if (dx == 0) {
		return 0;
	}

	if (x > x0) {
		xx = (x-x0) % dx;
		if (xx < NEA_EPS) {
			xx = x - xx;
			return 1;
		} else if (dx - xx < NEA_EPS) {
			xx = x + dx - xx;
			return 1;
		}
	} else {
		xx = (x0-x) % dx;
		if (xx < NEA_EPS) {
			xx = x + xx;
			return 1;
		} else if (dx - xx < NEA_EPS) {
			xx = x + xx - dx;
			return 1;
		}
	}
	return 0;
}
inline int NeaToGrid0(GL_Point *p0, long dy, double &cs, double &sn, GL_Point *pt)
{
	long yy;
	if (sn == 0.0) {	// hor
		if (CloseToGrid(p0->y, dy, pt->y,yy)) {
			pt->y = yy;
			return 1;
		}
	} else if (cs == 0.0) {	// vert
		if (CloseToGrid(p0->x, dy, pt->x, yy)) {
			pt->x = yy;
			return 1;
		}
	} else {
	}
	return 0;
}
inline int PrjToGrid
(
	const long x0,
	const long dx,
	const long x,
	const int dir,
	long &x2
)
{
	long xx = x - (x-x0) % dx;
	if (x > x0) {
		if (dir > 0) {
			xx += dx;
		}
	} else {
		if (dir < 0) {
			xx -= dx;
		}
	}
	if (dir > 0 && xx < x2 || dir < 0 && xx > x2) {
		x2 = xx;
		return 1;
	}
	return 0;
}



extern int SYSGL_API GetRangeInDir( GeomHdr *a_gh, GL_Transform &a_lc, GL_Point &a_p1, GL_Point &a_p2, long *a_id1=NULL, long *a_id2=NULL);
extern int SYSGL_API GetRangeInDir(GeomHdr *gh, GL_Transform &lc, long &mi, long &ma);
extern int SYSGL_API GetRangeInDir ( GeomHdr *gh, GL_Transform &lc, long &ymin, long &ymax, bool isVertLay);
extern int SYSGL_API GetBarsRange(GeomHdr *g, GL_Transform &l, long y, long	dy, int &i_mi,	int &i_ma);
extern int SYSGL_API GetRangeID(GeomHdr *gh, GL_Transform &lc, long y0, long dy);
extern int CloseToGrid(const long x0, const long dx, const long x, long &xx);
extern int VertRange(GeomHdr		*gh, GL_Transform	&lc, long		dy);

#endif	// __SAS_BARS_H__

