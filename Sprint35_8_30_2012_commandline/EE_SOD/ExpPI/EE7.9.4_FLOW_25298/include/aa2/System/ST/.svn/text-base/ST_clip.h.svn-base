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

// Name:				ST_clip.h
// Date:				02.01.97
// Author:			Alexander Zabolotnikov	
// Description:	Scanner clipping

#ifndef __SAS_CLIP_H__
#define __SAS_CLIP_H__

#include "ST_ysld.h"
//#include "sas_mmax.h"
//#include "ST_dbg.h"

////////////////////////////////////////////////////////////////////////////////////
#define MAX_BOX_CROSS 2048
#define ITER_PAGE_SIZE 10000	///???
const unsigned long ALL_FILLED_BOUND	=	0x00000001; // clipp all segments for filled polylines
const unsigned long TILL_FIRST_CRS		=	0x00000002; // clipp all segments for        polylines
const unsigned long ALL_CLOSED_BOUND	=	0x00000004; // clipp all segments for closed polylines
const unsigned long IGNORE_HIDE_FLAG	=	0x00000008;	// process hidden polylines
const unsigned long IGNORE_MULTI_LAY	=	0x00000010;	// clip multi-layer objects (like drills) only once
const unsigned long IGNORE_ONE_POINT	=	0x00000020;	// skip one point polylines
const unsigned long INSIDE_WINDOW   	=	0x00000040;	// clip only polylines placed inside box
const unsigned long INVERT_FILLED_OBJ	=	0x00000080; // invert shapes on the layer
const unsigned long USE_SEC_BOXES		=	0x00000100; // use sector's fields
const unsigned long MARK_OVL_POLY     	=	0x00000200;	// mark polylines covered the box
const unsigned long IGNORE_OVL_POLY    	=	0x00000400;	// mark polylines covered the box
const unsigned long RESERVED_80     	=	0x00000800;	// do not use line width

class	ODB_CrsIter;

/////////////////////////////////////////////////////////////////////
// BitRng - represents covertions coordinate <==> discretes
/////////////////////////////////////////////////////////////////////
class SYSGL_API BitRng
{
public:
	short	numRng;		/* numRng == sizeof(long)        */
	long	sizRng;		/* sizRng == maxRange/numRng     */
	long	yiRng;		/* min							 */
	long	yaRng;		/* max							 */

	BitRng();
	void			InitRng		(const long ymin, const long ymax);
	unsigned long	DefMask		(const long ymin, const long ymax);
	unsigned long	DefMinMask	(const long ymin, const long ymax);
	int prn_bits(unsigned long a, char *str);
};

/////////////////////////////////////////////////////////////////////
// BoxCross - represents the cross point between clipping box & input polylines
/////////////////////////////////////////////////////////////////////
class	BoxCross
{
public:
	BoxCross() {ord = 0; b_ord = 0; p.x = GL_MAX_COORD;}
	GL_Point p;			// cross-point
	GeomHdr *node;	// cross-item
	int ix;					// segmeint index
	int ord;					// ord of node
	int b_ord;				// side of box
};

/////////////////////////////////////////////////////////////////////
// SAS_Clip - clipping processor
/////////////////////////////////////////////////////////////////////
class IntClip
{
public:
	BndSeg		dn, up, lv, rv;		// bound box
	GL_Array PArr;					// cross-point array
	GL_Point *p_int;				// current cross point
	GL_Array ItrArray;				// output crs-iterators
	ODB_CrsIter *itr0;				// first crs-iterator
	ODB_CrsIter *itr;				// current crs-iterator
	int num_itr, max_itr;			// used and reserved number of iterators
};
class SAS_Clip
{
public:
	BitRng rng;				// bits processor
	unsigned long mask;		// extended mask
	unsigned long min_mask;	// minimal mak
	void InitRng(const long ymin, const long ymax);
	long old_w;		// maximal polyline width

