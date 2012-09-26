/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_ALLOCATORS_H__
#define __SCL_ALLOCATORS_H__

#include "stddef.h"
#include "malloc.h"

#ifdef _DEBUG
#define SCL_TRACK_MEMORY_ALLOCATIONS
#endif

namespace SCL
{

struct SYSCORE_API RuntimeClassBase
{
// Attributes
	const char * m_pszObjectName;
	size_t m_nObjectSize;

	virtual void CreateObject(void * p) const
	{
	}

	virtual void * CreateObject() const
	{
		return NULL;
	}
};

template <class T>
struct RuntimeClass : public RuntimeClassBase
{
	RuntimeClass(const char * pszName = NULL)
	{
		m_pszObjectName = pszName;
		m_nObjectSize = sizeof(T);
	}

	virtual void CreateObject(void * p) const
	{
		new (p) T;
	}

	virtual void * CreateObject() const
	{
		return new T;
	}
};

class SYSCORE_API Pool
{
	friend class MultiPool;

public:
	struct EmptyListNode
	{
#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
		int m_nAllocationNumber;
        void * m_pData;
#endif
		EmptyListNode * m_pNext;
	};

protected:
	EmptyListNode * m_EmptyList;

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
	unsigned m_nAmountAllocated;

	static EmptyListNode * NodeFromPtr(void * p)
	{
		return (EmptyListNode *)((char *)p - (int)offset);
	}
	static void * PtrFromNode(EmptyListNode * p)
	{
		return &p->m_pNext;
	}

public:
	struct Watcher
	{
		virtual void OnAlloc(void * pvData, size_t a_Size) = 0;
		virtual void OnFree(void * pvData, size_t a_Size) = 0;
		virtual void OnWalk(void * pvData, size_t a_Size) = 0;
		virtual void Dump(unsigned first, unsigned last) = 0;
	};

protected:
	Watcher * m_pWatcher;

#else
	static EmptyListNode * NodeFromPtr(void * p)
	{
		return (EmptyListNode *)p;
	}
	static void * PtrFromNode(EmptyListNode * p)
	{
		return p;
	}
#endif

	enum {
		PAGE_SIZE = 4096 // must be power of 2
		, PAGE_MASK = (~(PAGE_SIZE - 1))
		, SIGSTRING = 0x706f6f6c // "pool"
	};

	struct Page
	{
		Page * m_pNext;
		int m_Sig;
		Pool * m_pPool;
		int m_Count;
	};

	Page * m_PageList;

	int m_Size;
	bool m_bFill;

	void AllocatePage();

public:	

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
		static int offset;
		//offset = (int)&((EmptyListNode *)NULL)->m_pNext
#else
		static const int offset;
#endif

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
	Watcher * SetWatcher(RuntimeClassBase * a_Watcher);

	void Walk(unsigned first, unsigned last);
#endif

	Pool(int a_Size = 4);
	~Pool();

	inline bool SetFill(bool a_bFill)
	{
		bool retval = m_bFill;
		m_bFill = a_bFill;
		return retval;
	}

	inline void SetSize(int a_Size)
	{
		m_Size = a_Size;
	}

	inline int GetSize()
	{
		return m_Size;
	}

	inline void * Alloc()
	{
		if ( m_EmptyList == NULL ) {
			AllocatePage();
		}

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
		if ( m_pWatcher ) {
			m_pWatcher->OnAlloc(m_EmptyList, m_Size);
		}
		m_nAmountAllocated += GetSize();
#endif		
		void * p = Pool::PtrFromNode(m_EmptyList);
		m_EmptyList = m_EmptyList->m_pNext;

		return p;
	}

	inline void Free(void * p)
	{
		EmptyListNode * pNode = Pool::NodeFromPtr(p);
		pNode->m_pNext = m_EmptyList;
		m_EmptyList = pNode;
	}

	static Pool * FromPointer(void * p)
	{
		//!!! non-portable begin
		union {
			int m_int;
			void * m_p;
		} x;
		x.m_p = p;
		x.m_int &= PAGE_MASK;
		Page * pPage = (Page *)x.m_p;
		//!!! non-portable end

#ifdef _DEBUG
        if ( pPage->m_Sig == SIGSTRING ) {
			return pPage->m_pPool;
		}
		return NULL;
#else

		return pPage->m_pPool;
#endif
	}

	int CountPages();
	bool CheckIntegrity();
};

class SYSCORE_API MultiPool
{
	friend class alloc;

protected:
	enum {
		SIZE_STEP = 4
		, POOL_SIZE = 256
		, MAX_OBJ_SIZE = 1024  // object sizes from 4 to 1024
	};

	Pool m_Pool[POOL_SIZE];

	unsigned m_nAllocationNumber;

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
	unsigned m_nAmountAllocated;
	Pool::Watcher * m_pWatcher;
#endif

public:
	MultiPool();
	~MultiPool();

    size_t max_size() const
	{
		return MAX_OBJ_SIZE;
	}

	inline void * allocate(size_t nSize, const void * hint)
	{
		if ( nSize == 0 ) {
			return NULL;
		}
		if ( nSize >= MAX_OBJ_SIZE ) {
			return malloc(nSize);
		}

		SCL_ASSERT(nSize > 0 && nSize < MAX_OBJ_SIZE);

		nSize += Pool::offset;
		size_t i = (nSize + SIZE_STEP - 1) / SIZE_STEP - 1;
		SCL_ASSERT(i >= 0 && i < POOL_SIZE);

		Pool * pPool = m_Pool + i;
		if ( pPool->m_EmptyList == NULL ) {
			pPool->AllocatePage();
		}

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
		pPool->m_EmptyList->m_nAllocationNumber = m_nAllocationNumber++;
		if ( m_pWatcher ) {
			m_pWatcher->OnAlloc(pPool->m_EmptyList, nSize);
		}
		m_nAmountAllocated += pPool->GetSize();
		pPool->m_nAmountAllocated += pPool->GetSize();

		CheckIntegrity();
#endif
		void * p = Pool::PtrFromNode(pPool->m_EmptyList);
		pPool->m_EmptyList = pPool->m_EmptyList->m_pNext;

		return p;
	}

