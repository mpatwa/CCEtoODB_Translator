#ifndef __GL_POLY_H__
#define __GL_POLY_H__

#include <stdio.h>
#include <stdlib.h>
//#include "GL_vec.h"
#include "GL_GeomIter.h"
#include "GL_Geom.h"
#include "GL_Global.h"
#include "GL_qlist.h"

// reallocation threshold
#define POLY_ALLOC_FACTOR			0			// number of additional points allocated for polyline

class glPoly;
class glCircle;
class GeomGroup;
class AreaVtx;
class GeomHdr;
class IterAlgPoly;

//~~*~~~~~~~~~~ AreaSect ~~~~~~~~~~~~~~~~~~~~~~~~
const unsigned long ONLY_FIRST_BIT = 0x00000001; //0x80000000;  HDR_FLAG_FREE_MASK
const unsigned long CLEAR_FIRST_BIT = ~ONLY_FIRST_BIT;

class SYSGL_API AreaSect
{
public:
	AreaSect 		*next;				// own poly at the right hand from sector
	unsigned long	vtx;				// own vertex
										// poly - next (from own) poly around vtx in the counterclockwise order
	AreaSect();
	inline void		SetVertex(AreaVtx *v);
	inline void 	SetVertex(AreaVtx *v, int id);
	inline void 	SetNextSector(AreaSect *next_sector);
	
	inline AreaVtx	*GetVertex();
	inline AreaVtx	*GetFrontVertex();
	inline AreaSect	*GetFrontSector();
	inline int		GetOwnIndex();
	inline GeomHdr	*GetOwnHdr();

	AreaSect		*GetPrevSector();
	inline AreaSect	*GetNextSector();
	inline AreaSect	*GetAfterSector();
	inline GeomHdr	*GetNextPoly();
	inline int		GetDFP(AreaVtx **f, GeomHdr **p);
	inline GeomHdr	*GetPF(AreaVtx **f);
	inline GeomHdr	*GetPFD(AreaVtx **f, int *id);
	inline GeomHdr	*GetPD(int *id);
	inline int		GetDir();
	
	inline AreaSect	*GetNext(GeomHdr **p, AreaVtx **v);
	inline AreaSect	*GetNext(GeomHdr **p);
	inline AreaSect	*GetNext(AreaVtx **v);
	AreaSect		*IsInCountour(GeomHdr *p);
	int				IsIntLoop(char *fname = NULL);
};

//~~*~~~~~~~~~~ PolyArc ~~~~~~~~~~~~~~~~~~~~~~~~
long	SYSGL_API GeomSize2Count(long a_size);
long	SYSGL_API GeomCount2Size(long a_count);

class SYSGL_API	PolyArc 
{
public:
	GL_DPoint	c;
	long		r;

public:
	inline		PolyArc	(GL_DPoint *a_c=NULL, long a_r=0);
};
//~~*~~~~~~~~~~ PolyPoint ~~~~~~~~~~~~~~~~~~~~~~~~
class SYSGL_API PolyPoint
{
public:
	GL_Point	point;
	PolyArc		*arc;
public:
	inline		PolyPoint(GL_Point	*a_point=NULL, PolyArc *a_arc=NULL);

	inline bool	IsArc()			 	{ return arc!=0;};
	inline PolyArc *GetArc() 		{ return arc;};
};

//~~*~~~~~~~~~~ GeomHdr ~~~~~~~~~~~~~~~~~~~~~~~~
// GeomHdr - geometry header. Contains pointer to geometry  data,
// pointers to next and previous geometry header, etc
class SYSGL_API	GeomHdr	: public	GL_Node 
{
public:
#ifdef GL_MEM_DEBUG
	int		gh_id;
	GeomHdr *gh_next, *gh_prev;	// debug
#endif

	GeomHdr			*prev_geom;
	GeomHdr			*next_geom;
	unsigned long	hdr_flags;

	long			width;			// polyline width
	unsigned long	flags;			// geometry data flags
	unsigned long	clip_mask;		// mask used by clipping
	AreaSect		sector[2];		// should be the last in GeomHdr

public:
					GeomHdr			();
					~GeomHdr		();

	static GeomHdr	*Create			(long a_pnt_count=0, long  a_arc_count=0, short a_type = SGN_TYPE_POLY );
	void			Delete			();
	void			Delete			(long, long);
	void			InitHdr			(short a_type);
	void			ClearHdr		();