	SAS_Clip()
	{	sl = NULL; num_cross = 0; c_pnt = NULL; sc.itr0 = NULL; sc.num_itr = 0; sel_flag = ALL_FILLED_BOUND;
		ll.x = GL_MAX_COORD;
		ll.y = GL_MAX_COORD;
		ur.x = GL_MIN_COORD;
		ur.y = GL_MIN_COORD;
	}
	~SAS_Clip(){}
	void SetBox(const GL_Box &box);
	void SetSld(stSlider *s) {sl = s;}
	void Cross(const GL_Box *box = NULL);
	void InitIter();
	void Draw(GL_Transform &conv);
	void Draw();
	//virtual void StartPoly(GL_Node *node, int idx, GL_Point *beg);
	//virtual void PiecePoly();
	void InitLoc(GL_Array *arr = NULL);
	void CreateLoc(int ord, int idx);
	void CrossLoc(const GL_Box *box);
	void CrossSeg(GL_Point &lpt, GL_Point &rpt);
	void DrawLoc();
	void DrawLoc(IterPolyCross &itr);
	void GetGeoItr(int ord, IterGeom &geo);
	void GetGeoItr(XEvent *xi, IterGeom &geo);
	int ClipLoc(IterPolyGeom &geo, const GL_Box &box, GL_Array *arr);
	int ClipLoc1(GeomHdr *gh, const GL_Box &box, GL_Array *arr);
	int ClipSeg1(GeomHdr *gh, GL_Point &lpt, GL_Point &rpt, GL_Array *arr);
	int ClipList(const GL_Box *box, GeomHdr *clist, GeomHdr *flist, GL_Array *arr, unsigned long sel_flag = 0xffffffff);
//private:
	int  EvComp();
	int  CheckInt ();
	void CheckSeg(BndSeg &seg, GL_Point &beg,	const int ord, const int b_ord);
	void CheckOneSeg(BndSeg &seg, SAS_Seg &sg, GL_Point &beg, const int b, const int dir);
	inline void CheckOneSeg(BndSeg &s, const int ord, GL_Point &bg, const int b, const int dir);
	void WriteCross(const GL_Point &pnt, const int dn_ord, const int bnd_ord);
	int  CheckStatus(BndSeg &seg, int b, const int ord, const int ev, const int b_ord);
	void SetCrossPoint(const GL_Point *pnt, const int a_dir, const int b_ord,
		                const int ix, GeomHdr *node);
	void P_Delete();
	void CreateIter(int ord, int idx, GL_Point *beg = NULL);
	inline void CreateIter(int ord, GL_Point *beg = NULL);

	inline SAS_Seg *GetSeg(const int ord,  const GL_Point *point, const int b_ord);
	stSlider	*sl;				// slider to input data
	long	xsl;					// curren coordinate
	GL_Point	ll, ur, bc, pnt[2];	// bound points
	GL_Point	p[5];				// bound points
	int			up_ord, dn_ord;		// current ords

	GL_Array *ItrLoc;				// output iterators
	IterPolyCross *loc0;			// first iterator
	IterPolyCross *loc;				// current iterator
	int num_loc, max_loc;			// used & reserved number of iterators

	int num_cross;					// the number of cross points
	BoxCross *c_pnt;				// first cross point

	SAS_Seg seg;						// current segment
	VertItem *itm, *up_itm, *dn_itm;	// current slider items
	GL_Box box;							// expanded box
	unsigned int sel_flag;				// clipping mode for selector
	
	int	num_ovl_poly;
	double ovlMin, ovlMax;
	GeomHdr *ovlPoly;
	
	static IntClip sc;
};

/////////////////////////////////////////////////////////////////////
// CheckOneSeg check boundary segment for crossing
/////////////////////////////////////////////////////////////////////
inline void SAS_Clip::CheckOneSeg
(
	BndSeg		&s,	// segment
	const int	i,		// sl- index
	GL_Point	&bg,	// current point
	const int	b,		// side of bound
	const int	dir	// mutual position
)
{
	if(i > 0 && i < sl->GetCount()) {
		CheckOneSeg(s, *(sl->GetSeg(i, &bg)), bg, b, dir);
	}
}

