/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_HASH_MAP_H__
#define __SCL_HASH_MAP_H__

#include "SCL_HashSet.h"

namespace SCL
{

template <class TKEY, class T, class TRAITS>
struct pair_hash_comp : public TRAITS
{
	static const TKEY & cast(const pair<const TKEY, T> & x)
	{
		return x.first;
	}
};

template <class TKEY, class T, class TRAITS = _hash_comp<TKEY>, class A = alloc>
class hash_map : public hash_set<pair<const TKEY, T>, pair_hash_comp<TKEY, T, TRAITS>, A>
{
public:
	typedef T mapped_type;
	typedef pair<const TKEY, T> value_type;
	typedef const pair<const TKEY, T> const_value_type;

	typedef pair_hash_comp<TKEY, T, TRAITS> traits_type;
	typedef hash_set<pair<const TKEY, T>, traits_type, A> base;

	typedef hash_iter<value_type, traits_type> iterator;
	typedef hash_iter<const_value_type, traits_type> const_iterator;

protected:
	typedef hash_node<value_type, typename TRAITS::node> node;
	typedef hash_node<const_value_type, typename TRAITS::node> const_node;

	typedef node * nodeptr;

public:
	hash_map(int nTableSize = 0)
		: base(nTableSize)
	{
	}

	/*explicit*/ hash_map(const A & al, int nTableSize = 0)
		: base(al, nTableSize)
	{
	}

	hash_map(const hash_map &x)
		: base(x)
	{
	}	

	T & operator [] (const TKEY & k)
	{
		int nSize = base::m_nSize;
		nodeptr pNode = (nodeptr)insert_node(k);
		if ( nSize < base::m_nSize ) {
			new ((void *)&pNode->m_Value.first) TKEY(k);
			new ((void *)&pNode->m_Value.second) T;
		}
		return pNode->m_Value.second;
	}
};

template <class TKEY, class T, class TRAITS = _hash_comp<TKEY>, class A = alloc>
class hash_map2 : public hash_set2<pair<TKEY, T>, TRAITS, A>
{
public:
	typedef T mapped_type;
	typedef pair<TKEY, T> value_type;
	typedef pair<TKEY, const T> const_value_type;

	typedef hash_iter<value_type, TRAITS> iterator;
	typedef hash_iter<const_value_type, TRAITS> const_iterator;

protected:
	typedef hash_set2<pair<const TKEY, T>, TRAITS, A> base;

	typedef hash_node<value_type, typename TRAITS::node> node;
	typedef hash_node<const_value_type, typename TRAITS::node> const_node;

	typedef node * nodeptr;

public:
	hash_map2(int nTableSize = 0)
		: hash_set2<value_type, TRAITS>(nTableSize)
	{
	}

	explicit hash_map2(const A & al, int nTableSize = 0)
		: hash_set2<value_type, TRAITS>(al, nTableSize)
	{
	}

	explicit hash_map2(const TRAITS & comp, int nTableSize = 0)
		: hash_set2<value_type, TRAITS>(comp, nTableSize)
	{
	}

	/*explicit*/ hash_map2(const TRAITS & comp, const A & al, int nTableSize = 0)
		: hash_set2<value_type, TRAITS>(comp, al, nTableSize)
	{
	}

	hash_map2(hash_map2<value_type, TRAITS> &x)
		: hash_set2<value_type, TRAITS>(x)
	{
	}	

	T & operator [] (const TKEY & k)
	{
		int nSize = base::m_nSize;
		nodeptr pNode = (nodeptr)insert_node(k);
		if ( nSize < base::m_nSize ) {
			new (&pNode->m_Value) value_type(k, T());
		}
		return pNode->m_Value.second;
	}
};

} // namespace SCL

#endif // __SCL_HASH_MAP_H__

