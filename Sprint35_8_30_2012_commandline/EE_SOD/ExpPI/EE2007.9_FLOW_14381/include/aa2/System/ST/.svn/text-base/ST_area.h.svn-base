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

// Name:				ST_area.h
// Date:				02.01.00
// Author:			Alexander Zabolotnikov	
// Description:	sas_areas interface

#ifndef __SAS_AREA_H__
#define __SAS_AREA_H__

#include "ST_algb.h"
#include "ST_def.h"

#define CSN45 (_SQRT2/2.0)
#define SNS45 CSN45
#define L2_EPS 2
#define D2_EPS 2.0
#define EPS_1 1
#define D1_EPS 1.0


// options of GetIntersections0() 
const	unsigned int INT_SAVEPOINTS		=	1;		// save all points (including duplicated and intermediate - i.e. result isn't filtered)
const	unsigned int INT_SINGLEPOLY		=	2;		// process first polyline in chain ( without *next_geom)
const	unsigned int INT_NOCHECKBOXES	=	4;		// don't check boundary boxes ( use it for speed optimization in some cases)
const	unsigned int INT_FILTERTANGENT	=	8;		// delete tangent points, save points of intersections only
const	int		 INT_DIGERROR			=	2;		// default digitizing error (if distance<INT_DIGERROR - points are congruent)

//~~*~~~~~~~~~~~~~ CrossRf ~~~~~~~~~~~~~~~~~~~~~
class SYSGL_API CrossRf : public GL_DPoint
{
public:
	GeomHdr *poly1;
	GeomHdr *poly2;
	double t1;
	double t2;

   bool operator== (const CrossRf &a_crs) const
   {
	   return x == a_crs.x && y == a_crs.y && t1 == a_crs.t1 && t2 == a_crs.t2 && poly1 == a_crs.poly1  && poly2 == a_crs.poly2;
   }
   bool operator!= (const CrossRf &a_crs) const
   {
	   return !(*this == a_crs);
   }
   const CrossRf &operator= (const CrossRf &a_crs)
   {
	   x = a_crs.x; y = a_crs.y; 
      t1 = a_crs.t1; t2 = a_crs.t2;
      poly1 = a_crs.poly1; poly2 = a_crs.poly2;
	   ASSERT_WRN(IsValid(),"Invalid point");
	   return *this;
   }
};

//~~*~~~~~~~~~~~~~ MentPt ~~~~~~~~~~~~~~~~~~~~~
class SYSGL_API MentPt
{
public:
	GL_Point pt;
	PolyArc *arc;
	short is_marked;
	short dir;
	MentPt *next;
};

extern SYSGL_API int InitScan(stScan *scan, GeomHdr *gh, unsigned int flag = SCAN_DEF_TYPE);
extern SYSGL_API int F_Poly(GeomHdr *gh, char *file_name, char *mode);
extern SYSGL_API GeomHdr *CreateGeomList(GeomGroup	*gg);
extern SYSGL_API GeomHdr *BoxToPoly(GL_Box &box);
extern SYSGL_API GeomHdr *ListToGroup(GeomHdr *master);
extern SYSGL_API GeomHdr *GroupVoids(GeomHdr *gh);
extern SYSGL_API GeomHdr *GetBoundAtPoint(GL_Point &p, stScan *s, GeomHdr *g, int w = 0);
extern SYSGL_API GL_Point GetLeftPnt(GeomHdr *gh);
extern SYSGL_API GL_Point *GetIntersectPnt(GeomHdr *gh1, GeomHdr *gh2, int *num_pnt);
extern SYSGL_API CrossRf *GetIntersections(GeomHdr *p1, GeomHdr *p2, int *num_pnt);
extern SYSGL_API CrossRf *GetIntersections0(GeomHdr *p1, GeomHdr *p2, int *num_pnt, unsigned int a_flgs=0, int eps=INT_DIGERROR);
extern SYSGL_API CrossRf *GetIntersections0(GeomHdr **p1,int a_sz1, GeomHdr **p2, int a_sz2, int *a_num, unsigned int a_flgs=0, int a_epsilon=INT_DIGERROR);
extern SYSGL_API CrossRf *GetIntersections0(GeomHdr *p1, GeomHdr *p2, int *num_pnt, bool *a_isfirstinside, unsigned int a_flgs=0, int eps=INT_DIGERROR);
extern SYSGL_API GeomHdr *GetPolyPiece(GeomHdr *poly, CrossRf *crs1, CrossRf *crs2);
extern SYSGL_API GeomHdr *GetMentorgon(stScan *scan, GeomHdr *first_gh);
extern SYSGL_API GeomHdr *ArrayToPolygon(MentPt *arr, int num);

