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

// Name:				ST_vert.h
// Date:				09.30.98
// Author:			Alexander Zabolotnikov	
// Description:	hatch manager

#ifndef __SAS_VERT_H__
#define __SAS_VERT_H__

#include "ST_apx.h"

#define START_HATCH_SIZE 1000
#define DESCRIPTOR_SIZE 256		// stHatch::des[DESCRIPTOR_SIZE]
#define MAX_FOR_QSORT 256			// if (kx < MAX_FOR_QSORT) qsort()

const unsigned long GL_SAS_CUT	= 0x00020000; // GL_SAS_CROSS;
class	BarIter;
class ChainPoint;
class ChnPntHdr;

class SYSGL_API PntDir : public GL_Point
{
public:
	long dir;	// 1 for start of hathing line (down); -1 for end of hatching line (up)
};


class SYSGL_API stHatch : public GL_Array
{
public:
	stHatch(int num = 0);
	~stHatch();
	inline int			GetCount();
	inline GL_Point	*Expand(int delta_num);
	inline GL_Point	*InsItem(int ind, int delta);
	inline void			Clear();
	void			FreeBuff();

	void			StartVertHatch();
	void			EndVertHatch();
	inline void			WriteHatchPnt(long x,	long y);
	void ArcHatch(const GL_Point *f, const GL_Point	*s, const GL_Point	*c,const long r);
	void			ArcHatch(BarIter &bi);
	void			SetHatch(GL_Point *p0, long dx, double csn, double sns, int num = 0);
	void			SetHatch(GL_Point *p0, long dx, double fi = 90.0, int num = 0);
	void			SetHatch(GL_Point *p0, long dx, GL_Point *p1, int num = 0);
	void			SetBox(GL_Box *box);

	void			CreateHatch();
	void			CreateVertHatch(ChainPoint	*first, GL_Point	*h_point, long	dx);
	void			SetChainMem (glPageMem *pm_points);
	void			AddChain(ChnPntHdr	*hdr);
	void			CutChain(ChnPntHdr	*hdr);
	void			AddChain(ChainPoint	*first, unsigned long flag = 0x00000000);
	ChainPoint	*CutChain(ChainPoint	*prev, GL_Point *c_pnt, unsigned long flag = 0x00000000);
	ChainPoint	*CutChain(ChainPoint	*prev, long x, long y, unsigned long flag = 0x00000000);
	void			SeparateChain (ChnPntHdr *ch);

	void			AddGeomHatch(GeomHdr *gh);
	void			AddPolyHatch(GeomHdr *poly);
	void			AddCircleHatch(GeomHdr *circle);

	inline void			GetRotate0(GL_Point *a, GL_Point *b);
	double		dxx, dyy;					// temp data

	GL_Point	*h_p;							// first item
	GL_Point	ori;							// hatch origin
	long	x_crn, dx;					// current position
	long	Z;								// current height
	unsigned long	cut_flag;					// cutting flag
	double		fi_d, fi, sns, csn;		// direction
	int			id_crn, id_first, id_last;	// bar indexes
	int			kx;							// the number of items
	int			dir;

	glPageMem		*pm;							// pages for points
	ChnPntHdr	*first_h;					// first chain
	ChainPoint	*crn_cp;						// current point
	
	// Sorting data
	int Sort();
	GL_Point	*hp2;							// temp item
	int			kx2;
	int des[DESCRIPTOR_SIZE];
	int max_qs;									// max number to use qsort
	GL_Box *box;
	BarIter bi;

//=========================== Array of PntDir extention ===============================
	PntDir		*h_p3;						// first PntDir item
	int			kx3;							// number of PntDir items

// Methods for handling PntDir data
	inline PntDir		*Expand3(int delta_num);
	void			StartVertHatch3();
	void			EndVertHatch3();
	inline void			WriteHatchPnt3(long x, long y, long dir);
	int			Sort3();
	void			ArcHatch3(const GL_Point *f, const GL_Point *s, const GL_Point *c, const long r);
	void			AddGeomHatch3(GeomHdr *gh);
	void			AddPolyHatch3(GeomHdr *poly);
	void			AddCircleHatch3(GeomHdr *circle);

//=========================== Create Grid Matrix extention ===============================
protected:
	virtual	void	VirtWritePnt(long x, long y); // preserve existing method WriteHatchPnt()
	virtual	void	VirtWriteFirstPnt(long x, long y); // initialize buffers for previous values of indices 
	void			VirtAddArc(const GL_Point *f, const GL_Point *s, const GL_Point *c, const long r);
	void			VirtAddPoly(GeomHdr *poly);
	void			VirtAddCircle(GeomHdr *cir);
};

