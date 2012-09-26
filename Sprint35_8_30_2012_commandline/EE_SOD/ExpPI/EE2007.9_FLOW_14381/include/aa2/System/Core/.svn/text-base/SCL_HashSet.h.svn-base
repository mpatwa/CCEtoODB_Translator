/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_HASH_SET_H__
#define __SCL_HASH_SET_H__

#include <memory.h>
#include <string.h>

#include "SCL_Assert.h"
#include "SCL_Allocators.h"

#include "SCL_Pair.h"
#include "SCL_HashComp.h"

namespace SCL
{

template <class TKEY, class TRAITS> struct hash_iter;

template <class VAL, class HCACHE>
struct hash_node : public HCACHE
{
	hash_node * m_pNextHash;
	hash_node * m_pNext;
	hash_node * m_pPrev;

	VAL m_Value;
};

template <class VAL, class TRAITS>
struct hash_iter
{
public:
	typedef hash_node<VAL, typename TRAITS::node> node;
	typedef hash_iter<VAL, TRAITS> iterator;

protected:
	node * m_Focus;

	explicit hash_iter(node * x)
	{
		m_Focus = x;
	}

public:
	hash_iter()
	{
		m_Focus = NULL;
	}

	iterator & operator = (const iterator & x)
	{
		m_Focus = x.m_Focus;
		return *this;
	}

	const VAL & get_value() const
	{
		return m_Focus->m_Value;
	}

	void set_value(const VAL & x)
	{
		m_Focus->m_Value = x;
	}

	handle get_handle() const
	{
		return m_Focus;
	}

	void set_handle(handle a_handle)
	{
		m_Focus = (node *)a_handle;
	}

	operator VAL * () const
	{
		return m_Focus ? &m_Focus->m_Value : NULL;
	}

	VAL * operator -> () const
	{
		return &m_Focus->m_Value;
	}

	// prefix versions
	iterator & operator ++ ()
	{
		m_Focus = m_Focus->m_pNext;
		return *this;
	}
	iterator & operator -- ()
	{
		m_Focus = m_Focus->m_pPrev;
		return *this;
	}

	// postfix version
	iterator operator ++ (int)
	{
		node * retval = (node *)m_Focus;
		m_Focus = m_Focus->m_pNext;
		return iterator(retval);
	}
	iterator operator -- (int)
	{
		node * retval = (node *)m_Focus;
		m_Focus = m_Focus->m_pPrev;
		return iterator(retval);
	}
};

template <class TKEY, class TRAITS, class A, class N> class hash_set2;

template <class TKEY, class TRAITS = _hash_comp<TKEY>, class A = alloc, class N = NullNotifier>
class hash_set : public N
{
	friend class hash_set2<TKEY, TRAITS, A, N>;

public:
	typedef TKEY key_type;
	typedef const TKEY const_key_type;

	typedef TKEY value_type;
	typedef const TKEY const_value_type;

	typedef TRAITS key_compare;

	typedef typename A::size_type size_type;
	typedef typename A::difference_type difference_type;

	typedef A allocator_type;

	typedef hash_set<TKEY, TRAITS, A, N> set;
	typedef hash_iter<key_type, TRAITS> iterator;
	typedef hash_iter<const_key_type, TRAITS> const_iterator;

	typedef pair <iterator, bool> ibpair;

protected:

	typedef hash_node<key_type, typename TRAITS::node> node;
	typedef hash_node<const_key_type, typename TRAITS::node> const_node;

	typedef node * nodeptr;

	nodeptr * m_Table;
	int m_nSize, m_nTableSize;

	nodeptr m_pFirst, m_pLast;

	TRAITS m_HashComp;
	A m_Allocator;

