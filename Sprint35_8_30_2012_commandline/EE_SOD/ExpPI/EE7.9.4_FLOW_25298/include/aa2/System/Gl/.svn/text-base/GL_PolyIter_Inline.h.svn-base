/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:				SDB_PolyIter_Inline.H
// Date:				12-04-97
// Author:			Alexey Nagorny
// Description:	Inline header of special ODB iterator for Polyline.


#ifndef __SDB_POLYITER_INLINE_H__
#define __SDB_POLYITER_INLINE_H__

#include "GL_sgu.h"

#ifndef unexpected_msg
#define unexpected_msg "ERUNDA"
#endif

inline long GeomSize2Count(long a_size)
{
	//ASSERT_WRN (a_size%sizeof(PolyPoint)==0, "Size-Count missmatch");
	return a_size / sizeof(PolyPoint);
}

inline long GeomCount2Size(long a_count)
{
	return a_count * sizeof(PolyPoint);
}
//-------------------------------------------------------------------
inline	bool IterPolyGeom::GetNextId	(long	*a_id) const {
	if (p_cur && p_cur != p_last) {
		*a_id = (int) (p_cur - parent ->GetFirstPoint() + 1);
		return true;
	}
	return false;
}
inline	bool	IterPolyGeom::GetPrevId (long	*a_id) const {
	if (p_cur && p_cur != p_first) {
		*a_id = (int) (p_cur - parent ->GetFirstPoint() - 1);
		return true;
	}
	return false;
}
/*-----------------------------------------------------------------------------/
inline							IterPolyGeom::IterPolyGeom			(const GL_Node * a_hdr) 
{
	p_cur = NULL;
	if( (parent = (GeomHdr *) a_hdr)!=NULL)
	{
		p_first	= (PolyPoint *)(((parent -> GetFirstPoint())));
		p_last	= ((PolyPoint *)(((unsigned char *)(parent -> GetFirstPoint())) + GeomCount2Size(parent->data->GetUsedCount()))) - 1;
		if (parent -> data -> GetUsedCount())
			p_cur = p_first;
	}
}
inline							IterPolyGeom::IterPolyGeom			(const GL_Node & a_hdr) {
	parent = (GeomHdr *) &a_hdr;
	p_first	= (PolyPoint *)(parent -> GetFirstPoint());
	p_last	= ((PolyPoint *)(((unsigned char *)parent -> GetFirstPoint()) + GeomCount2Size(parent->data->GetUsedCount()))) - 1;
	if (parent -> data -> GetUsedCount())
		p_cur = p_first;
	else
		p_cur = NULL;
}
/-----------------------------------------------------------------------------*/

inline void	IterPolyGeom::SetParent (const GL_Node * a_hdr)
{
	parent = (GeomHdr*)a_hdr;
	p_cur = NULL;
	if (parent && parent->IsPoly() && parent->GetPointCount()) {
		p_first	= parent->GetFirstPoint();
		p_last	= p_first + parent->GetPointCount() - 1;	
		if (p_first <= p_last) p_cur = p_first;
	} else {
		p_last= NULL;
		p_first = p_last + 1;
	}
}


inline IterPolyGeom::IterPolyGeom (const GL_Node *a_hdr) 
{
	SetParent(a_hdr);
}
inline IterPolyGeom::IterPolyGeom (const GL_Node &a_hdr)
{
	SetParent((GL_Node *)&a_hdr);
}

