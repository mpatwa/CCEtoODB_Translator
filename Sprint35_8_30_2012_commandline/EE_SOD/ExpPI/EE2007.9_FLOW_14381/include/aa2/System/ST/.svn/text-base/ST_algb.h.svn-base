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

// Name:				ST_algb.h
// Date:				12.14.97
// Author:			Alexander Zabolotnikov	
// Description:	algebraic operations

#ifndef __SAS_ALGB_H__
#define __SAS_ALGB_H__

#include "ST_scan.h"
#include "ST_bars.h"
#include "ST_tree.h"

#define MIN_EXPAND_DIST 100
class Poly2Pt;
class MetalVertex;
class PShapes;
class ExtSeg;

SYSGL_API int AddGeomListToX(GeomHdr *node);
extern int AddGeomToX(XEventArr *XArr, GeomHdr *node,	short a_ori, short w_ori);
extern int AddGeomToX(stScan *scan, GeomHdr *node,	short a_ori, short w_ori);
inline void ClearScanPoly( stScan *scan);


/////////////////////////////////////////////////////////////////////
// LineStatus - represent current status of geom item
/////////////////////////////////////////////////////////////////////
class  LineStatus
{
public:
	GeomHdr	*gh;		//	pointer to geom item
	GL_Point	lp;		// left range of piece
	int			x_ind;	// current segment index
	int			vis;		// visibility
	int			ord;		// y-position
};

/////////////////////////////////////////////////////////////////////
// glAreaData - internal processor of different algebraic operations
/////////////////////////////////////////////////////////////////////
class  SYSGL_API glAreaData
{
public:
	stSlider	*sl;		// slider for processing
	stScan		*scan;		// input scanner
	stScan		*out_scan;	// output scanner
	LineStatus	*lst;		// array to keep the segments current status
	int 		ref;		// the number of references to glAreaData
	int 		num_inp;	// the number of input polylines
	int			num_out;	// the number of output polylines
	int			dx_static;	// the step between output static sliders
	BarHdr		*bars;		// bars,used to separate in/out into bars
	unsigned int scan_flag;	// flag of scanner (local/main)
	int			dec_mode;	// area decomposition mode
	short		proc_ver;
	short		skip_voids;	// ignore input voids
	int			merge_poly;	// merge input polylines to polygons

	GeomHdr		*soft_area;		// input soft area list
	GeomHdr		*rigid_area;	// input rigid area list
	GeomHdr		*soft_window;	// input soft window list
	GeomHdr		*rigid_window;	// input rigid window list
	GeomHdr		*hatch_first;	// input hatch lines list
	GeomHdr		*in_list;	// input polygons list
	GeomHdr		*out_list;	// output polygons list
	GeomHdr		*net_list;	// same net list
	glPageMem	pm_points;	// pages for points
	glPageMem	pm_tree;	// pages for tree nodes
	AreaTreeNode *top_of_tree;		// top of tree
	OpenBranch	*last_open_branch;	// last open branch
	OpenBranch	*first_open_branch;	// first open branch
	stData		*ud;				// stData for processing
	ChnNode		*last_node;
	ChnNode		*first_node;
	ChnNode		*dn_node;
	int			ovl_win;
	int			ovl_area;
	int			rev_win;
	int			rev_area;
	int			crs_win;
	int			crs_area;
	int			crn_set;
	int			is_sw3;
	int			alg_type;
	stScan		*scan2;		// input scanner for set2
	//~~~~~~~~~~~~~~~~~
	GL_Array	*NetArr;
	GL_Array	*PPtArr;
	Poly2Pt		*pp0, *ppt;
	SAS_Seg		**s_line;
	PShapes		*lpsm;
	Poly2Pt		*p20;
	int			num20;
	unsigned int inc_mode;
public:
	glAreaData();
	~glAreaData();
	int		(*ChkVis)(VertItem *in, VertItem *out);
	inline void	SetChkFunc(int (*chk_fun)(VertItem *in, VertItem *out));
	int		SetInput(int num = 0, int inf_ovl = 0, int rev = 0);
	int		UniteArea();
	int		SubtrArea();
	int		CrossArea();
	void	CreateHdrList();
	void	AddGeomInt(GeomHdr *gh, short a_ori = 1);
	int		AddGeom(GeomHdr *gh, int num_area, unsigned int vis_flg = 0);
	int		AddGeom(GeomHdr *gh, short a_ori = 0, short w_ori = 0);
	int		AddGeom(GeomHdr *gh, short a_ori, short w_ori, short a_rigit);
	inline int	AddGeomCent(GeomHdr *gh,short a_rigit = 1);
	int		AddPolygon(GeomHdr *gh, long dist = 0, short dir = 0, short a_ori = 0);
	int		Init();
	void	Clear();
	int		IsVisible(VertItem *itm);
	int		SetVisible(VertItem *in, VertItem *out);
	int		CalculateAreas();
	int		CalculateArea2();
	int		CalculateArea3();
	int		CalculateMultiArea();
	int		MultiAreas();
	int		WriteXi(GeomHdr *gh, short ev, int iy);
	GeomHdr	*WriteVisLine(int i, GL_Point *rp);
	inline void	LinkToScanner(GeomHdr *gh);
	inline void	SetBars(BarHdr *a_bar, unsigned int a_flag);
	inline void  SetStaticSl(int dx);