	void * insert_node(const typename TRAITS::key_type & k)
	{
		if ( m_nSize >= m_nTableSize ) {
			resize(m_nSize + 1);
		}

		nodeptr * list;
		unsigned hval = m_HashComp(k);
		list = m_Table + (hval % m_nTableSize);

		nodeptr pNode;
		for ( pNode = *list; pNode; pNode = pNode->m_pNextHash ) {
			typename TRAITS::key_type key = TRAITS::cast(pNode->m_Value);
			unsigned hv = pNode->get_hval(key, m_HashComp);
			if ( m_HashComp.match(hv, hval, key, k, m_HashComp) ) {
				break;
			}
		}
		if ( !pNode ) {
			pNode = (nodeptr)m_Allocator._Charalloc(sizeof(node));
			pNode->set_hval(hval);
			pNode->m_pNextHash = *list;
			*list = pNode;

			if ( m_pLast ) {
				m_pLast->m_pNext = pNode;
			}
			pNode->m_pNext = NULL;
			pNode->m_pPrev = m_pLast;
			m_pLast = pNode;
			if ( !m_pFirst ) {
				m_pFirst = pNode;
			}

			++m_nSize;
		}

		return pNode;
	}

	void * lookup_node(const typename TRAITS::key_type & k) const
	{
		if ( !m_nSize ) {
			return NULL;
		}
		unsigned hval = m_HashComp(k);
		nodeptr list = m_Table[hval % m_nTableSize];
		nodeptr pNode;
		for ( pNode = list; pNode; pNode = pNode->m_pNextHash ) {
			typename TRAITS::key_type key = TRAITS::cast(pNode->m_Value);
			unsigned hv = pNode->get_hval(key, m_HashComp);
			if ( m_HashComp.match(hv, hval, key, k, m_HashComp) ) {
				break;
			}
		}
		return pNode;
	}

	void * erase(void ** p)
	{
		if ( *p ) {
			nodeptr * ppNode = (nodeptr *)p;
			nodeptr pNode = *ppNode;

			Notify(EventCode::Delete, pNode);

			nodeptr pNext = pNode->m_pNext, pPrev = pNode->m_pPrev;
			nodeptr pNextHash = pNode->m_pNextHash;
			if ( pPrev ) {
				pPrev->m_pNext = pNext;
			}
			if ( pNext ) {
				pNext->m_pPrev = pNode->m_pPrev;
			}
			if ( pNode == m_pFirst ) {
				m_pFirst = pNext;
			}
			if ( pNode == m_pLast ) {
				m_pLast = pPrev;
			}
			--m_nSize;
			pNode->~hash_node<key_type, typename TRAITS::node>();
			m_Allocator._Chardealloc(pNode, sizeof(node));
			*ppNode = pNextHash;
			return pNext;
		}
		return NULL;
	}

	void init(int nTableSize)
	{
		m_Table = NULL;
		m_pFirst = m_pLast = NULL;
		m_nSize = m_nTableSize = 0;
		if ( nTableSize ) {
			resize(nTableSize);
		}
	}

public:
	hash_set(int nTableSize = 0)
	{
		init(nTableSize);
	}

	explicit hash_set(const A & al, int nTableSize = 0)
		: m_Allocator(al)
	{
		init(nTableSize);
	}

	hash_set(const hash_set & x)
	{
		init(0);
		if ( x.m_nSize ) {
			m_nSize      = x.m_nSize;
			m_nTableSize = x.m_nTableSize;

			m_Table = (nodeptr *)m_Allocator._Charalloc(m_nTableSize * sizeof(nodeptr));
			memset(m_Table, 0, m_nTableSize * sizeof(nodeptr));
			for ( nodeptr x_pNode = x.m_pFirst, pNode = m_pFirst; x_pNode;
				  x_pNode = x_pNode->m_pNext, pNode = pNode->m_pNext ) {
				pNode = (nodeptr)m_Allocator._Charalloc(sizeof(node));
				const typename TRAITS::key_type & x_key = TRAITS::cast(x_pNode->m_Value);
				new (&pNode->m_Value) TKEY(x_pNode->m_Value);
				pNode->set_hval(x_pNode->get_hval(x_key, m_HashComp));

				const typename TRAITS::key_type & key = TRAITS::cast(pNode->m_Value);
				nodeptr * list = m_Table + (pNode->get_hval(key, m_HashComp) % m_nTableSize);
				pNode->m_pNextHash = *list;
				*list = pNode;

				if ( m_pLast ) {
					m_pLast->m_pNext = pNode;
				}

				pNode->m_pNext = NULL;
				pNode->m_pPrev = m_pLast;
				m_pLast = pNode;
				if ( !m_pFirst ) {
					m_pFirst = pNode;
				}
			} // for ( nodeptr x_pNode = x.m_pFirst, pNode = m_pFirst; x_pNode;x_pNode = x_pNode->m_pNext, pNode = pNode->m_pNext )
		}
	}