inline 	GeomHdr *		IterPolyGeom::GetParent					() const {
	return parent;
}
inline 	bool			IterPolyGeom::IsValid						() const {
	return parent != NULL && (p_cur != NULL || IsCircle());
}
/// arc
inline 	bool			IterPolyGeom::HasArcs						() const {
	return (parent->hdr_flags & HDR_FLAG_ARC)!=0;
}
inline 	long			IterPolyGeom::GetArcCount					() const {
	return parent->GetArcCount();
}
inline	bool			IterPolyGeom::IsArc							() const {
	//return (p_cur<p_last && p_cur->IsArc());
	return (p_cur && p_cur->IsArc());
}
inline	void					IterPolyGeom::CreateArc					(const GL_DPoint & a_c, const long &a_r) {
	PolyPoint	*p_start_data = (PolyPoint *)(parent->GetFirstPoint());
	parent -> SetArcData(p_cur, true);
	p_cur = (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_cur - (unsigned char *)p_start_data));
	p_first	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_first - (unsigned char *)p_start_data));
	p_last	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_last - (unsigned char *)p_start_data));
	PolyArc *arc = p_cur->GetArc();
	if (arc) {
		arc->c = a_c;
		arc->r = a_r;
	} else {
		ASSERT_WRN(0,"CreateArc() error");
	}
}
inline	void					IterPolyGeom::DelArc						() {
	PolyPoint	*p_start_data = (PolyPoint *)(parent->GetFirstPoint());
	parent -> SetArcData(p_cur, false);
	p_cur = (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_cur - (unsigned char *)p_start_data));
	p_first	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_first - (unsigned char *)p_start_data));
	p_last	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_last - (unsigned char *)p_start_data));
};
inline	bool			IterPolyGeom::GetArcD						(GL_DPoint *a_c,long *a_r) const {
	//if (p_cur<p_last && p_cur->IsArc()) {}
	if (p_cur && p_cur->IsArc()) {
		PolyArc *arc = p_cur->GetArc();
		*a_c = arc->c;
		*a_r = arc->r;
		return true;
	} else {
		*a_r = 0;	// AZ
		return false;
	}
}
inline	void IterPolyGeom::SetArcD (const GL_DPoint & a_c, const long &a_r) {
	PolyArc *arc = p_cur->GetArc();
	arc->c = a_c;
	arc->r = a_r;
}

inline bool	IterPolyGeom::GetFirstId(long *a_id) const
{
	if (p_cur) {
		*a_id = (int)(p_first - parent->GetFirstPoint());
		return true;
	}
	*a_id = 0;
	return false;
}

inline bool IterPolyGeom::GetLastId(long *a_id) const
{
	if (p_cur) {
		*a_id = (int)(p_last - parent->GetFirstPoint());
		return true;
	}
	*a_id = 0;
	return false;
}

//------------------------------------------------------------------
inline	bool			IterPolyGeom::GetArc						(GL_Point *a_c,long *a_r) const {
	//if (p_cur<p_last && p_cur->IsArc()) {}
	if (p_cur && p_cur->IsArc()) {
		PolyArc *arc = p_cur->GetArc();
		a_c->x = LROUND(arc->c.x);
		a_c->y = LROUND(arc->c.y);
		*a_r = arc->r;
		return true;
	} else {
		*a_r = 0;	// AZ
		return false;
	}
}
inline	void					IterPolyGeom::SetArc						(const GL_Point & a_c, const long &a_r) {
	PolyArc *arc = p_cur->GetArc();
	arc->c.x = a_c.x;
	arc->c.y = a_c.y;
	arc->r = a_r;
}
//------------------------------------------------------------------
inline	const GL_DPoint	&IterPolyGeom::GetArcCenter				() const {
	return p_cur->GetArc()->c;
}
inline	bool			IterPolyGeom::GetArcCenter				(GL_DPoint	*a_c) const {
	//if (p_cur<p_last && p_cur->IsArc()) {}
	if (p_cur && p_cur->IsArc()) {
		*a_c = p_cur->GetArc()->c;
		return true;
	}
	return false;
}
inline	void					IterPolyGeom::SetArcCenter				(const GL_DPoint & a_c) {
	PolyArc *arc = p_cur->GetArc();
	arc->c = a_c;
}
inline	const long	&IterPolyGeom::GetArcRadius				() const {
	return p_cur->GetArc()->r;
}
inline	bool			IterPolyGeom::GetArcRadius				(long *a_r) const {
	//if (p_cur<p_last && p_cur->IsArc()) {}
	if (p_cur && p_cur->IsArc()) {
		*a_r = p_cur->GetArc()->r;
		return true;
	}
	return false;
}
inline	void					IterPolyGeom::SetArcRadius				(const long  &a_r) {
	PolyArc *arc = p_cur->GetArc();
	arc->r = a_r;
}
/// prev arc
inline	bool			IterPolyGeom::IsPrevArc					() const
{
		if (!p_cur || p_cur == p_first || p_cur > p_last)
			return false;
		return ((p_cur - 1) ->IsArc());
}
inline	void					IterPolyGeom::DelPrevArc					() {
	PolyPoint	*p_start_data = (PolyPoint *)(parent->GetFirstPoint());
	parent->SetArcData(p_cur - 1, false);
	p_cur = (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_cur - (unsigned char *)p_start_data));
	p_first	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_first - (unsigned char *)p_start_data));
	p_last	= (PolyPoint *)((unsigned char *)(parent->GetFirstPoint()) + ((unsigned char *)p_last - (unsigned char *)p_start_data));
};
inline	bool			IterPolyGeom::GetPrevArcD					(GL_DPoint *a_c,long *a_r) const {
	if (!IsPrevArc())
		return false;
	PolyArc *arc = (p_cur - 1)->GetArc();
	*a_c = arc->c;
	*a_r = arc->r;
	return true;
}
//--------------------------------------------------------------------------------------
inline	PolyArc*			IterPolyGeom::GetArcPtr					() {
	if (!IsArc())
		return NULL;
	return (p_cur)->GetArc();
}
inline	bool			IterPolyGeom::GetPrevArc					(GL_Point *a_c,long *a_r) const {
	if (IsPrevArc()) {
		PolyArc *arc = (p_cur - 1)->GetArc();
		//*a_c = arc->c;
		a_c->x = LROUND(arc->c.x);
		a_c->y = LROUND(arc->c.y);
		*a_r = arc->r;
		return true;
	} else {
		*a_r = 0;
		return false;
	}
}
//--------------------------------------------------------------------------------------
inline	const GL_DPoint	&IterPolyGeom::GetPrevArcCenter			() const {
	return (p_cur - 1)->GetArc()->c;
}
inline	const long	&IterPolyGeom::GetPrevArcRadius			() const {
	return (p_cur - 1)->GetArc()->r;
}
inline	GL_Point			*IterPolyGeom::GetPointPtr				() const {
	return	(p_cur)? &p_cur->point : NULL;
}
inline	GL_Point			*IterPolyGeom::GetPrevPointPtr			() const {
	if (p_cur && p_cur != p_first)
		return &(p_cur-1)->point;
	return NULL;
}
inline	GL_Point			*IterPolyGeom::GetNextPointPtr			() const {
	if (p_cur && p_cur != p_last)
		return &(p_cur+1)->point;
	return NULL;
}