	inline PolyPoint *GetFirstPoint	() const;
	inline PolyPoint *GetLastPoint	() const;
	inline PolyPoint *GetPoint		(long a_crn) const;
	inline long		GetPointCount	() const;
	inline PolyArc	*GetArc			(long a_crn) const;
	inline long		GetArcCount		() const;
	inline bool		HasPoints		();
	inline bool		HasArcs			() const;
	void			Insert			(long, long, const void *);
	void			Add				( const void * a_data,long a_size);
	GeomHdr			*Copy			(GeomHdr *a_dst=NULL);
	inline bool		IsClockwise		() const;
	inline bool		IsFilled		() const;
	inline unsigned long GetGraFlag	() const			{	return hdr_flags;	};			// TBD
	inline const long &GetWidth		() const;
	inline void		SetWidth		(long a_width)		{ width = a_width; };
	inline void		SetClockwiseFlag(bool a_flag);
	inline void		SetFilledFlag	(bool a_flag);
	void			SetArcData		(long a_cur_id, bool a_flag);
	void			SetArcData		(PolyPoint *pnt, bool a_flag);

	inline short	GetType			() const;
	inline void		SetType			(short a_type);
	inline bool		IsCircle		() const;
	inline bool		IsPoly			() const;

	int				GetUsedMemory	();
	int				GetAllocatedMemory();

	////////////////////////////////// AlgPoly
	inline AreaSect	*GetSector(int id);
	inline AreaVtx	*GetLeftVertex();
	inline AreaVtx	*GetRightVertex();
	inline AreaVtx	*GetVertex(int id);

	inline bool			IsGeomCent();	
	inline AreaVtx		*GetFrontVtx(const AreaVtx *vtx);	// get opposite vertex
	inline GeomHdr		*NextLeft   (const AreaVtx *vtx);	// next polyline around vtx
	inline GeomHdr		*PrevRight  (const AreaVtx *vtx);	// next polyline around opposite vertex
	inline GeomHdr		*NextLeft (const AreaVtx *vtx, AreaVtx **v); // next polyline around vtx
	inline GeomHdr		*PrevRight(const AreaVtx *vtx, AreaVtx **v); // next polyline around opposite vertex
	
	inline int			GetVertexId(const AreaVtx *vtx)	;
	inline AreaVtx		*GetFrontVtx(int  sector_id);	// get opposite vertex
	inline GeomHdr		*NextLeft   (int *sector_id);	// next polyline around vtx
	inline GeomHdr		*PrevRight  (int *sector_id);	// next polyline around opposite vertex
	inline GeomHdr		*NextLeft (int *sector_id, AreaVtx **v);	// next polyline around vtx
	inline GeomHdr		*PrevRight(int *sector_id, AreaVtx **v); // next polyline around opposite vertex

	inline void				InitAlgIter(const AreaVtx *vtx, IterAlgPoly *itr); // initiate polyline iterator
	inline unsigned long	GetVertexType(const	AreaVtx *vtx);
	inline int				GetNextPolyId(int index);
	inline int				GetPrevPolyId(int index);
	inline AreaSect			*GetNextSectorId(int &index);
	inline AreaSect			*GetPrevSectorId(int &index);
	//void					SetNextPolyId(int sector_id, int flag_id);
	void Isolate();
#if MEM_GEOM_DEBUG
	static int		poly_count;
	static int		circle_count;
	static int		GetPolysCount();
	static int		GetCirclesCount();
	static int		GetAllUsedMemory();
	static int		GetAllAllocatedMemory();
#endif
};

//~~*~~~~~~~~~~ glPoly ~~~~~~~~~~~~~~~~~~~~~~~~
// glPoly - polyline header.
class SYSGL_API	glPoly	:	public	GeomHdr,	public	 Array2Data
{
public:
					glPoly			(long a_pnt_count=0, long a_arc_count=0);
	static glPoly	*Create			(long a_pnt_count = 0, long arc_count=0);
	static glPoly	*Create			(long, GL_Point *);

	void			Invert			(long a_from = 0,long a_to = -1);
	inline bool		IsClosed		();

		   void		SetUsedCount	(long cnt);
	inline long		GetUsedCount	()				{ return GetArrSize1()/GetItemSize(); };
	inline long		GetUsedSize		()				{ return GetArrSize1(); };
	static long		GetItemSize		()				{ return sizeof(PolyPoint);};
	inline PolyPoint *GetFirstPoint0()				{ return GetArrSize1() ? (PolyPoint *)(GetData1()) : NULL;	};
	void			DeleteData		();
	glPoly			*Copy0			(glPoly *a_dst=NULL);
    void			*Insert0		( int sec_index, const void *data = NULL, int count=1); 
	PolyArc* 		CreateArc		(GL_DPoint *a_c=NULL, long a_r=0);