	~hash_set()
	{
		for ( nodeptr t = m_pFirst; t; ) {
			node * p = t;
			t = p->m_pNext;
			if ( p ) {
				p->m_Value.~TKEY();
				m_Allocator._Chardealloc(p, sizeof(node));
			}
		}
		if ( m_Table ) {
			m_Allocator._Chardealloc(m_Table, sizeof(nodeptr) * m_nTableSize);
		}
	}

	int size() const
	{
		return m_nSize;
	}

	void resize(int nSize)
	{
		int new_size = HashHelper::GetNewTableSize(nSize - 1);
		if ( new_size == m_nTableSize ) {
			return;
		}

		if ( m_Table ) {
			m_Allocator._Chardealloc(m_Table, sizeof(nodeptr) * m_nTableSize);
		}

		m_nTableSize = new_size;
		SCL_ASSERT(m_nTableSize);

		m_Table = (nodeptr *)m_Allocator._Charalloc(m_nTableSize * sizeof(nodeptr));
		memset(m_Table, 0, m_nTableSize * sizeof(nodeptr));

		for ( nodeptr pNode = m_pFirst; pNode; pNode = pNode->m_pNext ) {
			const typename TRAITS::key_type & key = TRAITS::cast(pNode->m_Value);
			nodeptr & p = m_Table[pNode->get_hval(key, m_HashComp) % m_nTableSize];
			pNode->m_pNextHash = p;
			p = pNode;
		}
	}

	bool empty() const
	{
		return (m_nSize == 0);
	}

	void clear()
	{
		for ( nodeptr t = m_pFirst; t; ) {
			node * p = t;
			t = p->m_pNext;
			p->m_Value.~TKEY();
			m_Allocator._Chardealloc(p, sizeof(node));
		}
		memset(m_Table, 0, sizeof(nodeptr) * m_nTableSize);
		m_pFirst = m_pLast = NULL;
		m_nSize = 0;
		this->Notify(EventCode::Clear);
	}

	iterator begin()
	{
		return (iterator &)m_pFirst;
	}

	iterator end()
	{
		return iterator();
	}

	const_iterator begin() const
	{
		return (const_iterator &)m_pFirst;
	}

	const_iterator end() const
	{
		return const_iterator();
	}

	iterator erase(iterator pos)
	{
		if ( pos && !empty() ) {
			const typename TRAITS::key_type & key = TRAITS::cast(*pos);
			unsigned idx = ((nodeptr &)pos)->get_hval(key, m_HashComp) % m_nTableSize;
			for ( nodeptr * ppNode = m_Table + idx; *ppNode; ppNode = &(*ppNode)->m_pNext ) {
				if ( *ppNode == (nodeptr &)pos ) {
					nodeptr pNext = (nodeptr)erase((void **)ppNode);
					return (iterator &)pNext;
				}
			}
		}
		return iterator();
	}

	int erase(const typename TRAITS::key_type & k)
	{
		if ( !empty() ) {
			unsigned hval = m_HashComp(k);
			nodeptr * ppNode;
			for ( ppNode = m_Table + (hval % m_nTableSize); *ppNode; ppNode = &(*ppNode)->m_pNext ) {
				const typename TRAITS::key_type & key = TRAITS::cast((*ppNode)->m_Value);
				if ( m_HashComp.match((*ppNode)->get_hval(key, m_HashComp), hval, key, k, m_HashComp) ) {
					erase((void **)ppNode);
					return 1;
				}
			}
		}
		return 0;
	}

	iterator find(const typename TRAITS::key_type & k)
	{
		nodeptr pNode = (nodeptr)lookup_node(k);
		return (iterator &)pNode;
	}

	const_iterator find(const typename TRAITS::key_type & k) const
	{
		nodeptr pNode = (nodeptr)lookup_node(k);
		return (const_iterator &)pNode;
	}

	ibpair insert(const TKEY & k)
	{
		int nSize = m_nSize;
		nodeptr pNode = (nodeptr)insert_node(TRAITS::cast(k));
		if ( nSize != m_nSize ) {
			new (&pNode->m_Value) TKEY(k);
			Notify(EventCode::Add, pNode);
		}
		return ibpair((iterator &)pNode, nSize != m_nSize);
	}