	int		UpdateAlg();
	int		UpdateAlg3();
	int		WriteSegment (SAS_Seg **scan_y, SAS_Seg *seg, int out_flag, GL_Point *pt);
	int		WriteSeg3 (SAS_Seg **scan_y, SAS_Seg *seg, int out_flag, GL_Point *pt);
	int		InsertBranch (SAS_Seg **scan_y, SAS_Seg *seg, GL_Point *pt);
	int		DeleteBranch (SAS_Seg **scan_y, SAS_Seg *seg, GL_Point *pt);
	void	LinkTreeNodes (AreaTreeNode *n1, AreaTreeNode *n2, int dir);
	void	DelTreeNode(AreaTreeNode *node);
	ChainPoint		*DelChain(ChainPoint *pt);
	AreaTreeNode	*DelTree(AreaTreeNode *node);
	GeomGroup		*CreateTree();
	int		IsIntersect(unsigned long a_flag, unsigned long m_flag, int check_all = 1);
	int		AlgSec();
	int		GetSameNetLinks(PShapes *lpsm);
	void	WritePart(SAS_Seg *seg, GL_DPoint &pt);
	int		StartUpg();
	int		UpgAlg();
	void	EndUpg();
	void	WriteUpgSeg(SAS_Seg *seg, GL_DPoint &pt);
	void	WriteTcsSeg(SAS_Seg *seg, GL_DPoint &pt);
	void	WriteNetSeg(ExtSeg *seg, GL_DPoint &pt);
	Poly2Pt *CreatePP(GL_DPoint &pt, SAS_Seg *s, int pdir, int up, int beg, int end, int prev, int next);
	Poly2Pt *CreateNt(GL_DPoint &pt, ExtSeg  *s, int pdir, int up, int beg, int end, int prev, int next);
	GeomHdr *P2P_Poly(Poly2Pt *pp0, int num);
	void	DeletePP(int id);
	void	DeleteNetPP(int id);
	void	ChkThrObj(SAS_Seg *seg, GL_Point &end);
	int		IsThrObj(SAS_Seg *seg);
	inline void CheckP2P(int rc, char *str);
	int		SubtractZones(Poly2Pt *sp0, int snum, Poly2Pt *wp0, int wnum);
	int		StartZ();
};
/////////////////////////////////////////////////////////////////////
// SetChkFunc - set function for visibility checking
/////////////////////////////////////////////////////////////////////
inline void	glAreaData::SetChkFunc(int (*chk_fun)(VertItem *in, VertItem *out))
{
	ChkVis = chk_fun;
}