//dbg
extern SYSGL_API GeomHdr **BuildClippings(GeomHdr **area, int numarea, GeomHdr **clip, int numclip, int *numresult, 
										  GeomHdr ***cuts, int *numcuts,unsigned int a_flgs=0,int a_eps=INT_DIGERROR);

inline GeomHdr *GroupToList(GeomHdr **a_poly)
{
	if ((*a_poly) && (*a_poly)->GetGeomType() == SGN_TYPE_GROUP) {
		GeomHdr *poly = CreateGeomList((GeomGroup *)(*a_poly));
		((GeomGroup *)(*a_poly))->Delete();
		*a_poly = poly;
	}
	return (*a_poly);
}

// add list or group a_poly to list a_master
inline GeomHdr *Add(GeomHdr **a_master, GeomHdr **a_poly)
{
	GeomHdr *gh = GroupToList(a_poly);
	if (gh) {
		for (; gh->next_geom; gh = gh->next_geom);	// find last object
		gh->next_geom = *a_master;
		*a_master = *a_poly;
		*a_poly = NULL;
	}
	return *a_master;
}
inline GeomHdr *DelCLList
(
	GeomHdr *gh	// first geom in list
)
{
	if (gh) {
		GeomHdr *prev;
		while (gh) {
			prev = gh;
			gh = gh->next_geom;
			prev->SetRoot(NULL);
			prev->Delete();
		}
	}
	return NULL;
}
inline GeomHdr *CopyToHead
(
	GeomHdr *gh,	// first geom in list
	GeomHdr *tail	// first geom in list
)
{
	if (gh) {
		//GeomHdr *g0 = SGU::CopyGeom(gh);
		//SGU::PolyCopy(*(glPoly *)gh, (glPoly *)g0);
		gh = GroupToList(&gh);
		GeomHdr *g0 = gh->Copy(NULL);
		GeomHdr *g = g0;
		for (gh = gh->next_geom; gh; gh = gh->next_geom) {
			//g->next_geom = glPoly::CreateTemp(0, gh->GetPointCount());
			g->next_geom = gh->Copy(NULL);
			g = g->next_geom;
			//SGU::PolyCopy(*(glPoly *)gh, (glPoly *)g);
		}
		g->next_geom = tail;
		tail = g0;
	}
	return tail;
}
inline GeomHdr *AddToHead
(
	GeomHdr *gh,	// first geom in list
	GeomHdr *tail	// first geom in list
)
{
	if (gh) {
		gh = GroupToList(&gh);
		if (tail) {
			GeomHdr *g = gh;
			while(g->next_geom) g = g->next_geom;
			g->next_geom = tail;
		}
		tail = gh;
	}
	return tail;
}
inline GeomHdr *SetOutlineRef(GeomHdr *gh)
{
	GL_Node *node = NULL;
	while (gh) {
		if (!gh->IsClockwise()) {
			node = (GL_Node *)gh;
		}
		gh->SetRoot(node);
		if (gh->next_geom == NULL) {
			break;
		}
		gh = gh->next_geom;
	}
	return gh;
}
inline void ClearRootNode(GeomHdr *gh)
{
	while (gh) {
		gh->SetRoot(NULL);
		gh = gh->next_geom;
	}
}

//~~*~~~~~~~~~~~~~ SAS_AREAS ~~~~~~~~~~~~~~~~~~~~~
class SYSGL_API stAreas
{
public:
	//stScan			*scan;
	glAreaData			*data;

	//GeomHdr		*first_geom;
	GeomHdr			ah;
	int				inf_ovl;
	
	GL_Array		*arr;
	int				id0;
	int				itr_num;
	
	stAreas();
	void	Clear();
	void	Delete();
	inline void	SetProc(glAreaData *a_data);
	int   UniteArea(stAreas *area1 = NULL, stAreas *area2 = NULL);
	int   SubtrArea(stAreas *area1, stAreas *area2);
	int   CrossArea(stAreas *area1, stAreas *area2);
	inline int	ExpandArea(stAreas *area1, long distance,  int upd = 1);
	int	ExpandByWidth(stAreas *area1, long distance = 0,  int upd = 1);
	inline void	LoadPolyList(GeomHdr *first, int ovl = 0);
	int	GetArea(stScan *scan, GL_Box *box, GL_Array *array);
	int	InitScan(stScan *scan, unsigned int scan_flag = SCAN_DB_LIGHT);
	inline int	InProc();
	int	AddArrayToX(GL_Array *arr, int id0, int num);
	int	SetFlags(GL_Array *array, int i0, int n, int num = 0, int dir = 1);
	int	SetFlags(GeomHdr *first, int num = 0, int dir = 1);
	inline int	SetFlags(int num = 0, int dir = 1);
protected:
	GeomHdr *CreateScanList(stScan *scan);
	GeomGroup *LinkVoids(GeomHdr *gh = NULL);
	int	UnLinkVoids(GeomHdr *gh = NULL);
};