/////////////////////////////////////////////////////////////////////
// GetRotate0 - get rotated point
/////////////////////////////////////////////////////////////////////
inline void stHatch::GetRotate0(GL_Point *a, GL_Point *b)
{
	dxx = b->x*csn - b->y*sns;
	dyy = b->x*sns + b->y*csn;
	a->x = LROUND(dxx);
	a->y = LROUND(dyy);
}
/////////////////////////////////////////////////////////////////////
// WriteHatchPnt - add hatch point to array
/////////////////////////////////////////////////////////////////////
inline void stHatch::WriteHatchPnt(long xx, long yy)
{
	if (bi.xl < xx && xx < bi.xr) {
		if (kx >= GL_Array::GetCount()) {
			h_p = Expand(1);
			kx--;
		}
		h_p[kx].x = xx;
		h_p[kx].y = yy;
		kx++;
	}
}

/////////////////////////////////////////////////////////////////////
// GetCount - get count of items
/////////////////////////////////////////////////////////////////////
inline int stHatch::GetCount()
{
	return kx;
}

/////////////////////////////////////////////////////////////////////
// Clear - clear array of items
/////////////////////////////////////////////////////////////////////
inline void stHatch::Clear()
{
	kx = 0;
}

/////////////////////////////////////////////////////////////////////
//		Expand - Expand array for scanner  corners
//		Return Value:	pointer to array
/////////////////////////////////////////////////////////////////////
inline GL_Point *stHatch::Expand
(
	int delta	// increment
)
{
	kx += delta;
	if (kx > GL_Array::GetCount()) {
		Create(kx - GL_Array::GetCount());
		h_p = (GL_Point *)GetData();
	}
	return(h_p);
}

/////////////////////////////////////////////////////////////////////
//		InsItem - Insert items to array for scanner  corners
//		Return Value:		pointer to array
/////////////////////////////////////////////////////////////////////
inline GL_Point *stHatch::InsItem
(
	int ind,		// index for first item
	int delta	//	the number of items
)
{
	GL_Point *xd;

	xd = (GL_Point *)Insert(ind, NULL, delta);
	h_p = (GL_Point *)GetData();
	return(h_p);
}

inline int CrdCmp(long x, long y)
{
	return (x < y)? -1 : (x > y)? 1 : 0;
}

extern ChainPoint	*CutChainPoly(ChainPoint	*prev, long x, long y, glPageMem *pm, int lnk = 0);
extern int GetHatchDir(GeomHdr *h1,GeomHdr *h2,GL_Point &p,double &c,double &s,long &dx);


//============================ Grid Matrix for fillings =======================

class DrwFill;

class SYSGL_API GridMatrix : public stHatch
{
public:
	GridMatrix();
	void CreateMatrix(IterPolyCross *iter, int num_iters, DrwFill *drw_fill);
	void CreateMatrixRotated(IterPolyCross *iter, int num_iters, DrwFill *drw_fill);
	inline int GetParam(long i, long j);
	inline void SetParam(long i, long j, int value);
	inline long GetSize1() { return (N);}
	inline long GetSize2() { return (M);}
	inline void GetOrigin(GL_Point *origin) { *origin = ArrOri;}
	inline long GetStep1() { return (d_x);}
	inline long GetStep2() { return (d_y);}

	void CreateFilling(IterPolyCross *iter, int num_iters, DrwFill *drw_fill);
	void CreateFillingRotated(IterPolyCross *iter, int num_iters, DrwFill *drw_fill);
	void CreateFillingWithSubtractedArea(IterPolyCross *iter, int num_iters, DrwFill *drw_fill);
	void ClipFilling(long x1, long y1, long x2, long y2);
	void ClipHatch(long x1, long y1, long x2, long y2);
	void AddHatchPoints(const GL_Point *h_pnt, long num_pnt);