/////////////////////////////////////////////////////////////////////
// SetStaticSl - set the step between static sliders
/////////////////////////////////////////////////////////////////////
inline void  glAreaData::SetStaticSl
(
	int dx	// step
)
{
	dx_static = dx;
}

/////////////////////////////////////////////////////////////////////
// SetBars - set bar values
/////////////////////////////////////////////////////////////////////
inline void	glAreaData::SetBars
(
	BarHdr			*a_bar,	// new bar
	unsigned int	a_flag	// bar flag (local/main)
)
{
	bars = a_bar;
	scan_flag = a_flag;
}

inline int glAreaData::AddGeomCent
(
	GeomHdr *gh,	// source polyline
	short a_rigit		// visibility flag for soft area
)
{
	gh->flags |= GL_CENT_LINE;
	return AddGeom(gh, 0, 0, a_rigit);
}

/////////////////////////////////////////////////////////////////////
// stArea - processor of different algebraic operations
/////////////////////////////////////////////////////////////////////
class SYSGL_API stArea
{
public:
	glAreaData *data;	// input/output data
	stArea();
	~stArea();
	inline void Clear();

	int    UniteArea(stArea &next);
	int    SubtrArea(stArea &next);
	int    CrossArea(stArea &next);

	stArea operator=(stArea &next);
	stArea operator=(stArea *next);
	stArea operator+(stArea &next);
	stArea operator-(stArea &next);
	stArea operator*(stArea &next);

	short   operator==(stArea &next);
	short   operator!=(stArea &next);

	inline int	AddGeom(GeomHdr *gh, int num_area, unsigned int vis_flg = 0);
	int	AddGeom( GeomHdr *gh, short a_ori = 0, short w_ori = 0);
	int	AddGeom(GeomHdr *gh, short a_ori, short w_ori, short a_rigit);
	inline int	AddGeomCent(GeomHdr *gh,short a_rigit = 1);
	inline int	AddPolygon(GeomHdr *gh, long dist = 0, short dir = 0, short a_ori = 0);
	inline int	CalculateAreas();
	inline int	MultiAreas();
	inline GeomGroup *CreateTree();
	inline void	SetBars(BarHdr *a_bar, unsigned int a_flag);
	inline void  SetStaticSl(int dx);

	inline void	SetChkFunc(int (*set_fun)(VertItem *in, VertItem *out));
	inline void	SetDecMode(int d_mode = 2);
};

