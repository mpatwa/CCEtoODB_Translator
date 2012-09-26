/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// Name:				SDB_Poly_Inline.h
// Date:				12-05-96
// Author:			Alexey Nagorny
// Description:	System Database Polyline inline definitions


#ifndef __GL_POLY_INLINE_H__
#define __GL_POLY_INLINE_H__

/////////////////////////////////////////////////////////////////////
// PolyPoint implementation
/////////////////////////////////////////////////////////////////////
inline PolyPoint::PolyPoint( GL_Point *a_point, PolyArc *a_arc) 
{
	if(a_point)
	{
		point=*a_point;
	}
	else
	{
		point.x = point.y = 0;
	}
	arc = a_arc;
}
/////////////////////////////////////////////////////////////////////
// PolyArc implementation
/////////////////////////////////////////////////////////////////////

inline PolyArc::PolyArc(GL_DPoint *a_c, long a_r) 
{
//dbg	ASSERT_WRN (false,"DBG, don't use directly");
	if(a_c)
	{
		c.x = a_c->x;
		c.y = a_c->y;
	}
	else
	{
		c.x = 0;
		c.y = 0;
	}
	r = a_r;
}
/////////////////////////////////////////////////////////////////////
// GeomData implementation
/////////////////////////////////////////////////////////////////////

inline short GeomHdr::GetType() const 
{
	return (short)(flags & GL_TYPE_MASK);
}

inline bool GeomHdr::IsCircle() const 
{
	return flags & GL_CIRCLE ? true : false;
}
inline bool GeomHdr::IsPoly() const 
{
	return flags & GL_POLY ? true : false;
}

inline void GeomHdr::SetType
(
	short a_type
) 
{
	flags &= ~GL_TYPE_MASK;
	if (a_type == SGN_TYPE_POLY) {
		flags |= GL_POLY;
	} else if(a_type == SGN_TYPE_CIRCLE) {
		flags |= GL_CIRCLE;
	}
}

inline bool	GeomHdr::HasPoints()
{
	return IsPoly() && ((glPoly*)this)->GetUsedSize();
}

/////////////////////////////////////////////////////////////////////
// GeomHdr implementation
/////////////////////////////////////////////////////////////////////

inline PolyArc * GeomHdr::GetArc
(
	long a_crn
) const 
{
	return GetPoint(a_crn)->GetArc();
}

inline long GeomHdr::GetArcCount() const 
{
	if ((GetGeomType() == SGN_TYPE_CIRCLE) || !HasArcs()) {
		return 0;
	}
	long		id, cnt, pnt_cnt = GetPointCount();
	PolyPoint	*pnt = (PolyPoint *)(GetFirstPoint());
	for (id=cnt=0; id < pnt_cnt; id++, pnt++) {
		if (pnt->IsArc()) {
			cnt ++;
		}
	}
	return cnt;
}

inline PolyPoint * GeomHdr::GetFirstPoint() const 
{
	return (PolyPoint *)(((glPoly*)this)->GetFirstPoint0());
}

inline PolyPoint * GeomHdr::GetLastPoint() const 
{
	return &((PolyPoint *)(GetFirstPoint()))[GetPointCount() - 1];
}

inline PolyPoint * GeomHdr::GetPoint
(
	long a_crn
) const 
{
	return &((PolyPoint *)(GetFirstPoint()))[a_crn];
}

inline long GeomHdr::GetPointCount() const 
{
	if(IsPoly()) return ((glPoly*)this)->GetUsedCount();
	else return 0;
}

inline const long & GeomHdr::GetWidth() const
{
	return width;
}

inline bool GeomHdr::HasArcs() const 
{
	return hdr_flags & HDR_FLAG_ARC ? true : false;
}

inline bool GeomHdr::IsClockwise() const 
{
	return hdr_flags & HDR_FLAG_CLOCKWISE ? true : false;
}

inline bool GeomHdr::IsFilled() const 
{
	return hdr_flags & HDR_FLAG_FILLED ? true : false;
}

inline void GeomHdr::SetClockwiseFlag
(
	bool a_flag
) 
{
	hdr_flags = a_flag ? (hdr_flags | HDR_FLAG_CLOCKWISE) : (hdr_flags & ~HDR_FLAG_CLOCKWISE);
}

inline void GeomHdr::SetFilledFlag
(
	bool a_flag
) 
{
	hdr_flags = a_flag ? (hdr_flags | HDR_FLAG_FILLED) : (hdr_flags & ~HDR_FLAG_FILLED);
}

/////////////////////////////////////////////////////////////////////
// glPoly implementation
/////////////////////////////////////////////////////////////////////

inline void glPoly::AddCorner
(
	const GL_Point & a_p
) 
{
	PolyPoint	p;
	p.point = a_p;
	p.arc = NULL;
	Add(&p, sizeof(PolyPoint));
}

inline void glPoly::AddCornerD
(
	const GL_Point &a_pnt,
	const GL_DPoint &a_center,
	const long		 &a_radius
)
{
	long id;
	AddCorner(a_pnt);
	GetLastId(&id);
	InsertArcD(id, a_center, a_radius);
}

inline void glPoly::AddCorner
(
	const GL_Point &a_pnt,
	const GL_Point &a_center,
	const long &a_radius
) 
{
	long id;
	AddCorner(a_pnt);
	GetLastId(&id);
	InsertArc(id, a_center, a_radius);
}

inline void glPoly::DeleteArc
(
	long a_id
) 
{
	SetArcData(a_id, false);
}

inline void glPoly::DeleteCorner
(
	long a_id
) 
{
	Delete(a_id, 1);
}
inline PolyArc * glPoly::GetArcPtr
(
	long a_id
)
{
	return (PolyArc *) GetArc(a_id);
}