/////////////////////////////////////////////////////////////////////
// CreateIter - create iterator
/////////////////////////////////////////////////////////////////////
inline void SAS_Clip::CreateIter
(
	int			ord,	// ord of segment
	GL_Point	*beg	// first point
)
{
	VertItem *y_item = sl->GetYItem(ord);
	CreateIter(ord, y_item->xi->GetOffSet(), beg);
}

/////////////////////////////////////////////////////////////////////
// GetSeg - set segment by ord
/////////////////////////////////////////////////////////////////////
inline SAS_Seg *SAS_Clip::GetSeg
(
	const int ord,				// ord of segment
	const GL_Point *point,	// current point
	const int b_ord			// range
)
{
	SAS_Seg *segmnt = sl->GetSeg(ord, NULL);
	if (segmnt) {
		if (segmnt->iy > 2 || b_ord + 1 == segmnt->iy) {
			return(NULL);
		}
		if (point) {
			segmnt->GoToPnt(*point);
		}
	}
	return segmnt;
}

#if 1
/////////////////////////////////////////////////////////////////////
// ODB_CrsIter - iterator on the part of geom item between two points
/////////////////////////////////////////////////////////////////////
class ODB_CrsIter: public IterGeom
{
public:	// data
	short	beg_id;		// index of first point in the piece
	short	end_id;		// index of last point on the piece
	GL_Point beg_p;	// start of piece
	GL_Point end_p;	// end of piece

public:
	//GL_Point GetPoint () const;
	inline void SetIter(IterGeom &crn);
	inline void SetIter(IterPolyCross &crn);
	inline void SetEndId(int id);
	inline void SetBegId(int id);
	inline int  GetEndId(int id);
	inline int  GetBegId(int id);
	inline void SetBegPnt (const GL_Point &pnt);
	inline void SetEndPnt (const GL_Point &pnt);

	void Draw(GL_Transform &conv);
	void Draw();
};
//--------------------------------------------
//inline GL_Point ODB_CrsIter::GetPoint () const
//{
//	if(id == beg_id){return beg_p;} else if(id == end_id){return end_p;} else return IterGeom::GetPoint();
//}
inline int  ODB_CrsIter::GetEndId(int id) { return end_id;}
inline int  ODB_CrsIter::GetBegId(int id) { return beg_id;}
inline void ODB_CrsIter::SetEndId(int id) {end_id = (short)id;}
inline void ODB_CrsIter::SetBegId(int id) {beg_id = (short)id;}
inline void ODB_CrsIter::SetBegPnt(const GL_Point &pnt) {beg_p = pnt;}
inline void ODB_CrsIter::SetEndPnt(const GL_Point &pnt) {end_p = pnt;}
inline void ODB_CrsIter::SetIter( IterGeom &crn)
{
	SetParent(crn.GetParent());
	GoTo (0);  // Random access
	crn.GetLast();	 end_p = crn.GetPoint(); end_id = (short)crn.GetCurentId();
	crn.GetFirst(); beg_p = crn.GetPoint(); beg_id = 0;
}

inline void ODB_CrsIter::SetIter( IterPolyCross &crn)
{
	IterGeom *itr = (IterGeom *)(&crn);
	IterGeom *my = (IterGeom *)(this);
	*my = *itr;
	SetParent(crn.GetParent());
	GoTo(crn.GetEndId());  // Random access
	end_p = GetPoint(); end_id = (short)crn.GetEndId();
	GoTo (crn.GetBegId());  // Random access
	beg_p = GetPoint(); beg_id = (short)crn.GetBegId();
}
#endif // ODB_CrsIter
///////////////////////////////////////////////////////////////////////////////
int SYSGL_API ClipScanArray(stScan **s, int n, int o,const GL_Box &box, GL_Array *arr);
unsigned int SYSGL_API GetClipFlg();
void SYSGL_API SetClipFlg(unsigned int flag);

inline int IsIncludeBox(GL_Box &a, GL_Box &b)
{
	return (a.ll.x <= b.ll.x && b.ur.x <= a.ur.x &&	a.ll.y <= b.ll.y && b.ur.y <= a.ur.y)? 1 : 0;
}

#endif	// _SAS_CLIP_H

