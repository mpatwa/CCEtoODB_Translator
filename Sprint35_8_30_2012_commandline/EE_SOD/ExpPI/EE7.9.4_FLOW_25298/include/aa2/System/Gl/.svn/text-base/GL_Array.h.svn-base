
#ifndef GL_Array_H
#define GL_Array_H

//#include <afxtempl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include "ut_types.h"

//#define SDB_EXPORT

////////////////////////////////////////////////////////////////////////
// GL_Array class
////////////////////////////////////////////////////////////////////////

//class SDB_EXPORT GL_Array
class SYSGL_API GL_Array
{
public:
	GL_Array ();
	~GL_Array ();

#ifdef GL_MEM_DEBUG
	int		gla_id;
	GL_Array *gla_next;
	GL_Array *gla_prev;
#endif
	static int CheckMem (char *fname = NULL);

	void				Free (void);
	// Set parameters
	bool			Init (const int sec_size, const int divisible);
	// Allocate
	bool			Alloc (const int sec_num,
							 const bool flag_copy = true,
							 const bool is_num = true,
							 const int divisible = -1);
	bool			Compress (void);
	bool			Copy (const GL_Array &source);
	void			*	Delete (const int sec_index,
					  			const int sec_count = 1);
	// Add
	void			*	Create (const int sec_count = 1,
					  			const int divisible = -1);
	void			*	Create (const void *data,
							   const int sec_count = 1,
					  			const int divisible = -1);
	void			*	Create (const GL_Array &source,
					  			const int divisible = -1);
	void			*	Insert (const int sec_index,
					  			const void *data = NULL,
					  			const int sec_count = 1,
					  			const int divisible = -1);
	void			*	Insert (const int sec_index,
								const GL_Array &source,
					  			const int divisible = -1);
	// Access
	void			*	Set (const int sec_index,
				  			const void *data,
				  			const int sec_count = 1);
	void			*	Get (const int sec_index,
				  			void *data = NULL,
				  			const int sec_count = 1);
	// Misc
	int					GetCount (void) const;
	bool			SetCount (const int sec_num);
	int					GetDivisible (void) const;
	bool			SetDivisible (const int divisible);
	int					GetMaxCount (void);
	void			*	GetData (void) const;
	int					GetSecSize (void) const;
	void			*	GetSecPtr (const int sec_index) const;
protected:
	char			*	p_data;
	int					n_sec_max;
	int					n_sec_num;
	int					n_sec_size;
	int					n_divisible;
};
////////////////////////////////////////////////////////////////////////
// GL_SortArray class
////////////////////////////////////////////////////////////////////////

typedef int (*qsort_func_typedef)(const void *, const void *);

