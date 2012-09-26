/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:			GL_qlist.h
// Date:			08.03.04
// Author:			Alexander Zabolotnikov	
// Description:		structures for fast access

#ifndef __GL_QLIST_H__
#define __GL_QLIST_H__

#include "GL_PageMem.h"
#define MEM_GEOM_DEBUG	0	
//~~~~~~~~~~~~~~~~~~~~~~ Array2Data ~~~~~~~~~~~~~~~~~~~~~~~~~~~

// (this)--static---(data1)--dynamic1--(data1 + used1)---- free --(data1 + used2)--dynamic2---(data2)
class Array2Data;
class PolyAllocator;
class Data2Data
{
	friend class Array2Data;
	friend class PolyAllocator;
private:
	char	*data1;	// ((char *)this) + static size
	char	*data2;	// ((char *)this) + array size - size2;
	char	*last1;	// data1 + used1
	char	*last2;	// data2 - used2
	short	size1;
	short	size2;
	
	void Init2(int num, int size, int static_size, int size2);
	int GetNumStatic() {return data1 - (char *)this;}
};

class SYSGL_API Array2Data
{
public:
//private:
	Data2Data		*d2d;
public:
#if MEM_GEOM_DEBUG
	static int		allocated_count;
	static int		used_count;
	static int		GetAllAllocatedMemory();// { return allocated_count;};
	static int		GetAllUsedMemory();//		 { return used_count;};
#endif
	static bool		permit_realloc;
	static double	inc_2arr;
	Array2Data	();
	~Array2Data	()	{
#if MEM_GEOM_DEBUG
	allocated_count-=GetAllocatedMemory();
	used_count-=GetUsedMemory();
#endif
		free(d2d); };

	void	InitD2D(int num, int size, int static_size = 0, int size2 = 0, int num2 = 0);
	void	Clear1() {
#if MEM_GEOM_DEBUG
	used_count-=GetArrSize1();
#endif
		if (d2d) d2d->last1 = d2d->data1;}

	void	Clear2() {
#if MEM_GEOM_DEBUG
	used_count-=GetArrSize2();
#endif
		if (d2d) d2d->last2 = d2d->data2-d2d->size2;}

	char	*GetStatic() {return (d2d)? (char *)(d2d + 1) : NULL;}

	char	*GetData1()	{return (d2d)? d2d->data1 : NULL;} 
	char	*GetData2()	{return (d2d)? d2d->data2 : NULL;} 

	char	*GetObj1(int id) {return (d2d)? d2d->data1 + id * d2d->size1 : NULL;} 
	char	*GetObj2(int id) {return (d2d)? d2d->data2 - id * d2d->size2 : NULL;}

	char*	AddObj1(char *new_obj = NULL, int id = -1);
	char*	AddObj2(char *new_obj = NULL, int id = -1);

	void	DelObj1(int id = -1);
	void	DelObj2(int id = -1);
//private:
	int		GetArrSize();
	int		GetArrSize1();
	int		GetArrSize2();
	void	SetArrSize1(int sz);
	void	SetArrSize2(int sz);
	void	AllocD2D(int num = -1);
	void	DelD2D();

	int		GetUsedMemory		()	{	return GetArrSize1()+GetArrSize2()+d2d->GetNumStatic()+sizeof(Array2Data); };
	int		GetAllocatedMemory	()	{	return GetArrSize()+sizeof(Array2Data);};
};