inline GL_Point glPoly::GetCorner
(
	long a_id
)  
{
	return GetPoint(a_id)->point;
}

inline bool glPoly::GetFirstId
(
	long *a_id
)  
{
	if (GetPointCount() == 0) {
		return false;
	}
	*a_id	= 0;
	return true;
}

inline bool glPoly::GetLastId
(
	long * a_id
) 
{
	if (GetUsedSize() == 0) {
		return false;
	}
	*a_id = GetPointCount() - 1;
	return true;
}
inline bool glPoly::GetNextId
(
	long * a_id
)
{
	*a_id +=1;
	if ((long)(*a_id) >= (long) GetUsedCount()) {
		*a_id -= 1;
		return false;
	}
	return true;
}

inline bool glPoly::GetPrevId
(
	long *a_id
)  
{
	*a_id -= 1;
	if (*a_id < 0) {
		*a_id += 1;
		return false;
	}
	return true;
}
inline void glPoly::InsertArcD
(
	long a_id, 
	const GL_DPoint & a_c,
	long a_r
) 
{
	PolyArc * arc = CreateArc();
	arc->c=a_c;
	arc->r=a_r;
	GetFirstPoint()[a_id].arc = arc;
}

inline void glPoly::InsertArc
(
	long a_id, 
	const GL_Point & a_c,
	long a_r
) 
{
	PolyArc * arc = CreateArc();
	arc->c=a_c;
	arc->r=a_r;
	GetFirstPoint()[a_id].arc = arc;
}

inline void glPoly::InsertCorner
(
	long a_id, 
	const GL_Point & a_p
) 
{
	PolyPoint	p;
	p.point = a_p;
	p.arc = NULL;
	Insert (a_id*sizeof (PolyPoint),sizeof (PolyPoint),&p);
}

// retruns true if polyline is closed (first and last points are the same)
inline bool glPoly::IsClosed() 
{
	if (GetUsedCount() < 3) {
		return false;
	}
	//return ((GetFirstPoint())->point == *(GL_Point *)((unsigned char *)(GetFirstPoint()) + GeomCount2Size(data->GetUsedCount() - 1))) ? true : false;
	return (GetFirstPoint()->point == GetLastPoint()->point)? true : false;
}

inline bool glPoly::IsFirstId
(
	const long & a_id
)  
{
	return (GetUsedSize() != 0 && a_id == 0);
}

inline bool glPoly::IsLastId
(
	const long &a_id
)  
{
	return (long) (a_id)  == (long) (GetUsedCount() - 1);
}

inline bool glPoly::IsValidId
(
	long a_id
)  
{
	return (a_id >= 0 && (long) (a_id) < (long) GetUsedCount());
}
//-----------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////
// GeomGroup implementation
/////////////////////////////////////////////////////////////////////
// Empty constructor of GeomGroup
// Returns pointer to the parent data structure for refer to parent geom group
inline GroupParentData*	GeomGroup::GetParentDataPtr		()	 
{
	return reinterpret_cast<GroupParentData *>(GetFirstPoint()+1);
};

// Returns count of children in group.
inline int					GeomGroup::GetItmCount				()  
{
	return GetPointCount() - 2; //??? 1;
};
// Returns pointer to the first "children" data structure for refer to child groups & its geom.
inline GroupData	*		GeomGroup::GetItmDataPtr			()	 
{
	return reinterpret_cast<GroupData *>(GetParentDataPtr() + 1);
};
// Add new children reference & it geom in group to the end of group.
inline	void					GeomGroup::AddItm						(GroupData a_itm) 
{
	InsertItm (GetItmCount(),a_itm);
};

inline void RemFromList(GeomHdr *n)
{
	if (n) {
		if(n->next_geom) n->next_geom->prev_geom = n->prev_geom;
		if (n->prev_geom) n->prev_geom->next_geom = n->next_geom;
		n->next_geom = n->prev_geom = NULL;
	}
}
inline void InsToNext(GeomHdr *p, GeomHdr *n)
{
	if (p && n && p != n && p->next_geom != n) {
		n->prev_geom = p;
		n->next_geom = p->next_geom;
		p->next_geom = n;
		if (n->next_geom) n->next_geom->prev_geom = n;
	}
}
inline void RemInsToNext(GeomHdr *p, GeomHdr *n)
{
	if (p && n && p != n && p->next_geom != n) {
		if(n->next_geom) n->next_geom->prev_geom = n->prev_geom;
		if (n->prev_geom) n->prev_geom->next_geom = n->next_geom;
		n->prev_geom = p;
		n->next_geom = p->next_geom;
		p->next_geom = n;
		if (n->next_geom) n->next_geom->prev_geom = n;
	}
}
inline void InsToPrev(GeomHdr *p, GeomHdr *n)
{
	if (p && n && p != n && p->prev_geom != n) {
		p->next_geom = n;
		p->prev_geom = n->prev_geom;
		n->prev_geom = p;
		if (p->prev_geom) p->prev_geom->next_geom = p;
	}
}
inline void RemInsToPrev(GeomHdr *p, GeomHdr *n)
{
	if (p && n && p != n && p->prev_geom != n) {
		if(n->next_geom) n->next_geom->prev_geom = n->prev_geom;
		if (n->prev_geom) n->prev_geom->next_geom = n->next_geom;
		p->next_geom = n;
		p->prev_geom = n->prev_geom;
		n->prev_geom = p;
		if (p->prev_geom) p->prev_geom->next_geom = p;
	}
}

#endif	// __GL_POLY_INLINE_H__

//
// End of file
//

