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

// Name:				ST_seg.h
// Date:				10.20.86
// Author:			Alexander Zabolotnikov	
// Description:	Scanner segment main functions

#ifndef __SAS_SEG_H__
#define __SAS_SEG_H__

#include "../Gl/GL_PolyIter.h"	//dbg
#include "ST_xitm.h"
#include "ST_util.h"
// SAS_SegType
#define	CIR_T 0
#define	VSG_T 1
#define	SEG_T 2
#define	ARC_T 4

class GL_Transform;

/////////////////////////////////////////////////////////////////////
// GeoSeg - iterator to geom item, include extended parameters
//           of current segment and processing status
/////////////////////////////////////////////////////////////////////
class SYSGL_API GeoSeg : public IterPolyGeom
{
public:

	double		a, b, r;	// Seg: y = a*x + b	| Arc: xc, yc, r | Vsg - [(r,a),(r,b)]
	GL_Point	beg;		// start
	GL_Point	end;		// end
	long		ymin;		// lower y-value
	long		ymax;		// upper y-value
	int			iy;			// position  in vertical
	short		type;		// arc | seg | vsg
	short		dir;		// Poly Direction
	short		is_poly;	// parent type 1- polyline, 0 - circle
	short		label;
	int			num_int;	// the Number of Intersections
	long		id_io;		// index of input data
	
	GeoSeg();
    GeoSeg(const IterPolyGeom &crn);
	int SetSegByData(GeomHdr *node, const int idx, const GL_Point *pnt);
    inline void Set(const IterPolyGeom &crn);
    int SetItem(GL_Node *node, int OffSet, short dir = 0);
	int SetItem(const XEvent *xi, const GL_Point *p = NULL);
	int SetSegByXi(const XEvent *xi);
	inline int IsRight();
	int GetNextRight(const GL_Point *p = NULL);
	int GetNRight(const GL_Point *pnt = NULL);
	int GoToX(const long x);

	inline int GoToPnt(const GL_Point &pnt);

	inline int	GetOrd()  const { return iy;}
	inline int	GetType() const { return (int)type;}
	inline int	GetDir()	 const { return (int)dir;}

	inline GL_Point	GetStartPoint() const { return(beg);}
	inline GL_Point	GetEndPoint()	 const { return(end);}
	inline GL_Point	GetCenter()		 const { GL_Point c; c.x = LROUND(a); c.y = LROUND(b);return(c);}
	inline long	GetRadius()		 const { return LROUND(r);}
	inline int	IsArc()		 const { return(type == ARC_T);}

	void			UpdMinMax	();
	inline void		ChangeRange();
	int GetTngDir	(const GL_Point &p, double &diff1, double &diff2) const;
	int GetTngDir	(double x, double y, double &diff1, double &diff2) const;
	void GetTng		(double x, double y, double &dx, double &dy) const;
	int			PntOnSeg		(const GL_Point *p) const ;
	int			PntOnSeg		(double x, double y, double eps = 1.0) const ;
	int			PntPosition	(const GL_Point *p) const ;
	int			PntPosition	(double x, double y, double eps = 1.0) const ;
	int			Y_Value		(const long x, long *py) const ;
	void		YProj			(GL_Point &pt);
	void		ConvertToSeg();
	int			AdjustCenter();
	void		PrintSeg		(FILE *out = NULL);
	//void		DrawSeg		();
	void			DrawSeg		(GL_Transform &conv);
};

class SYSGL_API SAS_Seg : public GeoSeg
{
public:
	short			num_area;	// area status
	short			dir_area;	// type of boundary
	short			num_sw;		// sw status
	short			dir_sw;		// sw direction
	short			num_win;		// window status
	short			dir_win;		// direction of window

	SAS_Seg();
	SAS_Seg(const IterPolyGeom &crn) : GeoSeg(crn) {	num_area = 0; dir_area = 0;num_win = 0; dir_win	= 0;}
};

//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// stSegArr - array for SAS_Seg
/////////////////////////////////////////////////////////////////////
class SYSGL_API stSegArr : public	glPageMem
{
public:
	stSegArr(){ Init(sizeof(SAS_Seg),DELTA_SIZE);}
	~stSegArr() {}
	inline SAS_Seg *NewSeg();
	inline void		DelSeg(SAS_Seg *seg);
	inline void		DelAll();
private:
	//SAS_Seg *fre_seg, *tmp_seg;
};

/////////////////////////////////////////////////////////////////////
// NewSeg - get memory for new segment
/////////////////////////////////////////////////////////////////////
inline SAS_Seg *stSegArr::NewSeg()
{
	return (SAS_Seg *)NewItem();
}
/////////////////////////////////////////////////////////////////////
// DelSeg - link segment's memory to free memory
/////////////////////////////////////////////////////////////////////
inline void stSegArr::DelSeg(SAS_Seg *seg)
{
	ASSERT_WRN(seg, "stSegArr: Null segment pointer");
	DelItem((void *)seg);
}
/////////////////////////////////////////////////////////////////////
//	free all memory
/////////////////////////////////////////////////////////////////////
inline void stSegArr::DelAll()
{
	glPageMem::FreeAll();
}
///////////////////// BndSeg //////////////////////////////////////////////
class SYSGL_API BndSeg : public SAS_Seg
{
public:
	BndSeg() : SAS_Seg() { next = NULL;}
   BndSeg(IterPolyGeom &crn) : SAS_Seg( crn) { next = NULL;}
   BndSeg(BndSeg *n) { next = n;}
	BndSeg operator= (SAS_Seg &seg);
	
	inline int IsValid () { return TRUE;}
   inline void LinkSeg( BndSeg *n)	{ next = n;}
	void Set(long xl, long y1, long x2, long y2);
	void Set(GL_Point &a, GL_Point &b);
	void CutSeg(GL_Point &p, BndSeg **cut_seg);
	void CutSeg(long  x, BndSeg **cut_seg);
	
	BndSeg *next;
};

class SegLow : public glLow
{
public:
	int IntCirCir (SAS_Seg *arc1, SAS_Seg *arc2);
	int IntCirLin (SAS_Seg *arc,  SAS_Seg *seg);
};

#endif

