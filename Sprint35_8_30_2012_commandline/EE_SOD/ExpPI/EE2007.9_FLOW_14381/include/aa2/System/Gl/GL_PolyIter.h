#ifndef __GL_POLYITER_H__
#define __GL_POLYITER_H__

//#include "GL_const.h"
#include "GL_poly.h"
//----------------------------------------------------------------------------------
class IterGeom;
//----------------------------------------------------------------------------------
//$$ Class: IterPolyGeom
class SYSGL_API	IterPolyGeom
{
public:		// data
	GeomHdr				*parent;
	PolyPoint			*p_first,	*p_last, *p_cur;
public:		// constructors
	inline					IterPolyGeom			(const GL_Node * a_hdr=NULL);
	inline					IterPolyGeom			(const GL_Node & a_hdr);
						IterPolyGeom			(const IterGeom &);
public:		// methods
	inline void				SetParent				(const GL_Node * a_hdr);
	inline GeomHdr *			GetParent				() const;
public:		// validation
	inline bool				IsValid					() const;
	inline bool				IsFilled				() const;	// AZ
public:		// arc
	inline bool				IsArc					() const;
	inline bool				HasArcs					() const;
	inline long				GetArcCount				() const;
	inline void				CreateArc				(const GL_DPoint & a_c, const long &a_r);
	inline void				DelArc					();
	inline bool				GetArc					(GL_Point *a_c,long *a_r) const;
	inline bool				GetArcD					(GL_DPoint *a_c,long *a_r) const;
	inline void				SetArc					(const GL_Point & a_c, const long &a_r);
	inline void				SetArcD					(const GL_DPoint & a_c, const long &a_r);
	inline const GL_DPoint			&GetArcCenter				() const;
	inline bool				GetArcCenter				(GL_DPoint	*) const;
	inline void				SetArcCenter				(const GL_DPoint & a_c);
	inline const long			&GetArcRadius				() const;
	inline bool				GetArcRadius				(long *) const;
	inline void				SetArcRadius				(const long  &a_r);
	inline PolyArc				*GetArcPtr();
public:		// prev arc
	inline bool				IsPrevArc				() const;
	inline void				DelPrevArc				();
	inline bool				GetPrevArc				(GL_Point *a_c,long *a_r) const;
	inline bool				GetPrevArcD				(GL_DPoint *a_c,long *a_r) const;
	inline const GL_DPoint			&GetPrevArcCenter			() const;
	inline const long			&GetPrevArcRadius			() const;
public:		
	inline GL_Point				*GetPointPtr				() const;
	inline GL_Point				*GetPrevPointPtr			() const;
	inline GL_Point				*GetNextPointPtr			() const;
	inline GL_Point				GetPoint				() const;
	inline bool				GetPoint				(GL_Point * a_p) const;
	inline GL_Point				GetPrevPoint				() const;
	inline bool				GetPrevPoint				(GL_Point * a_p) const;
	inline GL_Point				GetNextPoint				() const;
	inline bool				GetNextPoint				(GL_Point * a_p) const;
	inline int				GetNextDir				() const;	// AZ
	inline int				GetPrevDir				() const;	// AZ
	inline bool				GetFirstId				(long *a_id) const;
	inline bool				GetLastId				(long *a_id) const;
public:		// polyline
	inline void				SetPoint				(const GL_Point & a_p);
	inline void				SetNextPoint				(const GL_Point & a_p);
public:		// checking type geometry
	inline bool				IsCircle				() const;
public:		// circle
	inline int				GetCircle				(GL_Point *a_c,long *a_r) const;
	inline void				GetCircleD				(GL_DPoint *a_c,long *a_r) const;
	inline const GL_Point			&GetCircleCenter			() const;
	inline const GL_DPoint			&GetCircleCenterD			() const;
	inline void				SetCircleCenter				(const GL_Point & a_c);
	inline void				SetCircleCenterD			(const GL_DPoint & a_c);
	inline void				SetCircleCenter				(const GL_DPoint & a_c);
	inline long				GetCircleRadius				() const;
	inline void				SetCircleRadius				(const long & a_r);
	inline void				SetCircle				(const GL_DPoint & a_c, const long & a_r);
public:		// iteration
	inline const long			GetCurentId				() const;
	inline bool				GetFirst				();
	inline bool				GetNext					();
	inline bool				GetPrev					();
	inline bool				GetLast					();
	inline bool				IsLast					() const;
	inline bool				IsFirst					() const;
	inline bool				GoTo					(const long &a_crn,long shift=0);

