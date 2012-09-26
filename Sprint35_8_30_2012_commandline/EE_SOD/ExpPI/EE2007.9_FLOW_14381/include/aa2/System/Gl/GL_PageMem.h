//////////////////////////////////////////
// Name:		GL_PageMem.h
// Date:		08/20/03
// Authors:		Alexander Zabolotnikov & Vasily Alexeev	
// Description:	memory controller glPageMem
//////////////////////////////////////////

#ifndef GL_PAGE_MEM_H
#define GL_PAGE_MEM_H

#include <string.h>	//???

class glPageMem;
class PageItem;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/////////////////////////////////////////////////////////////////////
//
// PageItem - represents a mask for the page atom
//	next - is used for free item, points to the next free item
//
/////////////////////////////////////////////////////////////////////

class PageItem
{
public:
	PageItem *next;
};

/////////////////////////////////////////////////////////////////////
//
// glPageMem - header for the page manager
//	------------------------------------
// free_item - free item in the page
//	page - pointer to the first item of first page in chain,
//			 page->next - to the next page, so first item in the
//			 each page is used to support page list
//	num  - the number of items in one page
//	step - item size (in longs)
//
/////////////////////////////////////////////////////////////////////

class SYSGL_API glPageMem
{
public:
	glPageMem();
	~glPageMem();

	void					Init						(int a_size, int a_count);
	void					Clear						();
	void					ClearItems				();
	void				*	NewItem					();
	void				*	NewItem					(bool a_clear);
	void				*	GetFirstItem			();
	void				*	GetNextItem				();

	int					GetPageCount			();
	void				*	GetPageBeg				();
	void				*	GetPageEnd				();
	void				*	GetNextPage				();

	void					DelItem					(void * item);
	void					FreeAll					();
	void					DelAll					();
	void					Swap					(glPageMem & other);
	int						GetIdx					(void * item);
	void				*	GetItm					(int idx);

public:

	// temporary variables
	PageItem	*	last;
	PageItem	*	tmp;

	PageItem	*	crn_page;
	PageItem	*	crn_item;

	PageItem	*	free_item;
	PageItem	*	free_page;
	PageItem	*	page;

	int				num;
	int				step;

	void			NewPage					(bool a_clear = FALSE);
	void			AddPageToFreeItems	(PageItem *, bool a_clear = FALSE);

	// statistics
#ifdef GL_MEM_DEBUG
	static	glPageMem	*first_pm;
	glPageMem			*next_pm;
	glPageMem			*prev_pm;
	int					id_pm;
#endif
	static	int		CheckMemPm(char *file = NULL);
	int				GetPagesCount			();
	int				GetItemsInPageCount	();
	int				GetAllItemsCount		(int = -1);
	int				GetUsedItemsCount		(int = -1);
	int				GetFreeItemsCount		();

	void			DelFreePages			();

};

/////////////////////////////////////////////////////////////////////
// Clear
/////////////////////////////////////////////////////////////////////

inline void glPageMem::Clear() 
{ 
	FreeAll();
}

/////////////////////////////////////////////////////////////////////
// NewItem - get pointer to new item
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::NewItem()
{
	if (!free_item) {
		NewPage();
	}
	tmp = free_item;
	free_item = free_item->next;
	return (void *)tmp;
}

/////////////////////////////////////////////////////////////////////
// NewItem - get pointer to new item
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::NewItem(bool a_clear)
{
	if (a_clear) {
		void	*	ptr = NewItem();
		memset(ptr, 0, step * sizeof(PageItem));
		return ptr;
	} else {
		return NewItem();
	}
}

/////////////////////////////////////////////////////////////////////
// DelItem - delete item - add item to chain of free items
/////////////////////////////////////////////////////////////////////

inline void glPageMem::DelItem(void *item)
{
	if (item) {
		((PageItem *)item)->next = free_item;
		free_item = (PageItem *)item;
	}
}

/////////////////////////////////////////////////////////////////////
// Get pages counter
/////////////////////////////////////////////////////////////////////

inline int glPageMem::GetPageCount()
{
	return step ? (num / step - 1) : 0;
}

/////////////////////////////////////////////////////////////////////
// Get current page begin data
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::GetPageBeg()
{
	crn_page = page;
	if (crn_page) {
		return (void *)(crn_page + step);	// without header
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Get current page end data
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::GetPageEnd()
{
	crn_page = page;
	if (crn_page) {
		return (void *)(crn_page + num);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Get next page
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::GetNextPage()
{
	if (crn_page) {
		crn_page = crn_page->next;
	}
	if (crn_page) {
		return (void *)(crn_page + step);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Get first item in current page
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::GetFirstItem()
{
	if (page) {
		crn_page = page;
		last = page + num;
		crn_item = page + step;
		if (crn_item < last) {
			return (void *)crn_item;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Get next item in current page
/////////////////////////////////////////////////////////////////////

inline void * glPageMem::GetNextItem()
{
	if (crn_page) {
		crn_item = crn_item + step;
		if (crn_item < last) {
			return (void *)crn_item;
		}
		crn_page = crn_page->next;
		if (crn_page) {
			last = crn_page + num;
			crn_item = crn_page + step;
			if (crn_item < last) {
				return (void *)crn_item;
			}
		}
	}
	return NULL;
}

#endif // GL_PAGE_MEM_H

