/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_HASH_MULTI_MAP_H__
#define __SCL_HASH_MULTI_MAP_H__

#include "SCL_HashSet.h"

namespace SCL
{
template <class TKEY, class TRAITS, class A, class N> class hash_set;

template <class TKEY, class T, class TRAITS = _hash_comp<TKEY>, class A = alloc, class N = NullNotifier>
class hash_multimap : public hash_set<pair<const TKEY, T>, pair_hash_comp<TKEY, T, TRAITS>, A>
{
public:
	typedef T mapped_type;
	typedef pair<const TKEY, T> value_type;
	typedef pair<const TKEY, const T> const_value_type;

	typedef pair_hash_comp<TKEY, T, TRAITS> traits_type;
	typedef hash_set<pair<const TKEY, T>, traits_type, A, N> base;

	typedef hash_iter<value_type, traits_type> iterator;
	typedef hash_iter<const_value_type, traits_type> const_iterator;

protected:

	typedef hash_node<value_type, typename TRAITS::node> node;
	typedef hash_node<value_type, typename TRAITS::node> const_node;

	typedef node * nodeptr;

	void * insert_node(const typename TRAITS::key_type & k)
	{
		if ( base::m_nSize >= base::m_nTableSize ) {
			resize(base::m_nSize + 1);
		}

		unsigned hval = m_HashComp(k);
		nodeptr *list = base::m_Table + (hval % base::m_nTableSize);
		nodeptr  pLast = NULL;
		bool     look_for_end = true;
		for (; *list; list = &((*list)->m_pNextHash) ) {
			if ( look_for_end ) {
				if ( base::m_HashComp.equal(traits_type::cast((*list)->m_Value), k) ) {
					pLast = *list;
				} else {
					if ( pLast ) {
						break;
					}
				}
			}
			if ( (*list)->m_pNextHash == NULL ) {
				pLast = *list;
				break;
			}
		}

		nodeptr pNode = (nodeptr)base::m_Allocator._Charalloc(sizeof(node));
		pNode->set_hval(hval);		
		if ( pLast ) {
			pNode->m_pNextHash = pLast->m_pNextHash;
			pLast->m_pNextHash = pNode;
			nodeptr nextNode = pLast->m_pNext;
			pLast->m_pNext = pNode;
			pNode->m_pNext = nextNode;
			pNode->m_pPrev = pLast;
			if ( nextNode ) {
				nextNode->m_pPrev = pNode;
			}
			if ( base::m_pLast == pLast ) {
				base::m_pLast = pNode;
			}
		}
		else { // The first initialization of m_Table + (hval % m_nTableSize);
			pNode->m_pNextHash = NULL;
			*list = pNode;		

			if ( base::m_pLast ) {
				base::m_pLast->m_pNext = pNode;
			}
			pNode->m_pNext = NULL;
			pNode->m_pPrev = base::m_pLast;
			base::m_pLast = pNode;
		}
		if ( !base::m_pFirst ) {
			base::m_pFirst = pNode;
		}

		++base::m_nSize;

		return pNode;
	}

public:
	hash_multimap(int nTableSize = 0)
		: base(nTableSize)
	{
	}

	explicit hash_multimap(const A & al, int nTableSize = 0)
		: base(al, nTableSize)
	{
	}

	explicit hash_multimap(const TRAITS & comp, int nTableSize = 0)
		: base::m_HashComp(comp)
	{
		base::init(nTableSize);
	}

	/*explicit*/ hash_multimap(const TRAITS & comp, const A & al, int nTableSize = 0)
		: base::m_HashComp(comp), base::m_Allocator(al)
	{
		base::init(nTableSize);
	}

	hash_multimap(const hash_multimap &x)
		: base(x)
	{ 
	}

	void resize(int nSize)
	{
		int new_size = HashHelper::GetNewTableSize(nSize - 1);
		if ( new_size == base::m_nTableSize ) {
			return;
		}

		if ( base::m_Table ) {
			base::m_Allocator._Chardealloc(base::m_Table, sizeof(nodeptr) * base::m_nTableSize);
		}

		base::m_nTableSize = new_size;
		SCL_ASSERT(base::m_nTableSize);

		base::m_Table = (nodeptr *)base::m_Allocator._Charalloc(base::m_nTableSize * sizeof(nodeptr));
		memset(base::m_Table, 0, base::m_nTableSize * sizeof(nodeptr));

		for ( nodeptr pNode = base::m_pFirst; pNode; pNode = pNode->m_pNext ) {
			const typename TRAITS::key_type & key = traits_type::cast(pNode->m_Value);
			nodeptr & p = base::m_Table[pNode->get_hval(key, base::m_HashComp) % base::m_nTableSize];
			if ( p == NULL ) {
				p = pNode;
			}
		}
	}

	iterator insert(const value_type & k)
	{
		nodeptr pNode = (nodeptr)insert_node(traits_type::cast(k));
		new ((void *)&pNode->m_Value.first) TKEY(k.first);
		new ((void *)&pNode->m_Value.second) T(k.second);
		Notify(EventCode::Add, pNode);
		return (iterator &)pNode;
	}

	template<class InputIterator>
	void insert(InputIterator _First, InputIterator _Last)
	{
		base::init(0);
		for ( InputIterator it = _First; it != _Last; it++ ) {
			int nSize = base::m_nSize;
			const typename TRAITS::key_type & key = traits_type::cast(*it);
			nodeptr pNode = (nodeptr)insert_node(key);
			if ( nSize != base::m_nSize ) {
				new (&pNode->m_Value) TKEY(key);
				Notify(EventCode::Add, pNode);
			}
		}
	}

	iterator erase(iterator pos)
	{
		return base::erase(pos);
	}

	int erase(const TKEY & k)
	{
		if ( !base::empty() ) {
			unsigned hval = base::m_HashComp(k);
			int deleted = 0;
			for ( nodeptr pNode = *(base::m_Table + (hval % base::m_nTableSize)), pNextHash = NULL ; pNode; pNode = pNextHash ) {
				pNextHash = pNode->m_pNextHash;
				base::erase((void **) &pNode);
				deleted++;
			}
			*(base::m_Table + (hval % base::m_nTableSize)) = NULL;
			return deleted;
		}
		return 0;
	}

	iterator erase(iterator first, iterator last)
	{
		if ( first == base::begin() && last == base::end() ) {
			base::clear();
			return iterator();
		}
		else {
			while ( first != last ) {
				first = base::erase(first);
			}
			return first;
		}
	}
};


} // namespace SCL

#endif // __SCL_HASH_MULTI_MAP_H__

