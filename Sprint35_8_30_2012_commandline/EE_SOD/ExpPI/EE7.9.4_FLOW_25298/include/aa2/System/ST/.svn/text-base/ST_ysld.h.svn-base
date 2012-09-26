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

// Name:				ST_ysld.h
// Date:				10.20.86
// Author:			Alexander Zabolotnikov	
// Description:	functions for y-slider

#ifndef __SAS_YSLD_H__
#define __SAS_YSLD_H__

//#include "sys.h"
//#include "ST_def.h"
//#include "ST_util.h"
#include "ST_xitm.h"
#include "ST_seg.h"

class stSlider;
class stScan;
class SAS_Seg;

//-----------------------------------------------------------------
class SYSGL_API VertItem
{
public:
	VertItem() {p = NULL; xi = NULL;}
	inline void	GetSeg(SAS_Seg &sg, const GL_Point *pnt = NULL);

	GL_Point	*p;
	XEvent		*xi;
	//SAS_Seg		*seg;
	int			f[2];
	int			next;
	int			prev;
};

inline void VertItem::GetSeg(SAS_Seg &sg, const GL_Point *pnt)
{
	sg.SetItem(xi, NULL); if(pnt) { sg.GoToPnt(*pnt);}
}
//-----------------------------------------------------------------
#define Y_PAGE_SIZE 256
//#define Y_PAGE_SIZE 64000
class SYSGL_API VertLine
{
	friend class stSlider;
public:
	char			*array_y;
	VertItem		*scan_y;
	VertItem		*itm;
	unsigned short	*scan_s;
	int	iy, fre_itm, is_vector;
	int ky, count;
	VertLine();
	~VertLine();
	int			Init(const int count);
	void		Free();
	void		DeleteAll();

	inline VertItem	*GetItem(const int ord);
	inline VertItem	*YItem (const int ord);
	inline VertItem	*GetPrev(VertItem *i);
	inline VertItem	*GetNext(VertItem *i);
	
	inline VertItem	*CreateItem(const int ord);
	inline int			Delete(const int ord, stScan *scan = NULL);
	inline int			Swap(const int ord);
	inline void			MakePrevList();
	int					CheckLP();
};

inline VertItem *VertLine::GetPrev (VertItem *i)
{
	return (i && i->prev)? scan_y + i->prev : NULL;
}

inline VertItem *VertLine::GetNext (VertItem *i)
{
	return (i && i->next)? scan_y + i->next : NULL;
}

inline VertItem *VertLine::GetItem (const int ord)
{
	if(!is_vector) {
		itm = scan_y;
		for(iy = 1; iy < ky;) {
			scan_s[iy++] = (unsigned short)itm->next;
			itm	= scan_y + itm->next;
		}
		is_vector = 1;
	}
	return scan_y + (int)scan_s[ord];
}

inline VertItem *VertLine::YItem (const int ord)
{
	return scan_y + ord;
}

inline void	VertLine::MakePrevList()
{
	if (ky > 1) {
		iy = 0;
		do {
			scan_y[scan_y[iy].next].prev = iy;
			iy = scan_y[iy].next;
		} while (iy);
	}
}

inline VertItem *VertLine::CreateItem(const int i)
{
	if(fre_itm) {
		iy = fre_itm;
		fre_itm = (int)(scan_y[fre_itm].xi);
		//fre_itm = *(unsigned short *)&(scan_y[fre_itm].xi);
	} else {
		iy = ky;
	}
	if (i < ky) {
		memmove(scan_s + i + 1, scan_s + i, (unsigned int)(sizeof(unsigned short)*(ky - i)));
	}
	scan_s[i] = (unsigned short)iy;
	ky++;
	return(scan_y + iy);
}

inline int VertLine::Delete(const int ord, stScan *scan)
{
	ky--;
	itm = GetItem(ord);
	itm->xi = (XEvent *)fre_itm;
	//fre_itm = (unsigned short)(itm - scan_y);
	fre_itm = (int)(itm - scan_y);
	if(ord < ky) {
		memmove(scan_s + ord, scan_s + ord + 1, (unsigned int)(sizeof(unsigned short)*(ky - ord)));
	}
	return(ord);
}