	int				GetUsedMemory	()				{ return sizeof(glPoly)-sizeof(Array2Data)+Array2Data::GetUsedMemory(); };
	int				GetAllocatedMemory()			{ return sizeof(glPoly)-sizeof(Array2Data)+Array2Data::GetAllocatedMemory(); };
#if MEM_GEOM_DEBUG
	static int		allocated_data;
	static int		used_data;
	static int		GetAllUsedMemory()				{ return used_data;};
	static int		GetAllAllocatedMemory()			{ return allocated_data;};
#endif

protected:	
	void			Alloc			(const int num);
	inline void		AddCorner		(const GL_Point &);                       
	inline void		AddCornerD		(const GL_Point &,const GL_DPoint &,const long &);	 
	inline void		AddCorner		(const GL_Point &,const GL_Point &,const long &);	 
	inline void		DeleteArc		(long);
	inline void		DeleteCorner	(long);
	inline PolyArc	*GetArcPtr		(long);
	inline GL_Point	GetCorner		(long);
	inline bool		GetFirstId		(long *);
	inline bool		GetLastId		(long *);
	inline bool		GetNextId		(long *);
	inline bool		GetPrevId		(long *);
	inline void		InsertArc		(long, const GL_Point &,long);
	inline void		InsertArcD		(long, const GL_DPoint &,long);
	inline void		InsertCorner	(long, const GL_Point &);
	inline bool		IsFirstId		(const long &);
	inline bool		IsLastId		(const long &);
	inline bool		IsValidId		(long);
	void			SetCorner		(long,const GL_Point &);
	void 			CorrectArcPtrs	(char *data0, PolyPoint *ptr0=NULL);
};

//~~*~~~~~~~~~~ glCircle ~~~~~~~~~~~~~~~~~~~~~~~~
class SYSGL_API glCircle : public GeomHdr
{
public:
					    glCircle	();
	static inline glCircle	*Create	()							{ return (glCircle*)GL_Base::CreateCircle();}; 
	static inline glCircle	*Create	(GL_DPoint &c, long r)		{ return (glCircle*)GL_Base::CreateDCircle(c, r);};
	static glCircle	*Create		(glCircle &c, long a_width = 0);
	void				Delete		()							{ GL_Base::DelGeom(this); };

	inline const GL_DPoint	&GetDCenter	()						{ static GL_DPoint c;	return c=cir.GetCenter();};
	inline const GL_Point	&GetCenter	()						{ return cir.GetCenter();};
	inline long			GetRadius	()							{ return cir.radius;};
	inline void			GetLPoint	(GL_Point &pnt)				{ cir.GetLPoint(pnt);};
	inline void			GetRPoint	(GL_Point &pnt)				{ cir.GetRPoint(pnt);};
	inline void			GetCircle	(GL_Point &cent, long &rad)	{ cir.GetCircle(cent, rad);};
	
	inline void			SetCenter	(const GL_DPoint &c)		{ cir.SetCenter(c);};
	inline void			SetCircle	(const GL_DPoint &c, long r){ SetCenter(c), SetRadius(r);};
	inline void			SetRadius	(long r)					{ cir.radius=r;};
	inline void			Invert		()							{ SetRadius(-GetRadius());};

	GL_Circle			cir;
};

//~~*~~~~~~~~~~ GroupData ~~~~~~~~~~~~~~~~~~~~~~~~
struct	GroupData 
{
	GL_Node				*poly;
	GeomGroup			*child;
	void				*misc;
};

// This structure organize refer to up(parent) level from geom. group. 
// GeomGroup			*parent; - pointer to header of the "parent" group
//	int					ind_in_parent; - index this group in the parent group.
//	void				*misc; - misc data.

//~~*~~~~~~~~~~ GroupParentData ~~~~~~~~~~~~~~~~~~~~~~~~
struct	GroupParentData 
{
	GeomGroup	*parent;
	int			ind_in_parent;
	void		*misc;
};

//~~*~~~~~~~~~~ GeomGroup ~~~~~~~~~~~~~~~~~~~~~~~~
class	SYSGL_API GeomGroup : public glPoly	
{
public:
	inline GroupParentData *GetParentDataPtr	();	
	inline void			AddItm					(GroupData );
	inline int			GetItmCount				();
	static GeomGroup	*CreateTemp				(int a_count, int a_add_count); 
	inline GroupData	*GetItmDataPtr			();
private:
	void				InsertItm				(int, GroupData );
	void				ExcludeItm				(int);	
};

#endif //__GL_POLY_H__

