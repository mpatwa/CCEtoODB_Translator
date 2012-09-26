/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_MAP_H__
#define __SCL_MAP_H__

#include "SCL_Set.h"

namespace SCL
{

template <class TKEY, class T, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class map : public set<pair<const TKEY, T>, pred_first<pair<const TKEY, T>, PRED>, A, N>
{
public:
	typedef T mapped_type;
	typedef pair<const TKEY, T> value_type;
	typedef pair<const TKEY, const T> const_value_type;

	typedef typename A::size_type size_type;

	typedef TreeIterator<value_type> iterator;
	typedef TreeCIterator<value_type> const_iterator;

	typedef pred_first<pair<const TKEY, T>, PRED> pred_type;
	typedef set<pair<const TKEY, T>, pred_type, A, N> base;

public:
	map(const PRED & comp = PRED(),  const A & al = A())
		: base(pred_type(comp), al)
	{
	}

	/*explicit*/ map(const map & x)
		: base(x)
	{
	}	

	T & operator [] (const TKEY & k)
	{
		iterator it = find(k);
		if ( it == base::end () ) {
			it = insert(value_type(k, T())).first;
		}
		return ((*it).second);
	}

	size_type erase(const TKEY & x)
	{
		iterator it = find(x);
		base::erase(it);
		return 1;
	}

	iterator erase(iterator x)
	{
		return base::erase(x);
	}

	iterator erase(iterator first, iterator last)
	{
		return base::erase(first, last);
	}
};

template <class TKEY, class T, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class map2 : public set2<pair<const TKEY, T>, pred_first<pair<const TKEY, T>, PRED>, A, N>
{
public:
	typedef T mapped_type;
	typedef pair<TKEY, T> value_type;
	typedef pair<TKEY, const T> const_value_type;

	typedef TreeIterator<value_type> iterator;
	typedef TreeCIterator<value_type> const_iterator;

	typedef pair<iterator, bool> ibpair;

	typedef pred_first<pair<const TKEY, T>, PRED> pred_type;
	typedef set2<pair<const TKEY, T>, pred_type, A, N> base;

protected:

public:
	map2(const PRED & comp = PRED(),  const A & al = A())
		: base(comp, al)
	{
	}

	/*explicit*/ map2(const map2 & x)
		: base(x)
	{
	}

	template<class InputIterator>
	map2(InputIterator first, InputIterator last, const PRED & comp = PRED(), const A & al = A())
		: base(comp, al)
	{
		for ( InputIterator it = first; it != last; it++ ) {
			insert(*it);
		}
	}

	T & operator [] (const TKEY & x)
	{
		typename base::ibpair ibp = _insert(x);
		return ibp.first->second;
	}

	typename base::size_type erase(const TKEY & x)
	{
		typename base::iterator it = find(x);
		if ( it != base::end() ) {
			base::erase(it);
			return 1;
		}
		return 0;
	}

	typename base::iterator erase(typename base::iterator x)
	{
		return base::erase(x);
	}

	typename base::iterator erase(typename base::iterator first, typename base::iterator last)
	{
		return base::erase(first, last);
	}
};

} // namespace SCL

#endif // __SCL_MAP_H__