inline int VertLine::Swap(const int ord)
{
	iy						= (int)scan_s[ord];
	scan_s[ord]			= scan_s[ord + 1];
	scan_s[ord + 1]	= (unsigned short)iy;
	return(ord);
}
/*--------------------------------------------------------------
-----------------------------------------------------------------*/
class SYSGL_API stSlider : public GL_Point
{
friend class stScan;
public:
	stSlider();
	stSlider(stScan *scan);
	stSlider(stSlider *yslid);
	~stSlider();

	int GetFirst();
	int GetLast();
	inline int GetNext();
	inline int GetPrev();
	inline int operator++() { return GetNext();} //Move YSlider to the next right extremal point
	inline int operator--() { return GetPrev();} //Move YSlider to the prev. left extremal point

	void	ClearClipData();
	void	MakeListData();
	inline void	MakeVector();
	inline void	MakePrevList();

	int	GoIdL(int id);
	int	GoXL(const long x);
	int	GoL (const GL_Point &pnt);
	int	GetVSGL(const GL_Point &pnt, GL_Point &dn, GL_Point &up);
	int	GetFirstL();
	int	GetLastL();
	inline int	GetNextL();
	inline int	GetPrevL();
	inline void	InsertL(XEvent *xi);
	inline void	DeleteL(XEvent *xi);
	inline void	ReplaceL(XEvent *xi);
	inline void	SwapL(XEvent *xi);

	inline int	GetNextLP();
	inline int	GetPrevLP();
	inline void	InsertLP(XEvent *xi);
	inline void	DeleteLP(XEvent *xi);
	inline void	SwapLP(XEvent *xi);

	int GoAfterX(const long x);
	int GoX(const long x);   //Move YSlider to x position
	int GoY(const long y);   //Find y-position for point (XCoord, y)
	int Go (const GL_Point &pnt); //GoX(pnt.x); then GoY(pnt.y);
	int GoId(int id);
	inline int GoDnUp(const GL_Point &pnt,	XEvent **dn,	XEvent **up);

	inline VertItem *GetPrev (VertItem *item);
	inline VertItem *GetNext (VertItem *item);	
	inline VertItem *YItem (const int ord);

	inline void		SetPoint(GL_Point *pnt, const int ord);

	int				SetSld(stSlider *yslid);
	int				SetChannel(const int bot, const int top);  //Set channel for the movement.
	int				Status(int dir);       //Insert/Delete/Replace/Swap
	SAS_Seg			*Seg(const int Y_position);				    //Get the segment pointer
	void			Clear();
	void			Init(stScan *scan);
	inline int		Insert(XEvent *xi, GL_Point *event_p);
	inline int		Delete(const int ord);
	inline int		Replace(XEvent *xi, GL_Point *event_p);
	inline int		Swap(const int ord, GL_Point *event_p);
	SAS_Seg			*GetSeg(const int ord, const GL_Point *pnt = NULL);
	inline bool		GetSeg(SAS_Seg &seg, const int ord,  const GL_Point *pnt = NULL);
	int				GetVSG(const GL_Point &pnt, GL_Point &dn, GL_Point &up);
	int				GetMinVSG(const GL_Point &pnt, XEvent **xi, long &dy);
	inline int		IsValidOrd(int ord);
	int				GetValidOrd(int ord0, int dir);
	inline GL_Point *GetPoint(const int ord);
	inline VertItem *GetYItem(const int ord);
	inline int		GetCount() { return vl.ky;}
	int				Xind(long x, int beg_id, int end_id, int dir);
	int				CollectGeom(long x);
	int				GetNearNode(const GL_Point &pnt, GL_Point &nea, long &rad,	XEvent **xi);
	int				NextInReg(const int type_ev, const int ord, int &dn_ord, int &up_ord);
	int				PrevInReg(const int type_ev, const int ord, int &dn_ord, int &up_ord);

	//--------------------------------
	XEvent			*n_crn;
	int				n_idx, kx;
	int				i, j, ii, ev;

	int				GetPntOrd(const GL_Point &pnt, int &c_dir);
	int				GetSegOrd(const SAS_Seg *sg, int &c_dir);

///private:
	VertLine			vl;
	int				iy;				// current position
	int				dir;
	int				is_fixed;

	VertItem		*yitem;			//
	stScan		*scanner;		// parent
};