class SYSGL_API GL_SortArray : virtual public GL_Array
{
public:
	// Set parameters
	bool			Init (const int sec_size,
							const int divisible);
	void				SetSortFunction (qsort_func_typedef sort_fun);
	bool			Sort (void);
	void			*	CreateSorting (const void *data,
										const int sec_count = 1);
	void			*	CreateSorting (const GL_Array &source);
	void			*	CreateSorting (const GL_SortArray &source);
protected:
	qsort_func_typedef	p_sort_fun;
};
////////////////////////////////////////////////////////////////////////
// GL_ArrayPtr class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_ArrayPtr : virtual public GL_Array
{
public:
	// Set parameters
	bool			Init (const int divisible);
	// Allocate
	void			*	Delete (const int sec_index,
							  const int count = 1);
	// Add
	void			*	Create (const void *data);
	void			*	Create (const GL_ArrayPtr &source);
	void			*	Insert (const int index,
							  const void *data);
	void			*	Insert (const int index,
							  const GL_ArrayPtr &source);
	// Misc
	void			*	Set (const int index,
				  			const void *data);
	void			*	Get (const int index);
protected:
	void			*	SetDataPtr (const void *ptr_arr);
};
////////////////////////////////////////////////////////////////////////
// GL_ArrayPtr class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_SortArrayPtr : virtual public GL_ArrayPtr,
									virtual public GL_SortArray
{
public:
	// Set parameters
	bool	Init (const int divisible);
	void			*	CreateSorting (const void *data);
	void			*	CreateSorting (const GL_ArrayPtr &source);
	void			*	CreateSorting (const GL_SortArrayPtr &source);
};
////////////////////////////////////////////////////////////////////////
//	GL_ArrayListPtr class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_ArrayListPtr : virtual public GL_ArrayPtr
{
public:
	// Set parameters
	bool	Init (const int divisible);
	void			*	Include (const void *data);
	void 			*	Include (const int index, const void *data);
	void			*	IncludeBefore (const void *data);
	void			*	IncludeAfter (const void *data);
	void			*	Exclude (void);
	void			*	Exclude (const int index);
	void			*	Get (const int index);
	void			*	Get (void);
	void			*	Set (const int index,
			 	  			const void *data);
	void			*	Set (const void *data);
	void			*	Find (const void *data);
	void			*	GetFirst (void);
	void			*	GetLast (void);
	void			*	GetNext (void);
	void			*	GetPrev (void);
protected:
	int	n_id;
};
////////////////////////////////////////////////////////////////////////
//	GL_SortArrayListPtr class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_SortArrayListPtr : public GL_SortArrayPtr,
                             public GL_ArrayListPtr
{
public:
	bool	Init (const int divisible);
	void			*	IncludeSorting (const void *data);
};
////////////////////////////////////////////////////////////////////////
// GL_ArrayStack class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_ArrayStackPtr : public GL_ArrayPtr
{
public:
	bool	Push (const void *data);
	void			*Pop (void);
};
////////////////////////////////////////////////////////////////////////
// GL_ArrayQueue class
////////////////////////////////////////////////////////////////////////
class SYSGL_API GL_ArrayQueuePtr : public GL_ArrayPtr
{
public:
	bool	Push (const void *data);
	void			*Pop (void);
};
////////////////////////////////////////////////////////////////////////
// GL_Array class
////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_Array::Create (const void *data,
				   			 const int sec_count,
								 const int divisible)
{
	int	old_sec_num = n_sec_num;
	if (!Alloc(n_sec_num + sec_count, true, true, divisible)) {
		return NULL;
	}
	void *ptr = (void *)(p_data + GetSecSize() * old_sec_num);
	if (data) {
		memcpy(ptr, data, GetSecSize() * sec_count);
	}
	return ptr;
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_Array::Create (const GL_Array &source,
								 const int divisible)
{
	return Create(source.p_data, source.n_sec_num, divisible);
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_Array::Create (const int sec_count,
								 const int divisible)
{
	return Create(NULL, sec_count, divisible);
}
/////////////////////////////////////////////////////////////////////////////
inline
int GL_Array::GetSecSize (void) const
{
	return n_sec_size;
}
/////////////////////////////////////////////////////////////////////////////
inline
int GL_Array::GetDivisible (void) const
{
	return n_divisible;
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_Array::GetSecPtr (const int sec_index) const
{
	return (void *)(p_data + GetSecSize() * sec_index);
}
/////////////////////////////////////////////////////////////////////////////
inline
int GL_Array::GetCount (void) const
{
	return n_sec_num;
}
/////////////////////////////////////////////////////////////////////////////
inline
int GL_Array::GetMaxCount (void)
{
	return n_sec_max;
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_Array::GetData (void) const
{
	return (void *)p_data;
}
////////////////////////////////////////////////////////////////////////
// GL_SortArray class
////////////////////////////////////////////////////////////////////////
inline
bool GL_SortArray::Init (const int sec_size,
											const int divisible)
{
	p_sort_fun = NULL;
	return GL_Array::Init(sec_size, divisible);
}
/////////////////////////////////////////////////////////////////////////////
inline
void GL_SortArray::SetSortFunction (qsort_func_typedef sort_fun)
{
	p_sort_fun = sort_fun;
}
////////////////////////////////////////////////////////////////////////
// GL_ArrayPtr class
////////////////////////////////////////////////////////////////////////
inline
bool GL_ArrayPtr::Init (const int divisible)
{
	return GL_Array::Init(sizeof(void *), divisible);
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Delete (const int index,
									 const int count)
{
	return SetDataPtr(GL_Array::Delete(index, count));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Create (const void *data)
{
	return SetDataPtr(GL_Array::Create(&data));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Insert (const int index,
									 const void *data)
{
	return SetDataPtr(GL_Array::Insert(index, &data));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Insert (const int index,
									 const GL_ArrayPtr &source)
{
	return SetDataPtr(GL_Array::Insert(index, source.p_data, source.n_sec_num));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Set (const int index,
								 const void *data)
{
	return SetDataPtr(GL_Array::Set(index, &data));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_ArrayPtr::Get (const int index)
{
	return SetDataPtr(GL_Array::Get(index));
}
////////////////////////////////////////////////////////////////////////
// GL_SortArrayPtr class
////////////////////////////////////////////////////////////////////////
inline
bool GL_SortArrayPtr::Init (const int divisible)
{
	return GL_SortArray::Init(sizeof(void *), divisible);
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_SortArrayPtr::CreateSorting (const void *data)
{
	return GL_ArrayPtr::SetDataPtr(GL_SortArray::CreateSorting(&data));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_SortArrayPtr::CreateSorting (const GL_ArrayPtr &source)
{
	return GL_ArrayPtr::SetDataPtr(GL_SortArray::CreateSorting(source.GetData(), source.GetCount()));
}
/////////////////////////////////////////////////////////////////////////////
inline
void *GL_SortArrayPtr::CreateSorting (const GL_SortArrayPtr &source)
{
	return GL_ArrayPtr::SetDataPtr(GL_SortArray::CreateSorting(source.p_data, source.n_sec_num));
}
////////////////////////////////////////////////////////////////////////
// GL_ArrayListPtr class
////////////////////////////////////////////////////////////////////////
inline
bool  GL_ArrayListPtr::Init (const int divisible)
{
	n_id = -1;
	return GL_ArrayPtr::Init(divisible);
}
////////////////////////////////////////////////////////////////////////
// GL_SortArrayListPtr class
////////////////////////////////////////////////////////////////////////
inline
bool GL_SortArrayListPtr::Init (const int divisible)
{
	n_id = -1;
	return GL_SortArray::Init(sizeof(void *), divisible);
}
////////////////////////////////////////////////////////////////////////
// GL_ArrayStackPtr class
////////////////////////////////////////////////////////////////////////
inline
bool GL_ArrayStackPtr::Push (const void *data)
{
	return (GL_ArrayPtr::Create(data) != NULL);
}
////////////////////////////////////////////////////////////////////////
// GL_ArrayQueuePtr class
////////////////////////////////////////////////////////////////////////
inline
bool GL_ArrayQueuePtr::Push (const void *data)
{
	return (GL_ArrayPtr::Create(data) != NULL);
}
////////////////////////////////////////////////////////////////////////
//	GCC_Page class
////////////////////////////////////////////////////////////////////////
typedef struct GL_PageHeader
{
	GL_PageHeader	*p_next_page;
	char	*p_data_page () { return (char *)(this + 1); }
} GL_PageHeader;
////////////////////////////////////////////////////////////////////////
class SYSGL_API GCC_Page
{
public:
	GCC_Page ();
	~GCC_Page ();
	bool InitPage (const int page_size);
	void *CreatePage ();
	void FreeAll ();
protected:
	int		 		n_page_size;
	// Put
	GL_PageHeader *p_curr_page;
	GL_PageHeader *p_first_page;
};
////////////////////////////////////////////////////////////////////////
class SYSGL_API GCC_Heap : public GCC_Page
{
public:
	GCC_Heap ();
	void *CreateSec (const int size, const void *data = NULL);
protected:
	void InitHeap ();
	char				*p_curr_ptr;
	char				*p_end_ptr;
};
////////////////////////////////////////////////////////////////////////
class SYSGL_API GCC_FixSec : public GCC_Heap
{
public:
	GCC_FixSec ();
	bool InitFixSec (const int sec_size, const int sec_in_page);
	void *CreateFixSec (const void *data = NULL);
	int GetCountFixSec (void) const
	{
		return n_sec_put_num;
	}
	void *GetFirstFixSec (void *data);
	void *GetNextFixSec (void *data);
	void *GetFixSec (const int sec_index, void *data);
protected:
	int		 		n_sec_size;	  		// Section size
	int		 		n_sec_in_page;		// Sections in one page
	// Put
	int				n_sec_put_num;
	// Get
	int				n_sec_get_num;
	char				*p_page_get;		// pointer in curr page
	int				n_sec_get;			// Curr get secs in curr page
};

#endif

//
// end of file
//