inline	GL_Point	IterPolyGeom::GetPoint					() const {
		return p_cur->point;
}
inline	bool			IterPolyGeom::GetPoint						(GL_Point * a_p) const {
	if (p_cur) {
		*a_p = p_cur -> point;
		return true;
	}
	return false;
}
inline	GL_Point	IterPolyGeom::GetPrevPoint				() const {
	return (p_cur - 1) -> point;
}
inline	bool			IterPolyGeom::GetPrevPoint				(GL_Point * a_p) const {
	if (p_cur && p_cur != p_first) {
		*a_p = (p_cur - 1) -> point;
		return true;
	}
	return false;
}
inline	GL_Point	IterPolyGeom::GetNextPoint				() const {
	return (p_cur + 1) -> point;
}
inline	bool			IterPolyGeom::GetNextPoint				(GL_Point * a_p) const {
	if (p_cur && p_cur != p_last) {
		*a_p = (p_cur + 1) -> point;
		return true;
	}
	return false;
}
inline	void					IterPolyGeom::SetPoint						(const GL_Point & a_p) {
	p_cur -> point = a_p;
}
inline	void					IterPolyGeom::SetNextPoint				(const GL_Point & a_p) {
	(p_cur+1) -> point = a_p;
}
// checking type geometry
inline	bool			IterPolyGeom::IsCircle						() const {
	return parent && parent->GetGeomType() == SGN_TYPE_CIRCLE;
}
		// iteration
inline	const long		IterPolyGeom::GetCurentId					() const {
	return (long) (p_cur - (PolyPoint *)(parent -> GetFirstPoint()));
}
inline	bool			IterPolyGeom::GetFirst						() {
	if (p_last < p_first) {
		p_cur = 0;
		return false;
	} 
	p_cur = p_first;
	return true;
}
inline	bool			IterPolyGeom::GetFirstSeg					() {
	if (p_last <= p_first) {
		return false;
	}
	p_cur = p_first;
	return true;
}
inline	bool			IterPolyGeom::GetNext						() {
	if (p_cur && p_cur != p_last)	{
		p_cur ++;
		return true;
	}
	return false;
}