//-----------------------------------------------------------
inline void stSlider::SetPoint(GL_Point *pnt, const int ord)
{
	yitem = vl.GetItem(ord);
	if(yitem) {
		yitem->p = pnt;
	}
}
inline GL_Point *stSlider::GetPoint(const int ord)
{
	yitem = vl.GetItem(ord);
	if(yitem) {
		return yitem->p;
	} else {
		return NULL;
	}
}
inline VertItem *stSlider::GetYItem(const int ord)
{
	return vl.GetItem(ord);
}

//-----------------------------------------------------------
inline int stSlider::Insert(XEvent *xi, GL_Point *event_p)
{
	yitem			= vl.CreateItem(xi->GetOrd());
	yitem->xi	= xi;
	return(xi->GetOrd());
}

inline int stSlider::Delete(const int ord)
{
	return vl.Delete(ord, scanner);
}

inline int stSlider::Replace(XEvent *xi, GL_Point *event_p)
{
	yitem		 = vl.GetItem(xi->GetOrd());
	yitem->xi = xi;
	//if (yitem->seg) yitem->seg->GetNextRight();	//TBD for prev
	return(xi->GetOrd());
}

inline int stSlider::Swap(const int ord, GL_Point *event_p)
{
	return vl.Swap(ord);
}
inline int stSlider::GetNext()
{
	if(n_idx < kx) {
		ev = n_crn->GetType();
		if (ev == INS_EVENT) {
			iy = Insert(n_crn, NULL);
		} else if (ev == DEL_EVENT) {
			iy = Delete(n_crn->GetOrd());
		} else {	//case CRS_EVENT:
			iy = Swap(n_crn->GetOrd(), NULL);
		}
		n_idx++;
		n_crn++;
	} else {
		iy = 0;
	}
	return iy;
}

inline int stSlider::GetPrev()
{
	if(n_idx > 0) {
		n_idx--;
		n_crn--;
		ev = n_crn->GetType();
		if (ev == INS_EVENT) {
			iy = Delete(n_crn->GetOrd());
		} else if (ev == DEL_EVENT) {
			iy = Insert(n_crn, NULL);
		} else { //case CRS_EVENT:
			iy = Swap(n_crn->GetOrd(), NULL);
		}
	} else {
		iy = 0;
	}
	return iy;
}
//-----------------------------------------------------------
inline void stSlider::InsertL(XEvent *xi)
{
	vl.scan_y[xi->y_ind].xi		= xi;
	vl.scan_y[xi->y_ind].next	= vl.scan_y[xi->y_dwn].next;
	vl.scan_y[xi->y_dwn].next	= xi->y_ind;
	vl.ky++;
}

inline void stSlider::DeleteL(XEvent *xi)
{
	vl.scan_y[xi->y_dwn].next = vl.scan_y[xi->y_ind].next;
	vl.ky--;
}

inline void stSlider::ReplaceL(XEvent *xi)
{
	vl.scan_y[xi->y_ind].xi	= xi;
}

inline void stSlider::SwapL(XEvent *xi)
{
	i = vl.scan_y[xi->y_dwn].next;
	j = vl.scan_y[i].next;
	
	vl.scan_y[xi->y_dwn].next = j;
	vl.scan_y[i].next = 	vl.scan_y[j].next;
	vl.scan_y[j].next = 	i;
}

inline int stSlider::GetNextL()
{
	if(n_idx < kx) {
		ev = (int)n_crn->type_ind.type;
		if (ev == INS_EVENT) {
			InsertL(n_crn);
		} else if(ev == DEL_EVENT) {
			DeleteL(n_crn);
		} else {
			SwapL(n_crn);
		}
		n_idx++;
		n_crn++;
		vl.is_vector = 0;
		return 1;
//		return (int)n_crn->ord;
	} else {
		return 0;
	}
}

inline int stSlider::GetPrevL()
{
	if(n_idx > 0) {
		n_idx--;
		n_crn--;
		ev = (int)n_crn->type_ind.type;
		if (ev == INS_EVENT) {
			DeleteL(n_crn);
		} else if(ev == DEL_EVENT) {
			InsertL(n_crn);
		} else {
			SwapL(n_crn);
		}
		vl.is_vector = 0;
		return (int)n_crn->ord;
	} else {
		return 0;
	}
}
//------------------------------------------------
inline void stSlider::InsertLP(XEvent *xi)
{
	yitem			= vl.scan_y + xi->y_ind;
	yitem->xi	= xi;

	yitem->next = vl.scan_y[xi->y_dwn].next;
	vl.scan_y[xi->y_dwn].next = xi->y_ind;

	yitem->prev	= xi->y_dwn;
	vl.scan_y[yitem->next].prev = xi->y_ind;
	vl.ky++;
}