inline void	stAreas::SetProc(glAreaData *a_data) {
	data = a_data;
}

inline void	stAreas::LoadPolyList(GeomHdr *first, int ovl)
{
	ah.next_geom = first;
	inf_ovl		= ovl;
	arr			= NULL;
	id0			= 0;
	itr_num		= 0;
}
inline int	stAreas::InProc()
{
	if (ah.next_geom) {
		return AddGeomListToX(ah.next_geom);
	} else if (arr) {
		return AddArrayToX(arr, id0, itr_num);
	}
	return 0;
}
inline int	stAreas::SetFlags(int num, int dir)
{
	if (arr) {
		return SetFlags(arr, id0, itr_num, num, dir);
	} else if (ah.next_geom) {
		return SetFlags(ah.next_geom, num, dir);
	}
	return 0;
}

class SYSGL_API IslandIter : public IterPolyGeom
{
public:
	GL_Node *node;
	inline int SetStart(GeomHdr *gh);
	inline int NextIsland(GeomHdr **gh);
};

inline int IslandIter::SetStart(GeomHdr *gh)
{
	node = gh->GetRoot();
	if (node && node->GetGeomType() == SGN_TYPE_GROUP) {
		SetParent((GeomHdr *)node);
		return TRUE;
	}
	return FALSE;
}

inline int IslandIter::NextIsland(GeomHdr **gh)
{
	if (GetNext()) {
		node = (GL_Node *)(GetPointPtr()->x);
		*gh = (GeomHdr *)node;
		return 1;
	}
	return 0;
}

inline int IsCross(GL_Box *box, stScan *scan)
{
	if (box && scan) {
		if (box->ll.x >= scan->ur.x || box->ur.x <= scan->ll.x) return 0;
		if (box->ll.y >= scan->ur.y || box->ur.y <= scan->ll.y) return 0;
		return 1;
	}
	return 0;
}
inline int ScanInBox(GL_Box *box, stScan *scan)
{
	if (box && scan) {
		if (box->ll.x <= scan->ll.x && scan->ur.x <= box->ur.x &&
			 box->ll.y <= scan->ll.y && scan->ur.y <= box->ur.y ) {
			return 1;
		}
	}
	return 0;
}
inline int IsCross(GL_Box *box, GL_Point *p1, GL_Point *p2)
{
	if (box) {
		if (p1->x < p2->x) {
			if (box->ll.x >= p2->x || box->ur.x <= p1->x) return 0;
		} else {
			if (box->ll.x >= p1->x || box->ur.x <= p2->x) return 0;
		}
		if (p1->y < p2->y) {
			if (box->ll.y >= p2->y || box->ur.y <= p1->y) return 0;
		} else {
			if (box->ll.y >= p1->y || box->ur.y <= p2->y) return 0;
		}
		return 1;
	}
	return 0;
}

inline void NormVectLen
(
	GL_DPoint	*a,
	double &d	// length
)
{
	if (a->y == 0.0) {
		if (a->x > 0.0){ d = a->x;  a->x = 1.0;}
		else				{ d = -a->x; a->x = -1.0; }
	} else if (a->x == 0.0) {
		if (a->y > 0.0){ d = a->y;  a->y = 1.0;}
		else				{ d = -a->y; a->y = -1.0; }
	} else if (a->x == a->y) {
		if (a->x > 0.0) {
			d = CSN45 * a->x;
			a->x = CSN45;
			a->y = CSN45;
		} else {
			d = -CSN45 * a->x;
			a->x = -CSN45;
			a->y = -CSN45;
		}
	} else if (a->x == -a->y) {
		if (a->x > 0.0) {
			d = CSN45 * a->x;
			a->x = CSN45;
			a->y = -CSN45;
		} else {
			d = CSN45 * a->y;
			a->x = -CSN45;
			a->y = CSN45;
		}
	} else {
		d = sqrt(a->x*a->x + a->y*a->y);
		if (d) {
			a->x /= d;
			a->y /= d;
		} else {
			a->x = 1.0;
			a->y = 0.0;
		}
	}
}
inline void NormVect
(
	GL_DPoint	*a
)
{
	double d;
	NormVectLen (a, d);
}