	int max_size() const
	{
		return HashHelper::GetMaxTableSize();
	}

	void swap(hash_set & x)
	{
		SCL::swap(m_Table, x.m_Table);
		SCL::swap(m_nSize, x.m_nSize);
		SCL::swap(m_nTableSize, x.m_nTableSize);
		SCL::swap(m_HashComp, x.m_HashComp);
		SCL::swap(m_pFirst, x.m_pFirst);
		SCL::swap(m_pLast, x.m_pLast);
	
		this->Notify(EventCode::Reload);
		x.Notify(EventCode::Reload);
	}
};

template <class TKEY, class TRAITS = _hash_comp<TKEY>, class A = alloc, class N = NullNotifier>
class hash_set2 : public hash_set<TKEY, TRAITS, A, N>
{
public:
	typedef hash_set<TKEY, TRAITS, A, N> hash_set;
	typedef typename hash_set::ibpair ibpair;

	typedef TKEY key_type;
	typedef const TKEY const_key_type;

	typedef typename A::size_type size_type;

	typedef hash_iter<key_type, TRAITS> iterator;
	typedef hash_iter<const_key_type, TRAITS> const_iterator;

protected:
	typedef hash_node<key_type, typename TRAITS::node> node;
	typedef hash_node<const_key_type, typename TRAITS::node> const_node;

	typedef node * nodeptr;

public:
	/*explicit*/ hash_set2(int nTableSize = 0)
		: hash_set(nTableSize)
	{
	}

	/*explicit*/ hash_set2(const TRAITS & comp, int nTableSize = 0)
#ifdef __hpux
		: m_HashComp(comp)
#else
        : hash_set::m_HashComp(comp)
#endif
	{
		this->init(nTableSize);
	}

	/*explicit*/ hash_set2(const TRAITS & comp, const A & al, int nTableSize = 0)
#ifdef __hpux
		: m_HashComp(comp), m_Allocator(al)
#else
		: hash_set::m_HashComp(comp), hash_set::m_Allocator(al)
#endif
	{
		this->init(nTableSize);
	}

	hash_set2(const hash_set & x)
		: hash_set(x)
	{
	}

	template<class InputIterator>
	hash_set2(InputIterator _First, InputIterator _Last)
	{
		this->init(0);
		for ( InputIterator it = _First; it != _Last; it++ ) {
			int nSize = this->m_nSize;
			nodeptr pNode = (nodeptr)insert_node(*it);
			if ( nSize != this->m_nSize ) {
				new (&pNode->m_Value) TKEY(*it);
			}
		}
	}

	template<class InputIterator>
	hash_set2(InputIterator _First, InputIterator _Last, const A & _Al)
#ifdef __hpux
		: m_Allocator(_Al)
#else
		: hash_set::m_Allocator(_Al)
#endif
	{
		hash_set2(_First, _Last);
	}

	size_type count(const TKEY & k) const
	{
	}

	A get_allocator() const
	{
		return this->m_Allocator;
	}

	iterator rbegin()
	{
		return (iterator &)this->m_pLast;
	}

	iterator rend()
	{
		return iterator();
	}

	const_iterator rbegin() const
	{
		return (const_iterator &) this->m_pLast;
	}

	const_iterator rend() const
	{
		return const_iterator();
	}

	int erase(const typename TRAITS::key_type & k)
	{
		return this->erase(k);
	}

	iterator erase(iterator pos)
	{
		return this->erase(pos);
	}

	iterator erase(iterator first, iterator last)
	{
		if ( first != this->begin() && last != this->end() ) {
			while ( first != last ) {
				first = erase(first);
			}
			return first;
		} else {
			this->clear();
			return iterator();
		}
	}

	ibpair insert(const TKEY & k)
	{
		return this->insert(k);
	}

	template<class InputIterator>
	void insert(InputIterator _First, InputIterator _Last)
	{
		for ( InputIterator it = _First; it != _Last; it++ ) {
			int nSize = this->m_nSize;
			nodeptr pNode = (nodeptr)insert_node(*it);
			if ( nSize != this->m_nSize ) {
				new (&pNode->m_Value) TKEY(*it);
				Notify(EventCode::Add, pNode);
			}
		}
	}
};

} // namespace SCL

#endif // __SCL_HASH_SET_H__