inline void stSlider::DeleteLP(XEvent *xi)
{
	yitem	= vl.scan_y + xi->y_ind;
	vl.scan_y[xi->y_dwn].next = yitem->next;
	
	vl.scan_y[yitem->next].prev = xi->y_dwn;
	vl.ky--;
}


inline void stSlider::SwapLP(XEvent *xi)
{
	i = vl.scan_y[xi->y_dwn].next;
	j = vl.scan_y[i].next;
	
	vl.scan_y[xi->y_dwn].next = j;
	vl.scan_y[i].next = 	vl.scan_y[j].next;
	vl.scan_y[j].next = 	i;

	vl.scan_y[vl.scan_y[i].next].prev = i;
	vl.scan_y[i].prev = j;
	vl.scan_y[j].prev = xi->y_dwn;
}

inline int stSlider::GetNextLP()
{
	if(n_idx < kx) {
		ev = (int)n_crn->type_ind.type;
		if (ev == INS_EVENT) {
			InsertLP(n_crn);
		} else if(ev == DEL_EVENT) {
			DeleteLP(n_crn);
		} else {
			SwapLP(n_crn);
		}
		n_idx++;
		n_crn++;
		vl.is_vector = 0;
		return 1;
//		return (int)n_crn->ord;
	} else {
		return 0;
	}
}

inline int stSlider::GetPrevLP()
{
	if(n_idx > 0) {
		n_idx--;
		n_crn--;
		ev = (int)n_crn->type_ind.type;
		if (ev == INS_EVENT) {
			DeleteLP(n_crn);
		} else if(ev == DEL_EVENT) {
			InsertLP(n_crn);
		} else {
			SwapLP(n_crn);
		}
		vl.is_vector = 0;
		return (int)n_crn->ord;
	} else {
		return 0;
	}
}
//------------------------------------------------
inline void	stSlider::MakeVector()
{
	if(!vl.is_vector) {
		yitem = vl.scan_y;
		for(iy = 1; iy < vl.ky;) {
			vl.scan_s[iy++] = (unsigned short)yitem->next;
			yitem	= vl.scan_y + yitem->next;
		}
		vl.is_vector = 1;
	}
}

inline VertItem *stSlider::GetPrev (VertItem *item)
{
	return (item)? vl.scan_y + item->prev : vl.scan_y + vl.scan_y->prev;
}
inline VertItem *stSlider::GetNext (VertItem *item)
{
	return (item)? vl.scan_y + item->next : vl.scan_y + vl.scan_y->next;
}
inline VertItem *stSlider::YItem (const int ord)
{
	return vl.scan_y + ord;
}

inline bool stSlider::GetSeg(SAS_Seg &seg, const int ord,  const GL_Point *pnt)
{
	yitem = (ord < vl.ky)? vl.GetItem(ord) : NULL;
	if (yitem) {
		seg.SetItem(yitem->xi, NULL);
		if(pnt) {
		  seg.GoToPnt(*pnt);
		}
		return TRUE;
	}
	return (FALSE);
}
inline void	stSlider::MakePrevList() { vl.MakePrevList();}

inline int stSlider::GoDnUp(const GL_Point &pnt,	XEvent **dn,	XEvent **up)
{
	if (Go(pnt) >= 0) {
		*dn = vl.GetItem(iy)->xi;
		*up = vl.GetItem(i)->xi;
	} else {
		*dn = NULL;
		*up = NULL;
	}
	return iy;
}

inline int	stSlider::IsValidOrd(int ord)
{
	if (ord < vl.ky) {
		yitem = vl.GetItem(ord);
		GeomHdr *a = yitem->xi->node;
		if (a && !(a->flags & (GL_SAS_HIDE | GL_SAS_DELETE) || a->hdr_flags & HDR_FLAG_ERASE)) {
			return 1;
		}
	}
	return 0;
}

#endif	// __SAS_YSLD_H__