/////////// CLayer //////////////////////
class SYSGL_API DffLayer
{
public:
	int lyr_id;
	GeomHdr *hdr;
	GeomHdr *err;
	stScan		*scan;
	
	DffLayer();
	~DffLayer();
	inline void DelAll();
	inline void InitComLayer();
	inline void ClearComLayer();
	inline void ClearComErrors();
	inline void ClearScan();
	void Save(char *file_name, int append);
	GeomHdr *CopyList(GeomHdr *first);
	GeomHdr *CopyList(DffLayer *cl);
	GeomHdr *AddList(GeomHdr *first);
	GeomHdr *AddList(DffLayer *cl);
};
inline void DffLayer::InitComLayer()
{
	scan	= NULL;
	hdr	= NULL;
	err	= NULL;
};
inline void DffLayer::ClearComLayer()
{
	if  (hdr) hdr = DelPolyList(hdr);
	if  (err) err = DelPolyList(err);
	if (scan) {
		scan->kx = 0;	// no polylines 
		scan->Clear();
	}
}
inline void DffLayer::ClearComErrors()
{
	if  (err) err = DelPolyList(err);
}
inline void DffLayer::ClearScan()
{
	if (scan) {
		scan->kx = 0;	// Undo/redo???
		scan->Clear();
	}
}
inline void DffLayer::DelAll()
{
	if (err) err = DelPolyList(err);
	if (hdr) hdr = DelPolyList(hdr);
	if (scan) {
		scan->kx = 0;	// no polylines 
		delete scan;
		scan = NULL;
	}
}

/////////// IterPolyList //////////////////////
class SYSGL_API IterPolyList
{
public:
	GeomHdr *first;
	GeomHdr *poly;
	IterPolyList();
	IterPolyList(GeomHdr *gh);
	void SetParent(GeomHdr *gh);
	GeomHdr	*GetFirstPoly();
	GeomHdr	*GetNextPoly();
	inline GeomHdr	*GetCurrentPoly();
	void Save(char *filename);
	void GetGeomExtents(GL_Box &box, GeomHdr *gh = NULL);
protected:
	int GetGroupRange();
	PolyPoint *cpt;
	PolyPoint *lpt;
	GL_Node *node;
};
inline void Snap2(GL_Point *pt, GL_Point *p)
{
	if (pt->x - L2_EPS <= p->x && p->x <= pt->x + L2_EPS &&
		 pt->y - L2_EPS <= p->y && p->y <= pt->y + L2_EPS  ){
		p->x = pt->x;
      p->y = pt->y;
   }
}
inline void Snap2(GL_Point &pt, GL_Point &p)
{
	if (pt.x - L2_EPS <= p.x && p.x <= pt.x + L2_EPS &&
		 pt.y - L2_EPS <= p.y && p.y <= pt.y + L2_EPS  ){
		p.x = pt.x;
      p.y = pt.y;
   }
}

inline void Snap1(GL_Point *pt, GL_Point *p)
{
	if (pt->x - EPS_1 <= p->x && p->x <= pt->x + EPS_1 &&
		 pt->y - EPS_1 <= p->y && p->y <= pt->y + EPS_1  ){
		p->x = pt->x;
      p->y = pt->y;
   }
}
inline void Snap1(GL_Point &pt, GL_Point &p)
{
	if (pt.x - EPS_1 <= p.x && p.x <= pt.x + EPS_1 &&
		 pt.y - EPS_1 <= p.y && p.y <= pt.y + EPS_1  ){
		p.x = pt.x;
      p.y = pt.y;
   }
}
inline GeomHdr *IterPolyList::GetCurrentPoly() {return poly;}

inline int GetAreaId(const unsigned int flag, const int dir)
{
	if (flag & GL_WINDOW) {
		if (flag & GL_WIND_ABOVE) {
			return (dir > 0)? 5 : 6;
		} else {
			return (dir > 0)? 6 : 5;
		}
	} else if (flag & GL_SW3) {
		if (flag & GL_SW3_ABOVE) {
			if (flag & GL_SW3_BELOW) {
				return 0;
			} else {
				return (dir > 0)? 3 : 4;
			}
		} else {
			return (dir > 0)? 4 : 3;
		}
	} else if (flag & GL_SOFT) {
		if (flag & GL_SOFT_ABOVE) {
			if (flag & GL_SOFT_BELOW) {
				return 0;
			} else {
				return (dir > 0)? 1 : 2;
			}
		} else {
			return (dir > 0)? 2 : 1;
		}
	}
	return 0;
}

#endif	// __SAS_AREA_H__

