/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:		GL_Global.h
// Date:		06.11.05
// Author:		Alexander Zabolotnikov	
// Description:	global parametrs description

#ifndef __GL_GLOBAL_H__
#define __GL_GLOBAL_H__

#include "GL_PageMem.h"
#include "GL_Array.h"

class GeomHdr;	// forvard definition
class IterPolyCross;
class SelPie;
class SelNode;
class PolyMemPool; //forward declaration
class SYSGL_API GL_Global
{
public:
	static int is_init_gl;
	GL_Global();
	~GL_Global();
	void InitGL();
	void ClearGL();
	void DeleteAllGL();
	glPageMem PolyMem;
	glPageMem CircleMem;
	glPageMem SelNodeMem;
	glPageMem SelPieMem;
};

class SYSGL_API GL_Base
{
public:
	GL_Base();
	~GL_Base();
	static	void InitGLB();
	// Poly & Circle
	static	GeomHdr *CreatePoly(int num_pnt, int num_arc = 0);
	static	GeomHdr *CreateCircle();
	static	GeomHdr *CreateCircle(GL_Point &cent, long rad);
	static	GeomHdr *CreateDCircle(GL_DPoint &cent, long rad);
	static	void	DelGeom(GeomHdr *gh);
	// select & unselect
	static	SelNode *CreateSelNode();
	static	void	DelSelNode(SelNode *sn);
	static	SelPie *CreateSelPie();
	static	void	DelSelPie(SelPie *sp);
	static	int		SelectGeom(GeomHdr *gh, int i1 = -1, int i2 = -1);
	static	int		UnSelectGeom(GeomHdr *gh, int i1 = -1, int i2 = -1);
	static	void	SelectGeomArray(IterPolyCross *it0, IterPolyCross *itl);
	static	void	UnSelectGeomArray(IterPolyCross *it0, IterPolyCross *itl);
	static	void	SelectGeomList(GeomHdr *gh);
	static	void	UnSelectGeomList(GeomHdr *gh);
	static void		DumpPolyMem(char *fname, char *mode);
	static inline void *Alloc(int array_size,int &corrected_size);
	static inline void *ReAlloc(void *oarry,int oldsize, int newsize,int & corrected_size) ;
	static inline void	free(void * arr, int size);
	static inline int	GetCorrectedSize(int num_arr);
	static inline int	GetMaxPolyArrSize();

private:
	static int is_init_glb;
	static GL_Global glb;
	static PolyMemPool PolyMgr;
};

class PolyAllocator : public glPageMem
{
public:
	int num_pages;	// the number of allocated pages
	int num_items;	// the number of used items
	int num_del_after_compress;	// to did not chek the compress too often
public:
	void Init(int a_size, int a_count);
	inline void *NewPolyItem();				// allocate new item and calculate num_pages
	inline void DelPolyItem(void *item);	// delete the item and check for the Compress
	inline int NeedCompress();				// quick inline checking

	//// compress
	//		- delete empty pages
	//		- recalculate the free items list
	//		- returns num pages
	int Compress();

	//// LinkFreeItemsOnPage()
	//	add all empty items of the given page to the free items list
	// returns the number of items on the page
	int LinkFreeItemsOnPage(PageItem *page);
};

//	 Memory Pool Class for Poly arcs/segements 
//	memory for polygons arcs/segements will be 
//	created using GL_Base::Alloc/GL_Base::ReAlloc
//	Memory will be released using GL_Base::free
//	Memory required for the poly arcs/segements is
//	not known when we create glPoly Objects. Memory 
//	grows as we keep adding segements/arcs
//	Implementation  of PolyMemPool contains buckets of 
//	fixed sized glPageMem objects.When ever a new request 
//	for the memory comes we do move from lower bucket to higher bucket
//	For ex: If the current memory is 64 bytees and if we get a request for 12 bytes
//	, we move the data  into 128 size bucket to accomidate the feature requests also
//	we maintain buckets in this format 64-->128-->256-->512-->1024-->2048
//	Based on the request ,we allocate memory from appropriate bucket.
//	Min bucket size is 128
//	Max bucket size is 2048
//	Number of buckets is (log 2048/128)+1 =5 
//
//

#define MAX_POLY_ARR_SIZE 2048		// the maximal number of the items on one page
#define MIN_POLY_ARR_SIZE 128		// the minimal size of one item on the page 
#define NBUCKETS 6					// the number of allocators
#define MAX_ALLOCATORS (2*NBUCKETS) // the maximal number of allocators
class PolyMemPool
{
public:
	PolyMemPool(){ InitPolyArr();}
	~PolyMemPool(){RemovePolyArr();}
	void * Alloc(int size,int & corrected_size);
	void * ReAlloc(void * Oldarray,int size, int newsize,int & corrected_size);
	void free(void * Oldarray, int size);
	int GetCorrectedSize(int num_arr);
	void InitPolyArr(int size = MIN_POLY_ARR_SIZE, int nbuckets = NBUCKETS, int psize = MAX_POLY_ARR_SIZE);
	void DumpPolyMem(char *fname, char *mode);
	void GetMemPoolParam(char *fname);
	inline int GetMaxPageSize();
	inline int GetMinArrSize();
	inline int GetNumMemCnt();

private:
	void RemovePolyArr();