inline	bool			IterPolyGeom::GetNextSeg					() {
	if (p_cur) {
		p_cur ++;
		if (p_cur < p_last)		return true;
		p_cur--;
	}
	return false;
}
inline	bool			IterPolyGeom::GetPrevSeg					() {
	if (p_cur && p_cur != p_first) {
		p_cur --;
		return true;
	}
	return false;
}

inline	bool			IterPolyGeom::GetPrev						() {
	if (p_cur && p_cur != p_first) {
		p_cur --;
		return true;
	}
	return false;
}
inline	bool			IterPolyGeom::GetLast						() {
	if (p_cur) {
		p_cur = p_last;
		return true;
	}
	return false;
}

inline	bool			IterPolyGeom::GetLastSeg					() {
	if (p_cur && p_first != p_last) {
		p_cur = p_last - 1;
		return true;
	}
	return false;
}
inline	bool			IterPolyGeom::GoTo							(const long &a_crn,long a_shift) {
	static PolyPoint *p_goto;
	p_goto = (p_first + a_crn + a_shift);
	if (p_cur && p_goto <= p_last && p_goto >= p_first) {
		p_cur = p_goto;
		return true;
	}
	return false;
}
inline	bool			IterPolyGeom::IsLast						()	const {
	return p_cur == p_last;
}
inline	bool			IterPolyGeom::IsLastSeg					()	const {
	return p_cur + 1 == p_last;
}
inline	bool			IterPolyGeom::IsFirst						() const {
	return p_cur == p_first;
}
inline	bool			IterPolyGeom::IsFirstSeg					() const {
	return p_cur == p_first && p_first != p_last;
}

// insert
inline	void					IterPolyGeom::InsBefore					(const GL_Point	&a_p) {
	static	PolyPoint	p;
	p.point.x = a_p.x;
	p.point.y = a_p.y;
	p.arc = NULL;
	if (p_cur) {
		long	cur_id = GetCurentId ();
		long	first_id = p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> Insert (cur_id*sizeof(PolyPoint),sizeof (PolyPoint),&p);
 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
		p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id + 1;
		p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + cur_id + 1;
	}
	else {
		parent -> Add(&p,sizeof (PolyPoint));
 		p_cur = p_last = p_first	= (PolyPoint *)((((parent -> GetFirstPoint()))));
	}
}
inline	void					IterPolyGeom::InsAfterCrn					(const GL_Point	&a_p) 
{
	static	PolyPoint	p;
	p.point.x = a_p.x;
	p.point.y = a_p.y;
	p.arc = NULL;
	if (p_cur) {
		long	cur_id = GetCurentId();
		long	first_id = p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> Insert (MIN((long)((cur_id + 1)*sizeof(PolyPoint)), ((glPoly*)parent)->GetUsedSize()),sizeof (PolyPoint),&p);
 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
		p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id + 1;
		p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + cur_id;
	}
	else {
		parent -> Add(&p,sizeof (PolyPoint));
 		p_cur = p_last = p_first	= (PolyPoint *)((((parent -> GetFirstPoint()))));
	}
}

inline	void					IterPolyGeom::InsAfterSeg					(const GL_Point	&a_p) 
{
	GetNext();
	InsAfterCrn(a_p);
	GetPrev();
}
inline	void					IterPolyGeom::AddCrnToTail					(const GL_Point	&a_p) 
{	PolyPoint	p;
	p.point = a_p;
	p.arc = NULL;
	if (p_cur) {
		long	cur_id = GetCurentId ();
		long	first_id = p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> Add(&p,sizeof (PolyPoint));
 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
		p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id + 1;
		p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + cur_id;
	}
	else {
		parent -> Add(&p,sizeof (PolyPoint));
 		p_cur = p_last = p_first	= parent -> GetFirstPoint();
	}
}
inline void IterPolyGeom::AddArcCrnToTail(const GL_DPoint	&a_c,long	a_r,const GL_Point	&a_p) 
{
	//ASSERT_WRN (p_cur,"Cannot add arc to empty poly");
	if (p_cur) 
	{
		long	cur_id = GetCurentId ();
		long	first_id = p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> SetArcData(p_last, true);
 		p_first	= (PolyPoint *)(parent -> GetFirstPoint()) + first_id;
		p_last	= (PolyPoint *)(parent -> GetFirstPoint()) + last_id;
		p_cur = (PolyPoint *)(parent -> GetFirstPoint()) + cur_id;
		p_last->arc->c = a_c;
		p_last->arc->r = a_r;
	}
	AddCrnToTail(a_p);
}

		// delete
