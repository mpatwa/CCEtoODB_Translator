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

// Name:				ST_scan.h
// Date:				10.20.96
// Author:			Alexander Zabolotnikov	
// Description:	Scanner main functions

#ifndef __SAS_SCAN_H__
#define __SAS_SCAN_H__

#include "ST_seg.h"
#include	"ST_clip.h"

class	stSlider;
class stCross;
class stData;
class	stSegArr;
class stIntArr;
class LayerScanner;

class IdCorrector
{
public:
	short dy;
	short rm;
	IdCorrector *up;
	IdCorrector *dn;
};

class SYSGL_API stScan
{
public:

	stScan(int LiderLen = 0);
	~stScan();
	void Clear();

	inline bool IsEmpty() { return (y_max_count == 0); }
	int Update();

///   bool Insert(ODB_SegIter &Seg);
	bool Insert(IterGeom &crn);
	bool Insert(stScan &scan);
	bool Insert(GeomHdr *gh);

	bool Delete(GL_Segment &Seg);
	bool Delete(IterGeom &crn);
	bool Delete(stScan &scan);			// ??
	bool Delete(GeomHdr *gh);
	bool Upd_Geom(GeomHdr *gh);
	bool UpdScan();
	GeomHdr *StartEdit(GeomHdr *gh);
	bool EndEdit();
	bool Undo(int marker);

///   bool CheckCrossInsert(ODB_SegIter &seg);
	bool CheckCrossInsert(IterGeom  &crn);
	bool CheckCrossInsert(stScan &scan);

///   bool CheckDistInsert(ODB_SegIter &seg, long distance = 0);
	bool CheckDistInsert(IterGeom  &crn, long distance = 0);
	bool CheckDistInsert(stScan &scan, 		  long distance = 0);

///   bool CheckInsert(ODB_SegIter &seg);
	bool CheckInsert(IterGeom &crn);
	bool CheckInsert(stScan &scan);

	int 		GetStatus(int XInd, int dir); 	// Insert/Delete/Replace/Swap
	int 		GetYord  (int XInd);          	// Y position of point
	inline long X(int XIndex) {return (XIndex < 0)? ll.x : (XIndex < kx)? XArr.xl[XIndex].GetX() : ur.x;}
	int			Xind(long x, int beg_id, int end_id, int dir = 0);
///   ODB_SegIter *Seg(int XInd, int dir);  // Get the segment, which points belong to.
   IterGeom  *Crn(int XInd, int dir);  // Get the segment, which points belong to.

	//int		Load(SAS_LoadFilter *filter);
	int			LoadNode(GL_Node *node);
	void		SortX();
	bool		LoadArr(glPoly **arr, int num);
	bool		LoadPolyArr(glPoly **arr, int num_poly);
	int			OutPlines(int flg_pnt = 0);

	void		UpdRange(SAS_Seg *seg, const int maxy);
	void		UpdRange(int iy);

	void		FreeStatSl();
	int			SetDxSl(const long dx);
	int			SetDiSl(const int di);
	int			SetNumSl(const int num);
	void		SetUpdFlag();
	void		CrossLoc(const GL_Box *box, GL_Array *arr);
	void		CrossSeg(GL_Point &lpt, GL_Point &rpt, GL_Array *arr);

//	void		RemoveGeom(unsigned long flg = GL_SAS_DELETE, long *l = NULL, long *r = NULL);
//	void		CorrectPtr();
	int			LoadSimNode(GeomHdr *node);
	int			AddSimNode(IterPolyGeom *crn, GL_Box	&box, int is_mono);
//private:
	
	GL_Point ll, ur;
	int y_max_count;

	XEventArr XArr;	// for Insert/Delete/replace pointers
	int	XInd;		// XArr.xl[XInd] -- current item
	int	kx;		// max_point in Xarr
	
	stIntArr   IntArr;
	//stIntPnt *lastPnt[2];

	SAS_Clip	sas_c;
	stSlider sl;

	stSlider *YStatic;
	int	NumStatic;
	int	MaxStatic;
	int	CurStatic;
	long	XStep;

	int	Ylist;
	stSegArr	SegArr;

	// edit
	int num_chk, num_edit;
	stScan			*next;
	stScan			*prev;	//?
	GeomHdr		*first_gh;
	GeomHdr		*last_gh;
	LayerScanner		*p_scan;
	unsigned int	scan_level;
	int create_pbox;
	int num_hidden;
};