inline int Array2Data::GetArrSize()
{
	return (d2d)? (d2d->data2 - (char *)d2d) : 0;
}
inline int Array2Data::GetArrSize1()
{
	return (d2d)? (d2d->last1-d2d->data1) : 0;
}
inline int Array2Data::GetArrSize2()
{
	return (d2d)? (d2d->data2-d2d->last2-d2d->size2) : 0;
}
inline void Array2Data::SetArrSize1(int sz)
{
#if MEM_GEOM_DEBUG
	int usd=GetArrSize1();
	int alc=GetArrSize();
#endif
	if(!d2d)
	{
		AllocD2D(sz+d2d->size1+d2d->size2);
	}
	else if( d2d->data1+sz+d2d->size1>d2d->last2)
	{
		AllocD2D(GetArrSize2()+sz+d2d->size1+d2d->size2);
	}
	d2d->last1=d2d->data1+sz;
#if MEM_GEOM_DEBUG
	used_count+=GetArrSize1()-usd;
	allocated_count+=GetArrSize()-alc;
#endif
}
inline void Array2Data::SetArrSize2(int sz)
{
#if MEM_GEOM_DEBUG
	int usd=GetArrSize2();
	int alc=GetArrSize();
#endif
	if(!d2d)
	{
		AllocD2D(MAX(2*sz,d2d->size1+d2d->size2));
	}
	else if( d2d->data2-sz-d2d->size2<d2d->last1+d2d->size1)
	{
		AllocD2D(GetArrSize1()+sz+d2d->size1+d2d->size2);
	}
	d2d->last2=d2d->data2-sz-d2d->size2;
#if MEM_GEOM_DEBUG
	used_count+=GetArrSize2()-usd;
	allocated_count+=GetArrSize()-alc;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~ SimArray ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class SimArray
{
public:
	static double inc_arr;
	char*	SrtArr;
	int		arr_size;
	int		num_arr;
	int		obj_size;
	
	SimArray();
	~SimArray();
	void	AdvInitSimA(int start_size, double delta);
	void	InitSimA(int size);
	void	ClearSimA();
	void	DelSimA(int id);
	int		AddSimA(char *new_obj, int id);
};

//~~~~~~~~~~~~~~~~~~~~~~ SimArray ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class SortedArray : public SimArray
{
public:
	int	(*CmpSrtArr)(char *i1, char *i2);
	
	SortedArray();
	void	InitSA(int size, int (*cmp_fun)(char *i1, char *i2));
	int		AddSA(char *new_obj);
	int		FindSA(char *new_obj, int &dir);
	int		IsValidSA();
};

//~~~~~~~~~~~~~~~~~~~~~~ QuickListObj ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class QuickArrayObj;
// item of the list should have fist field in the structure: "next"
class QuickListObj
{
public:
	QuickListObj *next;
	QuickListObj *prev;
	QuickArrayObj *parent;
};

//~~~~~~~~~~~~~~~~~~~~~~ QuickArrayObj ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class QuickArrayObj
{
public:
	int num;			// number of the list items in the qlo-range
	QuickListObj *qlo;	// first list item
};
//~~~~~~~~~~~~~~~~~~~~~~ QuickList ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class QuickList
{
public:
	QuickArrayObj*	Items;
	int				arr_size;		// allocated size
	double			inc_arr;		// reallocation increment
	int				num_arr;		// number of items in the Items[] array
	int				no;				// number of objects in the list_head list
	int				mid_in_group;	// optimal number of items in the one group
	int				max_in_group;	// maximal number of items in the one group
	int				diff_in_group;	// the maximal deviation from the mid_in_group
	int				max_static;		// initial maximal number of items in the one group
	int				i_prev;			// last range
	QuickListObj*	last_prev;
	QuickListObj*	list_head;		// list header
	int				(*CmpCLO)(QuickListObj *i1, QuickListObj *i2);

	QuickList(int num = 0);
	~QuickList();
	void			AdvInitQL(int start_size, double delta, int max_st, int mid_in_gr);
	void			SetCmpFunc(int (*cmp_fun)(QuickListObj *i1, QuickListObj *i2));
	void			Init(QuickListObj *HeadList = NULL, int num = 0);
	inline void		Refresh();
	QuickListObj*	FindQuickListObj(QuickListObj *new_obj);
	void			AddQuickListObj(QuickListObj *new_obj);
	QuickListObj*	DelQuickListObj(QuickListObj *del_obj = NULL);
	void			SwapItems(QuickListObj *prev, QuickListObj *a);
	int				CheckList();
	int				RefreshRange(int id0, int id1);
};

inline void	QuickList::SetCmpFunc(int (*cmp_fun)(QuickListObj *i1, QuickListObj *i2))
{
	CmpCLO = cmp_fun;
}
//~~~~~~~~~~~~~~~~~~~~~~ MemQL ~~~~~~~~~~~~~~~~~~~~~~~~~~~
class MemQL : public QuickList
{
public:
	void InitMemQL(int size, int (*cmp_fun)(QuickListObj *i1, QuickListObj *i2));
	void ClearMemQL();
	void DelQLO(QuickListObj *del_obj = NULL);
	QuickListObj *AddQLO(QuickListObj *new_obj);
	
	glPageMem pm;
	int item_size;
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#define MAX_YY 16
class HYPnt
{
public:
	long y;
	short ovl;
	short dir;
};

class PageTreeNode
{
public:
	char *GetFirstPTI();
	int GetNumPTI();
	PageTreeNode *GetNextPage();
	PageTreeNode *GetPrevPage() {return prev_page;};
	int GetLevelDelta(PageTreeNode *root = NULL);
public:
	PageTreeNode *left_ptn;
	PageTreeNode *right_ptn;
	PageTreeNode *root_ptn;
	PageTreeNode *next_page;
	PageTreeNode *prev_page;
	short num_yy;
	short delta;
};

inline char *PageTreeNode::GetFirstPTI()
{
	return (char *)(this + 1);
}
inline int PageTreeNode::GetNumPTI()
{
	return (int)num_yy;
}

inline int PageTreeNode::GetLevelDelta(PageTreeNode *root)
{
	int i = 0;
	PageTreeNode *n = this;
	for (; n && n != root; n = n->root_ptn, i++);
	return i;
}

inline PageTreeNode *PageTreeNode::GetNextPage()
{
	return next_page;
#if 0
	if (right_ptn) {
		PageTreeNode *n = right_ptn;
		for (; n && n->left_ptn; n = n->left_ptn);
		return n;
	} else {
		PageTreeNode *n = this;
		for (; n && n->root_ptn; n = n->root_ptn) {
			if (n->root_ptn->left_ptn == n) {
				return n->root_ptn;
			}
		}
		return NULL;
	}
#endif
}

class DynamicPageTree
{
public:
	void InitDPT();
	void ClearDPT(glPageMem &pm);
public:
 	int num_call;		// events counter (insert + delete events)
	int num_items;		// the number of PT items
	int num_pages;		// the number of PT vertices
	int num_levels;	// the number of PT levels
	PageTreeNode *root;			// top of the tree
	PageTreeNode *first_page;	// first PT node
};

//
class PageTree
{
public:
	PageTree();
	//~PageTree();

	void	AdvInitPT(int max_pg, int max_pt, double opt_num, int rel_pg, int lin_rng, int num_cl);
	void	InitPT(int item_size, int (*cmp_fun)(char *i1, char *i2));
	void	ClearPT();
	
	int	InsertItem(char *pt);	//??
	void	DeleteItem(char *pt);
	void	DeleteItem(PageTreeNode *m = NULL, int k = -1);
	int	GetPosition(char *pt, PageTreeNode *src, int level);
public:
	int IsValidPT(DynamicPageTree *v = NULL, int l = 1);
	PageTreeNode *NewPTN();
	void DelPTN(PageTreeNode *n);
	void SetCmpFunc(int (*cmp_fun)(char *i1, char *i2));

	int ChkBalance();
	int Balance();
	int GetItemPosition(PageTreeNode *n, char *pt, int &dir);

	int InsertInPage(PageTreeNode *n, char *pt, int k);
	int InsertInternal(PageTreeNode *n, char *pt, int k);

	int DelFromPage(PageTreeNode *m, int k);
	int DelNodeFromTree(PageTreeNode *m);

	void MoveToNext(PageTreeNode *n, int k);
	void MoveToPrev(PageTreeNode *n, int k);
	void MoveToPrev0(PageTreeNode *n, int k);
	void MoveNN(PageTreeNode *to_node, int id_to, PageTreeNode *from_node, int id_from, int num_pti);
	void MoveN1(PageTreeNode *to_node, int id_to, void *from_PTI);
	int GetFromNext(PageTreeNode *n, int k);
	int PtInRange(char *pt, PageTreeNode *n);
	int PtIndex(char *pt);
	
	void PushOff(PageTreeNode *n, int id);
	int LiftUp(PageTreeNode *n, int id, char &pt);
	void	SetItemSize(int size) {item_size = size;};

	int (*CmpItems)(char *i1, char *i2);
	int max_pti;		// the maximal number of items in the page
	int item_size;		//
	int opt_size;		// the optimal number of items in the page
	int num_released;	// N*opt_size, where N - the number of released pages after balancing
	int num_bln_chk;	// num calls between balance checking
	int lin_range;		// the range for the binary searching
	PageTreeNode *crn_node;
	int crn_id;
	int crn_dir;
	int crn_level;

	DynamicPageTree *dpt;
	DynamicPageTree dpt0;
	glPageMem pm;
	int items_in_pm;
};
inline void	PageTree::SetCmpFunc(int (*cmp_fun)(char *i1, char *i2))
{
	CmpItems = cmp_fun;
}

inline void PageTree::MoveToNext(PageTreeNode *n, int k)
{
	//ASSERT_WRN(n && n->next_page && 0 < k && k <= n->num_yy && n->next_page->num_yy + k <= max_pti, "MoveToNext: input error");
	MoveNN(n->next_page, k, n->next_page, 0, n->next_page->num_yy);
	n->next_page->num_yy = n->next_page->num_yy + (short)k;
	n->num_yy = n->num_yy - (short)k;
	MoveNN(n->next_page, 0, n, n->num_yy, k);
}
inline void PageTree::MoveToPrev(PageTreeNode *n, int k)
{
	//ASSERT_WRN(n && n->prev_page && 0 < k && k <= n->num_yy && n->prev_page->num_yy + k <= max_pti, "MoveToPrev: input error");
	MoveNN(n->prev_page, n->prev_page->num_yy, n, 0, k);
	n->prev_page->num_yy = n->prev_page->num_yy + (short)k;
	n->num_yy = n->num_yy - (short)k;
	if (n->num_yy) MoveNN(n, 0, n, k, n->num_yy);
}
inline void PageTree::MoveToPrev0(PageTreeNode *n, int k)
{
	//ASSERT_WRN(n && n->prev_page && 0 < k && k <= n->num_yy && n->prev_page->num_yy + k <= max_pti, "MoveToPrev: input error");
	MoveNN(n->prev_page, n->prev_page->num_yy, n, 0, k);
	n->prev_page->num_yy = n->prev_page->num_yy + (short)k;
}
inline int PageTree::ChkBalance()
{
	if (++dpt->num_call > num_bln_chk) {
		dpt->num_call = 0;
		if (dpt->num_pages * opt_size - dpt->num_items > num_released ||					// pages are not balanced
			 dpt->num_pages < (1 << (dpt->num_levels - 1)) + (1 << (dpt->num_levels - 2))) {	// level(root) == 0
			return Balance();
		}
	}
	return 0;
}

//////////////// Iter ///////////////
class PageTreeIter
{
public:
	PageTreeIter();
	PageTreeIter(PageTree *pt, DynamicPageTree *dpt = NULL);
	char *InitIterPt(PageTree *pt, DynamicPageTree *dpt = NULL);
	char *GetFirstPt();
	char *GetLastPt();
	char *GetNextPt();
	char *GetPrevPt();
	char *Go(char *pt);
public:
	char *SetTreeNode(PageTreeNode *node);
public:
	PageTree *pt;
	DynamicPageTree *dpt;
	PageTreeNode *ptn;
	char *crn;
	char *crn_l;
	char *crn_f;
	int len;
};

/////////////// MISC ////////////////////
inline void PageTree::MoveNN(PageTreeNode *to_node, int id_to, PageTreeNode *from_node, int id_from, int num_pti)
{
	if (num_pti > 0) {
		void *t = (void *)(to_node->GetFirstPTI() + id_to*item_size);
		void *f = (void *)(from_node->GetFirstPTI() + id_from*item_size);
		memmove(t, f, (unsigned int)(item_size*num_pti));
	}
}
inline void PageTree::MoveN1(PageTreeNode *to_node, int id_to, void *from_PTI)
{
	void *t = (void *)(to_node->GetFirstPTI() + id_to*item_size);
	memmove(t, from_PTI, (unsigned int)item_size);
}
inline int PageTree::PtInRange(char *pt, PageTreeNode *n)
{
	int k = (pt - n->GetFirstPTI()) / item_size;
	if (0 <= k && k < n->num_yy) {
		crn_node = n;
		crn_id = k;
	} else {
		k = -1;
	}
	return k;
}
inline int PageTree::PtIndex(char *pt)
{
	if (PtInRange(pt, crn_node) < 0) {
		PageTreeNode *n = dpt->first_page;
		for (; n && PtInRange(pt, n) < 0; n = n->next_page);
	}
	return crn_id;
}
inline void	PageTree::DeleteItem(char *pt)
{
	if (PtIndex(pt) >= 0) DelFromPage(crn_node, crn_id);
}
inline void	PageTree::DeleteItem(PageTreeNode *m, int k)
{
	if (k < 0 || m == NULL) {
		DelFromPage(crn_node, crn_id);
	} else {
		DelFromPage(m, k);
	}
}

#endif // __GL_QLIST_H__

