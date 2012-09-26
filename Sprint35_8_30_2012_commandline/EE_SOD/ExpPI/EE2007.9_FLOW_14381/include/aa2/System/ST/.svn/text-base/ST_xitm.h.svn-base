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

// Name:				ST_xitm.h
// Date:				10.20.86
// Author:			Alexander Zabolotnikov	
// Description:	Scanner corners

#ifndef __SAS_XITM_H__
#define __SAS_XITM_H__

#include "../Gl/GL_Array.h"
#include "../Gl/GL_PageMem.h"
#include "../Gl/GL_Geom.h"
#include "../Gl/GL_poly.h"

class stScan;
class SAS_Seg;
class stIntPnt;

class	stSlider;
extern stIntPnt *GetIntPoint(GL_Node *a_node);
extern void SetIntPoint(GL_Node *a_node, stIntPnt *i_pnt);
extern int IntPntCmp (stIntPnt *vi, stIntPnt *vj, GL_Node *node);

#define INS_EVENT	0  // <--
#define DEL_EVENT	1	// -->
#define CRS_EVENT	2	//  X
#define REP_EVENT	3	// -*-
#define CRS_REPL	4
#define NON_EVENT	8
#define MAX_XIND_ORD	64000
#define MAX_POLY_OFF	8190
#define DELTA_SIZE 1024

/////////////////////////////////////////////////////////////////////
// XItemType - describes point data: type of event, index of point
//					in respect to polyline & polyline direction
/////////////////////////////////////////////////////////////////////
typedef struct 
{
	unsigned long x_ind	: 29;	// 0 <= ord <= 8192
	unsigned long type	:	2;	// Insert | Delete | Crossing | Replace
	unsigned long dir		:  1;	// XY-direction 1 --> // 0 <--
} XItemType;

/////////////////////////////////////////////////////////////////////
// XEvent - describes extremal point data
/////////////////////////////////////////////////////////////////////
class	SYSGL_API XEvent
{
public:
	//XEvent (Node *a_node,XItemType a_type_point,const GL_Box &a_box,short a_index_point = 0)
	XEvent(){};
   XEvent (GL_Node *a_node, int a_type_point, int a_index_point = 0);
	~XEvent(){};
    GeomHdr			*GetNode()  const { return node;};
	int				GetOrd()		const { return (int)ord; };
	inline int		GetOffSet() const { return (int)type_ind.x_ind;}; 
	inline int		GetType()	const { return (int)type_ind.type;};
	inline int		GetDir()		const { return (int)type_ind.dir ? 1 : -1;};
	inline GL_Point	*GetPoint(GL_Point *p) const;
	inline GL_Point	GetPnt() const;
	inline long		GetX() const;
	inline long		GetY() const;
	inline unsigned short			GetYInd() const { return y_ind; };
	inline unsigned short			GetYDwn() const { return y_dwn; };

	inline void     SetNode(GL_Node *a_node);
	inline void		SetOrd    (int an_ord)				{ ord					= an_ord; };
	inline void		SetOffSet (int a_ind)				{ 
//		ASSERT_WRN(a_ind <= 8192, "SAS: X Index Overflow");
		type_ind.x_ind	= a_ind;
	};
	inline void		SetType   (int a_type)				{ type_ind.type	= (unsigned short)a_type;};
	inline void		SetDir    (int a_dir)				{ type_ind.dir		= (a_dir > 0)? 1 : 0;};
	inline void		SetYInd	 (unsigned short id)					{ y_ind = id; };
	inline void		SetYDwn	 (unsigned short id)					{ y_dwn = id; };
	
	int				Set(SAS_Seg &seg, int ev = INS_EVENT);
    inline void     Set(GeomHdr *node, int id = 0, int dir = 1);

    GeomHdr			*node;		// point's parent
	XItemType		type_ind;	// point type & index
	long			ord;		// Y-ord
	unsigned short	y_ind;		// index in y-slider
	unsigned short	y_dwn;		// lower index in y-slider
};

/////////////////////////////////////////////////////////////////////
// XEventArr - array for XEvent
/////////////////////////////////////////////////////////////////////
class	SYSGL_API XEventArr : public GL_Array
{
public:
	XEventArr(int num = 0);
	inline int GetCount();
	inline XEvent *Expand(int delta_num);
	inline XEvent *InsItem(int ind, int delta);
	inline void Clear();
	inline void AddInsItem(GeomHdr *node, int id = 0, int dir = 1);
	void LoadOneGeom(GeomHdr *node);
	void AddFlag(unsigned long flag);
	void RemoveFlag(unsigned long flag);

	XEvent	*xl;	// first item
	XEvent *xi;	// tmp item
	int	kx;	// the number of items
};

inline void LoadOneGeom (GeomHdr *node, XEventArr *XArr)
{
	if (XArr) XArr->LoadOneGeom(node);
}

