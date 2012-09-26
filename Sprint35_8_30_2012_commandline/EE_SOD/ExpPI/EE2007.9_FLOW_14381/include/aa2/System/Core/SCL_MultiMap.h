/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_MULTI_MAP_H__
#define __SCL_MULTI_MAP_H__

#include "SCL_Tree.h"
#include "SCL_Set.h"

namespace SCL
{

template <class TKEY, class TRAITS, class A, class N> class set;
template <class TKEY, class TRAITS, class A, class N> class set2;

template <class TKEY, class T, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class multimap : public set2<pair<const TKEY, T>, pred_first<pair<const TKEY, T>, PRED>, A, N>
{
public:
	typedef T mapped_type;
	typedef pair<const TKEY, T> value_type;
	typedef pair<const TKEY, const T> const_value_type;

	typedef TreeIterator<value_type> iterator;
	typedef TreeCIterator<value_type> const_iterator;

	typedef pair<iterator, bool> ibpair;

	typedef typename A::size_type size_type;

	typedef pred_first<pair<const TKEY, T>, PRED> predicate_type;
	typedef set2<pair<const TKEY, T>, predicate_type, A, N> base;

public:
	explicit multimap(const PRED & comp = PRED(), const A & al = A())
		: base(predicate_type(comp), al)
	{
		base::m_bMulti = true;
	}

	multimap(const multimap& x)
		: base(x)
	{
		base::m_bMulti = true;
	}

	template<class InputIterator>
	multimap(InputIterator first, InputIterator last, const PRED & comp = PRED(), const A & al = A())
		: base(comp, al)
	{
		base::m_bMulti = true;
		for ( InputIterator it = first; it != last; it++ ) {
			insert(*it);
		}
	}

	iterator insert(const value_type & x)
	{
		ibpair p = base::insert(x);
		return p.first;
	}

	template<class InputIterator> void insert(InputIterator first, InputIterator last)
	{
		for ( InputIterator it = first; it != last; it++ ) {
			base::insert(*it);
		}
	}

	iterator erase(typename base::iterator x)
	{
		return base::erase(x);
	}

	iterator erase(typename base::iterator first, typename base::iterator last)
	{
		return base::erase(first, last);
	}

	size_type erase(const TKEY & x)
	{
		iterator it1 = find(x);
		if ( it1 == base::end() ) {
			return 0;
		}
		const_iterator it2 = upper_bound(x);
		int deleted;
		for ( deleted = 0; it1 != it2; deleted++ ) {
			it1 = base::erase(it1);
		}
		return deleted;
	}
};


} // namespace SCL

#endif // __SCL_MULTI_MAP_H__