	void FreeBuff();

protected:
	virtual	void VirtWritePnt(long x, long y);
	virtual	void VirtWriteFirstPnt(long x, long y);
	void VirtAddGeom(GeomHdr *gh);
	void ItemLocalBox(const GeomHdr *poly, GL_Box *loc_box);
	void GetLocalBox(const GeomHdr *gh, GL_Box *loc_box);
	inline void WriteHatchPoint(long x, long y);

private:
	inline int ClipHatchLinesByYBar(long y_lower, long y_upper, GL_Point *clip_p);
	inline void MarkInnerCells();
	inline void CreateAreaHatch(IterPolyCross *iter, int num_iters, long h_step, bool is_rotated, GL_Array &p_arr);
	inline void CombineGridMatrixAreaWithIntactPositivePattern(GL_Point *pat_p, int pat_p_num);
	inline void CombineAreaHatchWithIntactNegativePattern(GL_Point *p, GL_Point *pat_p);
	inline void CombineAreaHatchWithPattern(GL_Point *p, GL_Point *pat_p, int fill_outside);
	inline void CombineAreaHatchWithEntirePatternHatch(GL_Point *p, GL_Point *pat_p, long x_m, int fill_outside);
	inline void CombineAreaFillingHatchWithSubtractedAreaHatch(GL_Point *p, GL_Point *pat_p, long x_m);

protected:
	GL_Point ArrOri; // Origin point for GridMatrix
	GL_Point loc_ArrOri; // Origin point for GridMatrix in local (rotated) coord. system
	GL_Box loc_box; // Clip box in local (rotated) coord. system
	long N, M; // Grid sizes
	long d_x, d_y; // Grid steps
	long x0, y0; // Grid origin point coordinates
	double cs, sn; // cos(fi), sin(fi) - fi is direction of Grid x - axis
	GL_Array param_val_arr; // Array of values for GridMatrix
	int *Arr;
	long i1, j1; // buffers for indices of previous point
	long x1, y1; // buffers for coordinates of previous point
};


class SYSGL_API VectPattern
{
public:
	int			pattern_id;    // 0 - poly list, 1 - box, 2 - ... 
	GeomHdr*	hp;
	GL_Box		pbox;
	GL_Array	hpnt;
public:
	VectPattern();
	~VectPattern();
	int CreateVectPattern(DrwFill &fill, stHatch &hhh);
	int CreatePoly(char *file_name);
	int SetPoly(GeomHdr *poly_list);
};


///--------------------------stHatch Vtx---------------------------------------


class	SAS_HatchVertItem
{
public:
	long													y;
	int														vert;
	SAS_HatchVertItem											*next;
};

class	SAS_HatchVertHdr
{
public:
	SAS_HatchVertItem											*first;
	int													count;
	long													x;
};

#if 0 // TODO
class SYSGL_API stHatchVtx 
{
public:
						stHatchVtx							();
						stHatchVtx							(IterAreaVtx	*vtx,long	x0,long	dx,long xr = GL_MAX_COORD);
						~stHatchVtx							();
	void				MakeHatchVtx							(IterAreaVtx	*vtx,long	x0,long	dx,long xr = GL_MAX_COORD);
	long		GetX0										() {return	x0;};
	long		GetDx										();
	glPageMem *	GetSegs									(int	i);
	void				Clear										();
	void				PrepareForCrossing					(int	i,int	num_area);
	void				PrepareAllForCrossing				(int	num_area);
	int		GetCountVert							()	const {return vert_hdr.GetCount();};
	SAS_HatchVertItem		*GetFirstItem					(int	i) const {return vert_hdr.GetSecPtr(i) -> first;};
	SAS_HatchVertItem	*GetNextSeg							(int	i,SAS_HatchVertItem	*a_next);
	long		GetX										(int	i)	const {return	vert_hdr.GetSecPtr(i) -> x;};
protected:
	void				AddHatchCircle							(const GL_Point	&c, long	r);
	void				AddHatchArc								(const GL_Point	&p1,const GL_Point	&p2,const GL_Point	&c,long	r,int	*i_start, long *x_last,bool	last_seg = FALSE);
	void				AddHatchSeg								(const GL_Point	&p1,const GL_Point	&p2,int	*i_start, long *x_last,bool	last_seg = FALSE);
	void				AddPointToHatch						(const GL_Point	&p,int	i,bool	last_point = FALSE);
	void				AddVertToHatch							(const GL_Point	&p1,const GL_Point	&p2,int	i);
	void				SortHatchPoints						();

private:

	long													x0;
	long													dx;
	long													xr;
	GL_Vector<SAS_HatchVertHdr>							vert_hdr;
	glPageMem 												hatch;
	GL_Vector<SAS_HatchVertItem*>						sort_array;
};
#endif	// TODO stHatchVtx
#endif	//__SAS_VERT_H__