/////////////////////////////////////////////////////////////////////
// stIntPnt - represents cross-point
/////////////////////////////////////////////////////////////////////
class SYSGL_API stIntPnt : public GL_Point
{
public:
	//XEvent xi[2];	// temporary
	stIntPnt();
	~stIntPnt();
	inline GL_Point	*GetIntPnt()  const { return (GL_Point *)this; }
	inline void	Link	( GL_Point *p_int) { x = p_int->x; y = p_int->y;}
	//void Link( GL_Point *p_int, SAS_Seg *seg1, SAS_Seg *seg2);
	//inline int	GetOrd()		const { return xi[0].GetOrd(); }
	//inline int	GetUpOrd()	const { return xi[1].GetOrd(); }
};


/////////////////////////////////////////////////////////////////////
// stIntArr - array for cross-points
/////////////////////////////////////////////////////////////////////
class SYSGL_API stIntArr : public glPageMem
{
public:
	stIntArr(){ Init(sizeof(stIntPnt), DELTA_SIZE);}
	~stIntArr() {}
	stIntPnt *NewPnt();
	void		DelPnt(stIntPnt *seg);
	void		DelAll();
private:
	//stIntPnt *fre_seg, *tmp_seg;
};

/////////////////////////////////////////////////////////////////////
// NewPnt - reserve memory for new item
/////////////////////////////////////////////////////////////////////
inline stIntPnt *stIntArr::NewPnt()
{
	return (stIntPnt *)NewItem();
}
/////////////////////////////////////////////////////////////////////
// DelPnt - link item to free list
/////////////////////////////////////////////////////////////////////
inline void stIntArr::DelPnt
(
	stIntPnt	*ip	// deleted item
)
{
	ASSERT_WRN(ip, "stIntArr: Null segment pointer");
	DelItem((void *)ip);
}
/////////////////////////////////////////////////////////////////////
// DelAll - free all memory
/////////////////////////////////////////////////////////////////////
inline void stIntArr::DelAll()
{
	glPageMem::FreeAll();
}


////////////////////////////////////////////////////////////////////
//////////// IterEvent /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
class	SYSGL_API IterEvent
{
public:
	XEvent *x0;
	XEvent *xl;
	XEvent *xi;
	XEvent *xd;
	XEventArr *arr;

	IterEvent();
	~IterEvent();
	int Set(stScan *scan);
	int Set(stSlider *sl);
	inline int GetNext();
	inline int GetPrev();
	inline int GetNextA();
	inline int GetPrevA();
	inline int GoToIdx(int idx);
	inline int GoToXi(XEvent *xi);	//

    inline GeomHdr	*GetNode()  const { return xi->GetNode();};
	inline int		GetOrd()		const;
	inline int		GetOffSet() const;
	inline int		GetType()	const;
	inline int		GetDir()		const;
	inline int		GetIdx()		const;
	inline GL_Point	GetPnt()		const;
	inline long		GetX()		const;
	inline long		GetY()		const;
	inline unsigned short	GetYInd()	const;
	inline unsigned short	GetYDwn()	const;
	inline GL_Point	*GetPoint(GL_Point *p) const;
};

inline	int			IterEvent::GetOrd()		const { return xi->GetOrd();}
inline	int			IterEvent::GetOffSet()	const { return xi->GetOffSet();}
inline	int			IterEvent::GetType()		const { return xi->GetType();}
inline	int			IterEvent::GetDir()		const { return xi->GetDir();}
inline	int			IterEvent::GetIdx()		const { return (int)(xi - x0);}
inline	GL_Point	IterEvent::GetPnt()		const { return xi->GetPnt();}
inline	long	IterEvent::GetX()			const { return xi->GetX();}
inline	long	IterEvent::GetY()			const { return xi->GetY();}
inline	unsigned short	IterEvent::GetYInd()		const { return xi->GetYInd();}
inline	unsigned short	IterEvent::GetYDwn()		const { return xi->GetYDwn();}
inline	GL_Point  *IterEvent::GetPoint(GL_Point *p) const { return xi->GetPoint(p);}


inline int IterEvent::GoToIdx(int idx)
{
	if (idx >= 0 && x0 + idx < xl) {
		xi = x0+ idx;
		return 1;
	} else {
		return 0;
	}
}

inline int IterEvent::GoToXi(XEvent *xit)
{
	if (xit >= x0 && xit < xl) {
		xi = xit;
		return 1;
	} else {
		return 0;
	}
}

// GetNext by order
inline int IterEvent::GetNext()
{
	if(++xi < xl) {
		return 1;
	} else {
		--xi;
		return 0;
	}
};

// GetPrev by order
inline int IterEvent::GetPrev()
{
	if(xi > x0) {
		xi--;
		return 1;
	} else {
		return 0;
	}
}

// GetNext arround point
inline int IterEvent::GetNextA()
{
	if(++xi < xl) {
		return 1;
	} else {
		--xi;
		return 0;
	}
};

// Get prev arround point
inline int IterEvent::GetPrevA()
{
	if(xi > x0) {
		xi--;
		return 1;
	} else {
		return 0;
	}
}
inline GeomHdr		*GetAlgbPoly(XEvent *xi)				{ return (GeomHdr *)(xi->node);}

#endif

