/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_MULTI_SET_H__
#define __SCL_MULTI_SET_H__

#include "SCL_Set.h"

namespace SCL
{

template <class TKEY, class TRAITS, class A, class N> class hash_set;

template <class TKEY, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class multiset : public set2<TKEY, PRED, A>
{
public:
	typedef set2<TKEY, PRED, A> base;

	typedef TKEY value_type;

	typedef TreeIterator<TKEY> iterator;
	typedef TreeCIterator<TKEY> const_iterator;

	typedef pair<iterator, bool> ibpair;

	typedef typename A::size_type size_type;

public:
	explicit multiset(const PRED & comp = PRED(), const A & al = A())
		: base(comp, al)
	{
		base::m_bMulti = true;
	}

	multiset(const multiset& x)
		: base(x)
	{
		base::m_bMulti = true;
	}

	template<class InputIterator>
	multiset(InputIterator first, InputIterator last, const PRED & comp = PRED(), const A & al = A())
		: base(comp, al)
	{
		base::m_bMulti = true;
		for ( InputIterator it = first; it != last; it++ ) {
			set<TKEY, PRED, A>::insert(*it);
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

	iterator erase(iterator x)
	{
		return base::erase(x);
	}

	iterator erase(iterator first, iterator last)
	{
		return base::erase(first, last);
	}

	size_type erase(const TKEY & x)
	{
		iterator it1 = find(x);
		if ( it1 == base::end() ) {
			return 0;
		}
		const_iterator it2 = base::upper_bound(x);
		int deleted;
		for ( deleted = 0; it1 != it2; deleted++ ) {
			it1 = erase(it1);
		}
		return deleted;
	}
};


} // namespace SCL

#endif // __SCL_MULTI_SET_H__