inline int	stArea::AddGeom(GeomHdr *gh, int num_area, unsigned int vis_flg)
{
	if (data) {
		return data->AddGeom(gh, num_area, vis_flg);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// clear memory, set default values
/////////////////////////////////////////////////////////////////////
inline void	stArea::Clear()
{
	if (data) {
		data->Clear();
	}
}

/////////////////////////////////////////////////////////////////////
// set modes for MultiArea
/////////////////////////////////////////////////////////////////////
inline void	stArea::SetDecMode
(
	int d_mode	// 2  - add two vertical segments  to isolated points
					// 1  - add upper vertical segment to isolated points
					// -1 - add lower vertical segment to isolated points
					// 0  - switch off the isolated points processing
)
{
	if (data) {
		data->dec_mode = d_mode;
	}
}

/////////////////////////////////////////////////////////////////////
// SetBars - set bars
/////////////////////////////////////////////////////////////////////
inline void	stArea::SetBars
(
	BarHdr			*a_bar,	// new bar
	unsigned int	a_flag	// bar flag
)
{
	if (data) {
		data->SetBars(a_bar, a_flag);
	}
}

/////////////////////////////////////////////////////////////////////
// SetStaticSl - set the step between static sliders
/////////////////////////////////////////////////////////////////////
inline void  stArea::SetStaticSl
(
	int dx	// the step
)
{
	if (data) {
		data->SetStaticSl(dx);
	}
}

/////////////////////////////////////////////////////////////////////
// CalculateAreas - algebraic operation: calculate areas by data wich
//						  was loaded by AddGeom()
//						  rezult - area boundaries and foreground lines are loaded
//						  into outscan
//						  set flags for each rezulting item:
//						  item->data->flags & GL_AREA_BELOW - upper boundary
//						  item->data->flags & GL_AREA_ABOVE - lowel boundary
//						  else - foreground line (hatch line in BRE)
/////////////////////////////////////////////////////////////////////
inline int	stArea::CalculateAreas()
{
	if (data) {
		return data->CalculateAreas();
	} else {
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////
// MultiAreas - algebraic operation: calculate areas by data wich
//					was loaded by AddGeom() with rigid parameters
//					rezult - three types of area boundaries (free, soft, rigid)
//					and foreground lines are loaded	into outscan
//					set two flags: one for each side of rezulting item:
//					item->data->flags & GL_AREA_BELOW - upper bnd of free area
//					item->data->flags & GL_AREA_ABOVE - lowel bnd of free area
//					item->data->flags & GL_SOFT_BELOW - upper bnd of soft area
//					item->data->flags & GL_SOFT_ABOVE - lowel bnd of soft area
//					item->data->flags & GL_WIND_BELOW - upper bnd of rigid area
//					item->data->flags & GL_WIND_ABOVE - lowel bnd of rigid area
//					only line between next pairs of areas are permitable in result:
//						free-free												  (hatch lines)
//						free-soft, soft-free									  (soft boundary)
//						free-rigid, rigid-free, soft-rigid, rigid-soft (rigid boundary)
/////////////////////////////////////////////////////////////////////
inline int stArea::MultiAreas()
{
	if (data) {
		return data->MultiAreas();
	} else {
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////
// CreateTree:	creates the topology tree of area as a GeomGroup
//					- made algebraic operation: unites all outlines
//					  and subtract all voids
//					- represents resulting area boundary as a tree of nesting
// input data must be added in glAreaData before by AddPolygon() methods
/////////////////////////////////////////////////////////////////////
inline GeomGroup *stArea::CreateTree()
{
	if (data) {
		return data->CreateTree();
	} else {
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////
// SetChkFunc - set function for visibility checking
/////////////////////////////////////////////////////////////////////
inline void	stArea::SetChkFunc(int (*chk_fun)(VertItem *in, VertItem *out))
{
	if (data) {
		data->SetChkFunc(chk_fun);
	}
}

/////////////////////////////////////////////////////////////////////
// AddPolygon - add boundary polygon for the future area calculation (CreateTree)
// Examples (see parameter description before):
//		glAreaData ad;
//		ad.AddPolygon(g1);	-- boundary polygon will be the same, as g1 (without expanding),
//									   if g1 is closed or will encircle g1 by distance 
//										MAX(width,MIN_EXPAND_DIST), if g1 is open
//									--	type of boundary (outline/void) will be determined
//										by orientation
//									-- orientation will be calculated
//		ad.AddPolygon(g1, dist)	-- if dist == 0 then see previous example, else
//									-- boundary polygon will create by expanding g1 on dist
//									--	type of boundary (outline/void) will be determined
//										by orientation
//									-- orientation will be calculated
//		ad.AddPolygon(g1, dist, dir)	-- if dir == 0 then see previous example, else
//									-- boundary polygon will create by expanding g1 on dist
//									--	type of boundary: dir ==1 for outline; dir = -1 for void
//									-- orientation will be calculated
//		ad.AddPolygon(g1, dist, dir, ori) -- if ori == 0 then see previous example, else
//									-- boundary polygon will create by expanding g1 on dist
//									--	type of boundary: dir == {1 for outline; -1 for void }
//									-- orientation: ori == {1 for counterclockwise, -1 clockwise}
/////////////////////////////////////////////////////////////////////
inline int	stArea::AddPolygon
(
	GeomHdr	*gh,	// source polyline - open path or closed polygon (or circle)
							// the boundary of area will be created as an expanded polygon
							// arround source polyline
	long	dist,	// distance for expanding
							//		dist = 0 (default value) => closed polyline will not be
							//             expanded, open polyline will be expanded by
							//					MAX(width,MIN_EXPAND_DIST)
							//		dist > 0 expand to outside of area
							//		dist < 0 expand to inside area
	short			dir,	// boundary type
							//		dir = 1  for outline
							//    dir = -1 for void
							//    dir = 0  (default value) => type will determine
							//             by orientation: counterclockwise means outline
	short			a_ori	// orientation of polygon, will ignore for circle
							//		if a_ori = 0 (default value), then orientation will be
							//		determined by polygon
)
{
	if (data == NULL) {
		data = new glAreaData;
		data->ref += 1;
	}
	if (data) {
		return data->AddPolygon(gh, dist, dir, a_ori);
	}
	return 0;
}

inline int	stArea::AddGeomCent
(
	GeomHdr	*gh,		// source polyline
	short			a_rigit	// flag of visibility on the soft area
)
{
	if (data == NULL) {
		data = new glAreaData;
		data->ref += 1;
	}
	if (data) {
		return data->AddGeomCent(gh, a_rigit);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// SetOutFlags - set flags for output polyline
/////////////////////////////////////////////////////////////////////
inline void SetOutFlags
(
	VertItem	*itm,	// item, wich refers to polyline
	GeomHdr	*gh	// polyline
)
{
	gh->flags =	(itm->xi->node->flags & GL_AREA_CLEAR);
	if (itm->next) {
		if (itm->next > 0) {
			gh->flags |= GL_AREA_BELOW;
		} else {
			gh->flags |= GL_AREA_ABOVE;
		}
	} else if (itm->prev) {
		if (itm->prev > 0) {
			gh->flags |= GL_AREA_ABOVE;
		} else {
			gh->flags |= GL_AREA_BELOW;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// GetAreaDir - get direction from polyline to area
/////////////////////////////////////////////////////////////////////
inline int GetAreaDir
(
	GeomHdr *gh	// polyline
)
{
	if (gh) {
		if (gh->flags & GL_AREA_ABOVE) {
			return 1;
		} else if (gh->flags & GL_AREA_BELOW) {
			return -1;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// LinkToScanner - link geom item to scanner
/////////////////////////////////////////////////////////////////////
inline void	glAreaData::LinkToScanner
(
	GeomHdr *gh	// geom item
)
{
	if (gh && out_scan) {
		if (bars == NULL || 	bars->GeomInBar(gh, scan_flag)) {
			gh->next_geom = out_scan->first_gh;
			out_scan->first_gh = gh;
			num_out++;
		} else {
			gh->Delete();
		}
	}
}

/////////////////////////////////////////////////////////////////////
// GetBoundType - determines type of boundary polyline
// return:
//     1 - the upper boundary;
//    -1 - the lower boundary
//     0	- it is not the boundary
/////////////////////////////////////////////////////////////////////
inline int GetBoundType
(
	const	VertItem	*itm	// pointer to polyline description
)
{
	if (itm) {
		if (itm->next) {			// area
			return itm->next;
		} else if (itm->prev) { // window
			return itm->prev * 2;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// DelPolyList - delete geom list from memory
/////////////////////////////////////////////////////////////////////
inline GeomHdr *DelPolyList
(
	GeomHdr *gh	// first geom in list
)
{
	GeomHdr *prev;
	while (gh) {
		prev = gh;
		gh = gh->next_geom;
		prev->Delete();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// ClearScanPoly - clear scanner and delete scanner's geom list from memory
/////////////////////////////////////////////////////////////////////
inline void ClearScanPoly
(
	stScan	*scan	// source scanner
)
{
	if (scan) {
		scan->Clear();
		DelPolyList(scan->first_gh);
		scan->first_gh = NULL;
		scan->last_gh = NULL;
	}
}

#endif	//	__SAS_ALGB_H__

//
// End  of  file
//