inline void stScan ::UpdRange(SAS_Seg *seg, const int maxy)
{
	if (seg) {
		if(seg->ymin   < ll.y) ll.y = seg->ymin;
		if(seg->ymax   > ur.y) ur.y = seg->ymax;
		if(seg->beg.x  < ll.x) ll.x = seg->beg.x;
		if(seg->end.x  > ur.x) ur.x = seg->end.x;
		if(y_max_count < maxy) y_max_count = maxy;
	}
}

inline void stScan ::UpdRange(int iy)
{
		if(y_max_count <= iy) y_max_count = iy+1;
}

class SYSGL_API stCross
{
public:
	GL_Point p;
	SAS_Seg *i;
	SAS_Seg *j;
};

class SYSGL_API stData
{
public:
					stData();
					stData(stScan	*scanner);
					~stData();
	void			Init(stScan	*scanner);
	int Update();

	void			X_Insert0	(SAS_Seg *seg);
	int			X_Insert		();
	int			Y_Insert		(const GL_Point &p);
	int			Y_InsAlg		(const GL_Point &p);
	int			P_Insert		(SAS_Seg *i,SAS_Seg *j, GL_Point *p);
	int			ReplaceSeg	();
	
	int			X_Delete();
	int			Y_Delete(SAS_Seg *ie);
	int			P_Delete();
	
	int			GetSegments();
	int			NextEvent (int fl);
	SAS_Seg		*GetElement ();

	int			elsys(int i, int j, GL_Point &pt, GL_Point *pi, int k);
	int			WriteXItem(SAS_Seg *seg, SAS_Seg *seg_up = NULL);

	// in sas_link
	int			WriteXItemM(SAS_Seg	*seg, SAS_Seg *up);
	int			AddScan(stScan	*a_scan);
	int			AddSl(stSlider *sln);
	int			InitLink();
	int			GetStart();
	int			UniteScan(stScan *rezult = NULL);
	int			Y_InsertM (const GL_Point &p);
	int			GetSegmentsM();
	SAS_Seg		*GetElementM();
	int			NextEventM (int fl);
	SAS_Seg		*PushInsert(stSlider *slj, const int idj);
	int			CompSlSegs(stSlider *sli, stSlider *slj, const int idi, const int idj);
	inline SAS_Seg	*GetSlSeg(stSlider *slid, const int sl_idx);
	SAS_Seg		*SetSlSeg(stSlider *sl, const int sl_idx);
	int			LinkXArr();

	int			IntScaners(stScan *rezult);
	int			GetSegmentsA();

	stSlider	**sl;
	stScan		*rez_scan, *first_scan;
	int			num_sliders;
	int			first_idx;
	VertItem	*itm;

	stScan		*scan;
	stCross	*scanP;
	SAS_Seg		**scanX;
	SAS_Seg		**scanY;
	SAS_Seg		**scanE;
	SAS_Seg		*seg, *dn_seg, *up_seg, *e_seg;

	GL_Array	YArr;
	GL_Array	XArr;
	GL_Array	PArr;
	GL_Array	EArr;
	XEventArr	ZArr;
	XEventArr	OArr;
	int			z_ind;

	XEvent			*item;
	GL_Point	pt, pi, po;
	GL_Point	*p_beg, *p_int, *p_end;
	int			ix, kx, fx;
	int			iy, ky;
	int			ip, kp;
	int			ie, ke;
	int			event_type;
	int			num_itm;
	int			ind_itm;
	int			ovl_w;
	int			ovl_a;
	int			rev_w;
	int			rev_a;
};

inline SAS_Seg *stData::GetSlSeg(stSlider *slid, const int ord)
{
	//if (slid && ord < slid->GetCount()) {}
	if (slid) {
		itm = slid->YItem(ord);
		return  (SAS_Seg *)itm->p;
	}
	return NULL;
}

void SYSGL_API DisconnectNodes(stScan *scan);
extern void SYSGL_API RemoveGeomFromList(GeomHdr **first,unsigned long rem_flag);
extern int SYSGL_API GetBoxAndIdx(IterPolyGeom *crn, GL_Box &box, int &il, int &ir, int is_mono);
int SYSGL_API CorrectGeomMask(stScan *scan);

#endif
// End of file

