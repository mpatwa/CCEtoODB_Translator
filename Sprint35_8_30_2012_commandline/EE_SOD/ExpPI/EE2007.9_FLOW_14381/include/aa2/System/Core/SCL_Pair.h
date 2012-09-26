/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_PAIR_H__
#define __SCL_PAIR_H__

namespace SCL
{

template <class T1, class T2>
struct pair
{
	T1 first;
	T2 second;

    typedef T1 first_type;
    typedef T2 second_type;

	pair(const T1 & x)
		: first(x)
	{
	}

	pair(const T1 & x, const T2 & y)
		: first(x), second(y)
	{
	}

	template<class U1, class U2>
	pair(const pair<U1, U2> & pr)
		: first(pr.first), second(pr.second)
	{
	}

	bool operator == (const pair & x) const
	{
		return first == x.first && second == x.second;
	}

	bool operator != (const pair & x) const
	{
		return !(*this == x);
	}
};

template <class T1, class T2>
pair<T1, T2> make_pair(const T1 & t1, const T2 & t2)
{
	return pair<T1, T2> (t1, t2);
}

template <class T, class PRED>
struct pred_first
{
	PRED m_pred;

	pred_first()
	{
	}
	pred_first(const PRED & x)
		: m_pred(x)
	{
	}
	pred_first(const pred_first & x)
		: m_pred(x.m_pred)
	{
	}
	bool operator () (const T & x1, const T & x2) const
	{
		return m_pred(x1.first, x2.first);
	}
};

} // namespace SCL

#endif // __SCL_PAIR_H__