	inline bool				GetFirstSeg				();
	inline bool				GetNextSeg				();
	inline bool				GetPrevSeg				();
	inline bool				GetLastSeg				();
	bool					GoToSeg					(const long &a_crn,long shift=0);
	inline bool				IsLastSeg				()	const;
	inline bool				IsFirstSeg					() const;

public:		// insert
	inline void				InsBefore					(const GL_Point	&a_p);
	inline void				InsAfterCrn					(const GL_Point	&a_p);
	inline void				InsAfterSeg					(const GL_Point	&a_p);
	void				AddCrnToTail				(const GL_Point	&a_p);
	inline void				AddArcCrnToTail				(const GL_DPoint	&a_c,long	a_r,const GL_Point	&a_p);

public:		// Group insert
	void					InsAfterCrn					(IterPolyGeom	*i,long	to = -1,bool	move_mode = false);
	void					AddToTail					(IterPolyGeom	*i,long	to = -1,bool	move_mode = false);

public:		// delete
	inline void				DelBefore					();
	inline void				DelAfterCrn					();
	inline void				DelCrn						();
public:		// flags
	inline bool				IsClockwise					() const;
	inline bool				IsClosed						() const;
	inline void					Invert						(long from = 0,long to = -1);

public:		// width
	inline const long			&GetWidth					() const;
	inline void				SetWidth						(const long &a_width);
	inline long				GetPointCount				() const;
public:		// split mono arc
	int					MakeMonoArcs				();
	void				MakeAllMonoArcs			();
	inline bool				GetNextId (long	*a_id) const;
	inline bool				GetPrevId (long	*a_id) const;
				// sgu
	inline void					GetExtents(GL_Box *ext_box, bool a_add_width = true)  const;
	inline double					GetArea() const;
	inline double					GetLength() const;
};

//----------------------------------------------------------------------------------
//$$ Class: IterGeom
class SYSGL_API	IterGeom : public	IterPolyGeom
{
public:		// data
	short					type_parent;				
public:		// arc
	inline bool				HasArcs						() const;
	inline long				GetArcCount					() const;
	inline void				CreateArc					(const GL_DPoint & a_c, long a_r);
	inline void				DelArc						();
public:		// polyline
	inline void				SetPoint					(const GL_Point & a_p);
public:		// insert
	inline void				InsBefore					(const GL_Point	&a_p);
	inline void				InsAfterCrn					(const GL_Point	&a_p);
	inline void				InsAfterSeg					(const GL_Point	&a_p);
	inline void				AddCrnToTail				(const GL_Point	&a_p);
	inline void				AddArcCrnToTail			(const GL_Point	&a_c,long	a_r,const GL_Point	&a_p);
public:		// flags
	inline bool				IsClockwise					() const;
	inline bool				IsClosed					() const;
public:		// utilities

public:		// split mono arc
	int					MakeMonoArcs				();
	inline void				MakeAllMonoArcs			();
};
//----------------------------------------------------------------------------------
//$$ Class: IterPolyCross
///class IterPolyCross: public IterPolyGeom
class SYSGL_API IterPolyCross: public IterPolyGeom
{
public:	// constructors
	inline IterPolyCross();

public:	// operators
	inline IterPolyCross	&operator=	(const IterPolyGeom & a_iter);
	inline IterPolyCross	&operator=	(const IterGeom & a_iter);
	inline IterPolyCross 	&operator=	(const IterPolyCross & a_iter);

public:
	// selector
	inline bool				IsStartArc					();
	inline bool				IsEndArc					();
	inline void				SetBegId					(int a_id = NULL);
	inline void				SetEndId					(int a_id = -1);
	inline int				GetBegId					() const;
	inline int				GetEndId					() const;
};

//----------------------------------------------------------------------------------
// IterGeomGroup	object for  manipulation with geom group.
// It inherit IterPolyGeom. So, can work with polyline like as
// sample group. Also each curent child geom can be Activate.
// The following data help work with group geom:
//	GeomGroup			*top_group; - pointer to top group. No parent can be get from top_group.
//	GeomGroup			*group; - pointer to curent level hierarchy.
//	GroupData			*i_first,*i_last,*i_cur - pointers in first,last,curent child;
//
class	SYSGL_API IterGeomGroup : public	IterPolyGeom
{
public:
							IterGeomGroup			();
							IterGeomGroup			(const GL_Node * a_hdr);
							IterGeomGroup			(const GL_Node & a_hdr);

	GL_Node		*GetCurentGeom				()	const;
	GeomGroup		*GetCurentChild			()	const;
	bool					GetFirstGroup				();
	bool					GetNextGroup				();
	bool					GetGlobalFirstGroup		();
	bool					GetGlobalNextGroup		();
	bool					SetGroup						(GeomGroup *a_group, int a_group_id = 0);
	void					AddGroupToTail				(GL_Node *a_poly, GeomGroup *a_child = NULL, void	*a_misc = NULL);

private:
	void					SetTopGroup					(GeomGroup *a_top_group);
	void					ActivateCurentGeom		();
	bool					GetFirstChildGroup		();
	bool					GetParentGroup				();	
	bool					GetGlobalPrevGroup		();
	bool					GetPrevGroup				();
	bool					GetLastChildGroup			();
	bool					IsValid						() const; 
	bool					IsTopCorrect				()	const; 

	GeomGroup		*top_group,	*group;
	GroupData		*i_first, *i_last, *i_cur;
};

class PieIter
{
public:
	int i0;
	int il;
	int is_sel;
	SelPie *sp;
	IterPolyCross itr;
public:
	IterPolyCross *GetFirstIter(IterPolyCross *inp);
	IterPolyCross *GetNextIter();
};

#endif //__GL_POLYITER_H__