inline	void					IterPolyGeom::DelBefore					() {
	if (p_cur) {
		if (!IsFirst()) {
			long	first_id= p_first - (PolyPoint *)((parent -> GetFirstPoint()));
			long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
			long	prev_id = p_cur - (PolyPoint *)((parent -> GetFirstPoint())) - 1;
			parent -> Delete(prev_id,1);
	 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
			p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id - 1;
			p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + prev_id;
			return;
		}
	}
	//ASSERT_WRN(false,"Bad delete");
}

inline	void					IterPolyGeom::DelAfterCrn					() 
{
	if (p_cur && !IsLast()) {
		long	next_id = p_cur - (PolyPoint *)((parent -> GetFirstPoint()))+1;
		long	first_id= p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> Delete(next_id, 1);
 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
		p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id - 1;
		p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + next_id-1;
		return;
	}
	//ASSERT_WRN(false,"Bad delete");
}
inline	void					IterPolyGeom::DelCrn						() {
	if (p_cur) {
		static	long	cur_id;
		cur_id = GetCurentId ();
		long	first_id = p_first - (PolyPoint *)((parent -> GetFirstPoint()));
		long	last_id = p_last - (PolyPoint *)((parent -> GetFirstPoint()));
		parent -> Delete(cur_id, 1);
 		p_first	= (PolyPoint *)((parent -> GetFirstPoint())) + first_id;
		p_last	= (PolyPoint *)((parent -> GetFirstPoint())) + last_id - 1;
		p_cur = (PolyPoint *)((parent -> GetFirstPoint())) + cur_id;
		if (p_last < p_first)
			p_cur = NULL;
		else {
			if (p_cur > p_last) p_cur = p_last;
		}
		return;
	}
	else {
		//ASSERT_WRN(false,"Bad delete");
	}
};
// Get polyline direction - returns true if clockwise
inline bool IterPolyGeom::IsClockwise() const
{
	return ((glPoly *)parent)->IsClockwise();
}
inline 	bool		IterPolyGeom::IsClosed() const {
	//return (*(GL_Point *)p_first == *(GL_Point *)p_last);
    return IsCircle()? true : (p_first->point == p_last->point);
}
inline	const long	&IterPolyGeom::GetWidth () const {
	return parent->GetWidth();
}
inline	void			IterPolyGeom::SetWidth (const long &a_width) {
	parent->SetWidth(a_width);
}
inline	long	IterPolyGeom::GetPointCount				() const {
	return parent -> GetPointCount();
}

inline void IterPolyGeom::Invert(long from,long to) 
{
	if(parent->IsCircle())
	{
		((glCircle *)parent)->Invert();
	}
	else
	{
	PolyPoint	*p_start_data = (PolyPoint *)((parent->GetFirstPoint()));
	((glPoly *)parent)->Invert(from,to);
	p_cur = (PolyPoint *)((unsigned char *)((parent->GetFirstPoint())) + ((unsigned char *)p_cur - (unsigned char *)p_start_data));
	p_first	= (PolyPoint *)((unsigned char *)((parent->GetFirstPoint())) + ((unsigned char *)p_first - (unsigned char *)p_start_data));
	p_last	= (PolyPoint *)((unsigned char *)((parent->GetFirstPoint())) + ((unsigned char *)p_last - (unsigned char *)p_start_data));
}
}
inline	void			IterPolyGeom::GetCircleD(GL_DPoint	*a_c,long *a_r) const {
	*a_c = ((glCircle *)parent)->GetCenter();
	*a_r = ((glCircle *)parent)->GetRadius();
}
//-------------------------------------------------------------------------------------
inline	int	IterPolyGeom::GetCircle(GL_Point *a_c, long *a_r) const {
	if (IsCircle()) {
		*a_c = ((glCircle *)parent)->GetCenter();
		*a_r = ((glCircle *)parent)->GetRadius();
		return 1;
	}
	return 0;
}
//-------------------------------------------------------------------------------------
inline	const GL_DPoint	&IterPolyGeom::GetCircleCenterD() const {
	static GL_DPoint pnt;
	return pnt = ((glCircle *)parent)->GetCenter();
}
inline	const GL_Point	&IterPolyGeom::GetCircleCenter() const {
	return ((glCircle *)parent)->GetCenter();
}