	inline void deallocate(void * p, size_t nSize = 0)
	{
		SCL_ASSERT(nSize >= 0);
		if ( nSize >= MAX_OBJ_SIZE ) {
			free(p);
			return;
		}

		size_t i;
		Pool * pPool;
        if  ( nSize ) {
            // this allows for very fast release-mode fixed size deallocations,
            // as compiler, when a constant is passed as nSize, optimizes out
            // everything but the final assignment
			nSize += Pool::offset;
			i = (nSize + SIZE_STEP - 1)/ SIZE_STEP - 1;
			pPool = m_Pool + i;
			SCL_ASSERT(pPool == Pool::FromPointer(p));
		} else {
			pPool = Pool::FromPointer(p);
			i = pPool - m_Pool;
		}
		if ( i < 0 || i >= POOL_SIZE ) {
			free(p);
			return;
		}
		Pool::EmptyListNode * pNode = Pool::NodeFromPtr(p);
		pNode->m_pNext = pPool->m_EmptyList;
		pPool->m_EmptyList = pNode;

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
		if ( pPool->m_pWatcher ) {
			pPool->m_pWatcher->OnFree(pNode, nSize);
		}
		pPool->m_EmptyList->m_nAllocationNumber = -1;
		m_Pool[i].m_nAmountAllocated -= pPool->GetSize();
		m_nAmountAllocated -= pPool->GetSize();

		CheckIntegrity();
#endif
	}

	bool CheckIntegrity();

#ifdef SCL_TRACK_MEMORY_ALLOCATIONS
	unsigned GetAmountAllocated()
	{
		return m_nAmountAllocated;
	}
	int GetLastAllocationNumber()
	{
		return m_nAllocationNumber;
	}

	Pool::Watcher * SetWatcher(RuntimeClassBase * a_Watcher);

	void Walk(unsigned first, unsigned last);

	void Dump(unsigned first, unsigned last);
#endif
};

extern SYSCORE_API MultiPool Alloc;

class SYSCORE_API alloc
{
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    static inline char * _Charalloc(size_type n)
	{
		return (char *)Alloc.allocate(n, NULL);
	}

    static inline void _Chardealloc(void * p, size_type nSize)
	{
		Alloc.deallocate(p, nSize);
	}

	static inline void * allocate(size_type n, void * pHint)
	{
		return Alloc.allocate(n, pHint);
	}
	static inline void deallocate(void * p, size_t nSize)
	{
		Alloc.deallocate(p, nSize);
	}
    size_t max_size() const
	{
		return Alloc.max_size();
	}
};

class SYSCORE_API PooledObject
{
public:
	inline void * operator new(size_t nSize)
	{
		return Alloc.allocate(nSize, NULL);
	}

	inline void operator delete(void * p)
	{
		if ( p ) {
			Alloc.deallocate(p);
		}
	}

	inline void * operator new(size_t nSize, void * p)
	{
		return p;
	}

	inline void operator delete(void * p, void *)
	{
	}
};

template <class T>
class allocator : public alloc
{
public:
    typedef T * pointer;
    typedef const T * const_pointer;

    typedef T & reference;
    typedef const T & const_reference;

    typedef T value_type;

	template<class _Other>
	struct rebind
	{
		typedef allocator<_Other> other;
	};

    static pointer address(reference x)
	{
		return &x;
	}

    static const_pointer address(const_reference x)
	{
		return &x;
	}

	allocator()
	{
	}

    allocator<T> & operator = (const allocator<T>)
	{
		return *this;
	}

    bool operator == (const allocator<T> &)
	{
		return true;
	}

    static pointer allocate(size_type n, const void * hint = NULL)
	{
		return (pointer)Alloc.allocate(n * sizeof(T), hint);
	}

    static void deallocate(void * p, size_type n)
	{
		Alloc.deallocate(p, n * sizeof(T));
	}

    static void construct(pointer p, const T & val)
	{
		new (p) T(val);
	}

    static void destroy(pointer p)
	{
		p->~T();
	}
};

template <class T>
class noreturn_allocator
{
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

    typedef T * pointer;
    typedef const T * const_pointer;

    typedef T & reference;
    typedef const T & const_reference;

    typedef T value_type;

protected:
	size_type size;
	char * buf, * s;

public:
	template<class _Other>
	struct rebind
	{
		typedef allocator<_Other> other;
	};

	noreturn_allocator(size_t a_size = 1000000)
	{
		size = a_size;
		s = buf = NULL;
	}

	~noreturn_allocator()
	{
		delete [] buf;
	}

	void * _Charalloc(size_t a_size)
	{
		if ( !buf ) {
			s = buf = new char[size];
		}
		void * retval = s;
		s += a_size;
		SCL_ASSERT(s - buf < (int)size);
		return retval;
	}

	void _Chardealloc(void * p, size_t a_size)
	{
	}

    pointer allocate(size_type n, const void * hint = NULL)
	{
		return (pointer)_Charalloc(n * sizeof(T), hint);
	}

	void deallocate(void * p, size_type size)
	{
	}
};

} // namespace SCL

#endif // __SCL_ALLOCATORS_H__