	PolyAllocator PolyArr[MAX_ALLOCATORS];
	int min_poly_arr_size;	// the minimal size of the allocated memory
	int max_pmem_cnt;		// the number of page mem controllers in the PolyArr[]
	int max_page_size;		// the maximal number of the items on one page
};

inline int PolyMemPool::GetMaxPageSize() {return max_page_size;}
inline int PolyMemPool::GetMinArrSize() {return min_poly_arr_size;}
inline int PolyMemPool::GetNumMemCnt() {return max_pmem_cnt;}

// GL_Base:: inline functions related to PolyMemPool
inline int GL_Base::GetMaxPolyArrSize()
{
	return PolyMgr.GetMaxPageSize();
}
inline void *GL_Base::Alloc(int array_size,int & corrected_size) 
{
	return PolyMgr.Alloc(array_size,corrected_size);
}
inline void *GL_Base::ReAlloc(void *oarry,int oldsize, int newsize,int & corrected_size) 
{
	return PolyMgr.ReAlloc(oarry,oldsize,newsize,corrected_size);
}
inline void GL_Base::free(void * arr, int size)
{
	PolyMgr.free(arr,size);
}
inline int GL_Base::GetCorrectedSize( int num_arr)
{
	return PolyMgr.GetCorrectedSize(num_arr);
}



//~~*~~~~~~~~~~~ SelPie ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class SelPie
{
public:
	int beg_id;
	int end_id;
	SelPie *next_pie;
	void InitSelPie(int i1, int i2, SelPie *next);
};
//~~*~~~~~~~~~~~ SelNode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class SelNode : public GL_Node
{
public:
	SelPie first_pie;
};

//~~*~~~~~~~~~~~ DBG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AF_Table;
class dbg
{
public:
	// poly  in box
	static int	InBox(GeomHdr *gh);
	static int	InBoxG(GL_Point *pt);
	static int	InBoxL(long x, long y);
	static int	InBoxD(double x, double y);
	static int	InCorner(GeomHdr *gh, long x, long y, long eps = 0);
	static int	NumOvlSeg(GeomHdr *gh, GL_Point &a, GL_Point &b, long min_ovl = 4);
	static int	NumOvlBox(GeomHdr *gh, GL_Box &box, long min_ovl = 4);
	static void SetBox();
	// check nex_geom list
	static GL_Node *ExcludeRoot(GL_Node *prev);
	static int	CheckLoop(GeomHdr *f);
	// check list intersection
	static int	CheckListInt(GeomHdr *f1, GeomHdr *f2, int isList2 = 1);
	//check duplicating in array (gh == 0), or check: is gh in arr
	static int	ChkIter(GeomHdr *gh, GL_Array *arr);
	static void GetDPoint(char *name);
	static int  ChkDPoint(double x, double y);
	static int ChkDPoint2(long x1, long x2, long y1, long y2);
	static char *GdfFlag( unsigned long flg, unsigned long ignore_flg = 0);
	static char *ClearStrBuff();
	static char *SetStrBuff(int i = 0, int cl = 0);
	static char *AddStr(char *s1, char *s2 = NULL, char *s3 = NULL, char *s4 = NULL);
	static char *AddInt(int num);
	static char *AddStrInt(char *s1, int i1, char *s2 = NULL, int i2 = 0, char *s3 = NULL, int i3 = 0);
	static char *GetStr();
	static int	ListIsClosed (GeomHdr *first, char *mess);
	static int	fpoly(GeomHdr *gh, char *file_name, char *mode = NULL);
	static int	fpie(double t1, double t2, GeomHdr *gh, char *file_name, char *mode = NULL);
	static int  fpl(GL_Node *n, char *fn, unsigned short fl = SGN_FLAG_CENT | SGN_FLAG_SHELL);
	static int  fshell(GL_Node *n, char *fname);
	static int  fcent(GL_Node *n, char *fname);
	static int  fseg(GL_Point &ll, GL_Point &ur, char *file_name, char *mode = NULL);
	static int  fbox(GL_Point &ll, GL_Point &ur, char *file_name, char *mode = NULL);
	static FILE *open(char *file_name, char *mode);
	static void SetTemp(int id, int val);
	static int  GetTemp(int id = 0);
	static long heapGl(char *outFile = NULL);
	static long heapdump( char *srcFileMask = "*", char *outFile = NULL, char *outMode="w", bool group=false);
	static long heapdump( char **FileList, char *outFile = NULL, char *outMode="w", bool group=false);
	static void GroupDump(char *inf, char *outf);
	static GeomHdr *CheckClearances (AF_Table *af, long tolerance, int &num_err, char *fname = NULL, int del_err = 1);
public:
	static char *StrBuff;
	static GL_Point D_Pnt[];
	static GL_DPoint DD_Pnt[];
	static double ddeps;
	static int num_stdpnt;
	static int num_stpnt;
	static int num_char;
	static int is_chk_dpt;
	static int dump[];
	static double DD[];
	static GL_Array arr;
	static GL_Box dbox;
};

#endif //__GL_GLOBAL_H__