inline	void			IterPolyGeom::SetCircleCenterD(const GL_DPoint & a_c) {
	((glCircle *)parent)->SetCenter(a_c);
}
inline	void			IterPolyGeom::SetCircleCenter(const GL_Point & a_c) {
	((glCircle *)parent)->SetCenter(a_c);
}

inline	long	IterPolyGeom::GetCircleRadius() const {
	return ((glCircle *)parent)->GetRadius();
}

inline	void			IterPolyGeom::SetCircleRadius(const long & a_r) {
	((glCircle *)parent)->SetRadius(a_r);
}

inline	void			IterPolyGeom::SetCircle(const GL_DPoint & a_c, const long & a_r) {
	((glCircle *)parent)->SetCenter(a_c);
	((glCircle *)parent)->SetRadius(a_r);
}

inline	bool	IterPolyGeom::IsFilled() const
{
	if (parent) return parent->IsFilled();
	return false;
}
inline int IterPolyGeom::GetNextDir() const
{
	//if (p_cur && p_cur < p_last)	return GL_ScanDir(*(GL_Point *) p_cur,*(GL_Point *) (p_cur+1));
	if (p_cur && p_cur < p_last)	return GL_ScanDir(p_cur->point,(p_cur+1)->point);
	return 0;
}
inline int IterPolyGeom::GetPrevDir() const
{
	//if (p_cur > p_first)	return GL_ScanDir(*(GL_Point *) (p_cur - 1),*(GL_Point *) p_cur);
	if (p_cur > p_first)	return GL_ScanDir((p_cur - 1)->point, p_cur->point);
	return 0;
}

inline void IterPolyGeom::GetExtents(GL_Box *ext_box, bool a_add_width) const
{
	SGU::GetExtents(parent, ext_box, a_add_width);
}
inline double	IterPolyGeom::GetArea() const
{
	return SGU::GetArea(parent);
}
inline double	IterPolyGeom::GetLength() const
{
	return SGU::GetLength(parent);
}

///////////////////////////////////////////////////////////////////////////////
/// IterGeom
inline	bool IterGeom::HasArcs() const {
	if(type_parent	== SGN_TYPE_POLY)
		return (((glPoly *)parent)->hdr_flags & HDR_FLAG_ARC)!=0;
	else if (type_parent	== SGN_TYPE_CIRCLE)
			return false;
	//ASSERT_WRN (false,unexpected_msg);	
	return false;
}
inline 	long IterGeom::GetArcCount() const {
	if(type_parent == SGN_TYPE_POLY) 
		return	IterPolyGeom::GetArcCount();
	if(type_parent == SGN_TYPE_CIRCLE)
		return 0;
	//ASSERT_WRN (false,unexpected_msg);
		return 0;
}
inline	void			IterGeom::CreateArc(const GL_DPoint & a_center, long a_radius) {
	if(type_parent == SGN_TYPE_POLY) {
		IterPolyGeom::CreateArc(a_center,a_radius);
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		((glCircle *)parent)->SetCircle	(a_center, a_radius);
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
}
inline	void			IterGeom::DelArc() {
	if(type_parent == SGN_TYPE_POLY) {
		IterPolyGeom::DelArc();
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Delete arc not support for circle yet"); 
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
}
inline	void			IterGeom::SetPoint(const GL_Point & a_point) {
	if(type_parent == SGN_TYPE_POLY) {
		IterPolyGeom::SetPoint(a_point);
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Change point for circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
}
inline void	IterGeom::InsAfterCrn(const GL_Point &a_p) {
	if(type_parent == SGN_TYPE_POLY)  {
		IterPolyGeom::InsAfterCrn(a_p); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Insert to circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
	return;
}
inline void	IterGeom::InsAfterSeg(const GL_Point &a_p)	{
	if(type_parent == SGN_TYPE_POLY)  {
		IterPolyGeom::InsAfterSeg(a_p); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Insert to circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
	return;
}
inline	void					IterGeom::AddCrnToTail					(const GL_Point	&a_p) {
	if(type_parent == SGN_TYPE_POLY)  {
		IterPolyGeom::AddCrnToTail(a_p); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Add to circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
	return;
}
inline	void					IterGeom::AddArcCrnToTail					(const GL_Point	&a_c,long	a_r,const GL_Point	&a_p) {
	if(type_parent == SGN_TYPE_POLY)  {
		IterPolyGeom::AddArcCrnToTail(a_c,a_r,a_p); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Add to circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
	return;
}
inline void IterGeom::InsBefore(const GL_Point &a_p) {
	if(type_parent == SGN_TYPE_POLY)  {
		IterPolyGeom::InsBefore(a_p); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) {
		//ASSERT_WRN (false,"Insert to circle");
		return;
	}
	//ASSERT_WRN (false,unexpected_msg);
	return;
}
// Get polyline direction - returns true if clockwise
inline bool IterGeom::IsClockwise() const {
	if(type_parent == SGN_TYPE_POLY)  
		return IterPolyGeom::IsClockwise(); 
	//ASSERT_WRN (false,unexpected_msg);
	return false;
}
inline 	bool		IterGeom::IsClosed() const {
	if(type_parent == SGN_TYPE_POLY)  {
		return IterPolyGeom::IsClosed(); 
	}
	if(type_parent == SGN_TYPE_CIRCLE) 
		return true;
	//ASSERT_WRN (false,unexpected_msg);
	return false;
}
//$$ Remarks:
//Split all Arc segments from current to the last to several monotonous arc segments.
// So, if you call GetFirst(), then MakeAllMonoArcs(), you will split all arcs of the object.
//$$ Return value:
//Number spliting (other words number created new arc segments).
inline 	void	IterGeom::MakeAllMonoArcs () {
	if(type_parent == SGN_TYPE_POLY) {
		IterPolyGeom::MakeAllMonoArcs(); 
		return;
	}
	if(type_parent == SGN_TYPE_CIRCLE) 
		return;
	//ASSERT_WRN (false,unexpected_msg);
	return;
}

//$$ Class: IterPolyCross implem.

inline	IterPolyCross::IterPolyCross() : IterPolyGeom()
{
}
inline	IterPolyCross & IterPolyCross::operator=(const IterPolyGeom & a_iter) {
	SetParent(a_iter.parent);
	p_cur = a_iter.p_cur;
	return *this;
}
inline	IterPolyCross & IterPolyCross::operator=(const IterGeom & a_iter) {
	SetParent(a_iter.parent);
	p_cur = a_iter.p_cur;
	return *this;
}
inline	IterPolyCross & IterPolyCross::operator=(const IterPolyCross & a_iter) {
	SetParent(a_iter.parent);
	p_first = a_iter.p_first;
	p_cur = a_iter.p_cur;
	p_last = a_iter.p_last;
	return *this;
}
inline	bool		IterPolyCross::IsStartArc() {
	return IterPolyGeom::IsArc();
}
inline	bool		IterPolyCross::IsEndArc() {
	PolyPoint	*prev_last = p_last-1;
	if (p_first && prev_last >= p_first)
		return (prev_last->IsArc());
	return false;
}
inline	void IterPolyCross::SetBegId(int a_id)
{ 
	if (parent && parent->IsPoly() && parent->GetPointCount()) {
		p_first	= parent->GetFirstPoint() ? parent->GetFirstPoint() + a_id : NULL;
		if (p_first <= p_last) {
			if(p_cur < p_first) {
				p_cur = p_first;
			}
		} else {
			p_cur = NULL;
		}
	} else {
		SetParent(parent);
	}
}
inline	void IterPolyCross::SetEndId(int a_id)
{ 
	if (parent && parent->IsPoly() && parent->GetPointCount()) {
		p_last = parent->GetFirstPoint();
		if(p_last) {
			if (a_id == -1) {
				p_last += parent->GetPointCount() - 1;
			} else {
				p_last	+=  a_id;
			}
		}
		if (p_first <= p_last) {
			if(p_cur > p_last) {
				p_cur = p_last;
			} else if (p_cur == NULL) {
				p_cur = p_first;
			}
		} else {
			p_cur = NULL;
		}
	} else {
		SetParent(parent);
	}
}
inline	int				IterPolyCross::GetBegId() const { 
	return p_first	- parent->GetFirstPoint();
}
inline	int				IterPolyCross::GetEndId()	const { 
	return p_last	- parent->GetFirstPoint();
}


#endif  /// __SDB_POLYITER_INLINE_H__

//
// End of file
//

