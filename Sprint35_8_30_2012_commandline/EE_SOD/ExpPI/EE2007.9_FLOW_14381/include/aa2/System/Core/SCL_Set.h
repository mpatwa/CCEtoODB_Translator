/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_SET_H__
#define __SCL_SET_H__

#include "SCL_Tree.h"

namespace SCL
{

template <class TKEY, class PRED, class A, class N> class set2;

/*!
The template class describes an object that controls a varying-length sequence of elements
of type const <tt><b>TKEY</b></tt>. Each element serves as both a sort key and a value.
The sequence is represented in a way that permits lookup, insertion, and removal of an arbitrary
element with a number of operations proportional to the logarithm of the number of elements
in the sequence (logarithmic time). Moreover, inserting an element invalidates no iterators,
and removing an element invalidates only those iterators that point at the removed element.

The class set is a child of SCL::Tree and shares all its public functions and properties.
\see SCL::Tree documentation.

\note To encrease performance of the class Tree there are some limitations on 
end's iterators: it's impossible to call <tt>operator -- ()</tt> for iterators returned
by <tt>end()</tt> and <tt>rend()</tt>.


There are two classes:
<ol>
	<li>SCL::set
	<li>SCL::set2
</ol>
The second one has more functions but not so fast as the first one.
\see class SCL::hash_set which is more faster than SCL::set but has a number of limitations on
<tt>hashing</tt>.

The object orders the sequence it controls by calling a stored function object of type
<tt><b>PRED</b></tt>. You access this stored object by calling the member function <tt>m_Comp()</tt>.
Such a function object must impose a total order on sort keys.
For any element <tt>x</tt> that precedes <tt>y</tt> in the sequence, <tt>m_Comp()(y, x) is false</tt>.
Unlike template class <tt>multiset</tt>, an object of template class set ensures that
<tt>m_Comp()(x, y) is true</tt>. (Each key is unique.)

The object allocates and frees storage for the sequence it controls through a protected
object named <tt>m_Allocator</tt>, of <tt><b>class A</b></tt>. Such a <tt>m_Allocator</tt>
object must have the same external interface as an object of template class <tt>SCL::alloc</tt>.

\note that <tt>m_Allocator</tt> is not copied when the object is assigned.
*/
template <class TKEY, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class set : public Tree<TKEY, PRED, A, N>
{
	friend class set2<TKEY, PRED, A, N>;

public:
	/// The type describes the sort key object that constitutes each element of the controlled sequence.
	typedef TKEY key_type;
	/// The type describes the constant sort key object that constitutes each element of the controlled sequence.
	typedef const TKEY const_key_type;

	/// The type describes an element of the controlled sequence.
	typedef TKEY value_type;
	/// The type describes a constant element of the controlled sequence.
	typedef const TKEY const_value_type;

	/// The type describes a function object that can compare two sort keys to determine the relative order of any two elements in the controlled sequence.
	typedef PRED key_compare;

	///  The unsigned integer type describes an object that can represent the length of any controlled sequence.
	typedef typename A::size_type size_type;
	/// The signed integer type describes an object that can represent the difference between the addresses of any two elements in the controlled sequence.
	typedef typename A::difference_type difference_type;

	/// The type is a synonym for the template parameter <tt>A</tt>.
	typedef A allocator_type;

protected:

public:
	/*!
	The constructor stores argument named <tt>comp</tt> to the function to compare
	keys (<tt>m_Comp()</tt>) and allocator object <tt>al</tt> and specifies an empty
	initial controlled sequence.
	*/
	explicit set(const PRED & comp = PRED(),  const A & al = A())
		: Tree<TKEY, PRED, A, N>(comp, false, al)
	{
	}

	/*!
	The constructor specifies a copy of the sequence controlled by <tt>x</tt>.
	It uses <tt>x.m_Allocator</tt> to init <tt>m_Allocator</tt>.
	*/
	set(const set& x)
		: Tree<TKEY, PRED, A, N>(x)
	{
	}

};

/*!
The template class describes an object that controls a varying-length sequence of elements
of type const <tt><b>TKEY</b></tt>. Each element serves as both a sort key and a value.
The sequence is represented in a way that permits lookup, insertion, and removal of an arbitrary
element with a number of operations proportional to the logarithm of the number of elements
in the sequence (logarithmic time). Moreover, inserting an element invalidates no iterators,
and removing an element invalidates only those iterators that point at the removed element.

The class set is a child of SCL::set and shares all its public functions and properties.
\see SCL::set documentation.
*/
template <class TKEY, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class set2 : public set<TKEY, PRED, A, N>
{
public:

	typedef TreeNode<TKEY> node;

	typedef typename A::size_type size_type;

	typedef TreeIterator<TKEY> iterator;
	typedef TreeCIterator<TKEY> const_iterator;

	typedef pair<iterator, bool> ibpair;

protected:
	typedef set<TKEY, PRED, A, N> basic_set;

public:
	/*!
	The constructor stores argument named <tt>comp</tt> to the function to compare
	keys (<tt>m_Comp()</tt>) and allocator object <tt>al</tt> and specifies an empty
	initial controlled sequence.
	*/
	explicit set2(const PRED & comp = PRED(), const A & al = A())
		: basic_set(comp, al)
	{
	}

	/*!
	The constructor specifies a copy of the sequence controlled by <tt>x</tt>.
	It uses <tt>x.m_Allocator</tt> to init <tt>m_Allocator</tt>.
	*/
	set2(const basic_set & x)
		: basic_set(x)
	{
	}

	/*!
	The constructor stores argument named <tt>comp</tt> to the function to compare
	keys (<tt>m_Comp()</tt>) and allocator object <tt>al</tt> and inits the controlled
	sequence with values from range <tt>[first, last)</tt>.
	*/
	template<class InputIterator>
	set2(InputIterator first, InputIterator last, const PRED & comp = PRED(), const A & al = A())
		: basic_set(comp, al)
	{
		for ( InputIterator it = first; it != last; it++ ) {
			basic_set::insert(*it);
		}
	}

	/*!
	The member function determines whether an element <tt>y</tt> exists in the sequence
	whose key matches that of <tt>x</tt>. (The keys match if <tt>!m_Comp(x. first, y.first)
	&& !m_Comp(y.first, x.first)</tt>.) If not, it creates such an element <tt>y</tt>
	and initializes it with <tt>x</tt>. The function then determines the iterator <tt>it</tt>
	that designates <tt>y</tt>. The function returns <tt>ibpair</tt> in which iterator
	is <tt>it</tt> and <tt>bool</tt> is <tt>true</tt> if an insertion occurred or
	<tt>false</tt> otherwise. 
	*/
	ibpair insert(const TKEY & x)
	{
		return basic_set::insert(x);
	}

	/*!
	The member function inserts elements from range <tt>[first, last)</tt> into
	the controlled sequence.
	*/
	template<class InputIterator> void insert(InputIterator first, InputIterator last)
	{
		for ( InputIterator it = first; it != last; it++ ) {
			basic_set::insert(*it);
		}
	}

	/*!
	The member function removes the element of the controlled sequence pointed to by
	<tt>x</tt> and returns an iterator that designates the first element remaining
	beyond any elements removed, or <tt>end()</tt> if no such element exists. 
	*/
	iterator erase(iterator x)
	{
		return basic_set::erase(x);
	}

	/*!
	The member function erases element from the controlled sequence in the range
	<tt>[first, last)</tt>.
	*/
	iterator erase(iterator first, iterator last)
	{
		return basic_set::erase(first, last);
	}

	/*!
	The member function removes the elements with sort keys in the range 
	<tt>[lower_bound(x), upper_bound(x))</tt>. It returns the number of elements it removes.
	*/
	size_type erase(const TKEY & x)
	{
		iterator it = find(x);
		if ( it == basic_set::end() ) {
			return 0;
		}
		set<TKEY, PRED, A, N>::erase(it);
		return 1;
	}

	/*!
	The member function returns an iterator that designates the earliest element <tt>el</tt>
	in the controlled sequence for which <tt>m_Comp(el, x)</tt> is <tt>false</tt>.

	If no such element exists, the function returns <tt>end()</tt>.
	*/
	const_iterator lower_bound(const TKEY & x) const
	{
		return find(x);
	}

	/*!
	The member function returns an iterator that designates the earliest element <tt>el</tt>
	in the controlled sequence for which <tt>m_Comp(x, el)</tt> is <tt>true</tt>.

	If no such element exists, the function returns <tt>end()</tt>.
	*/
	const_iterator upper_bound(const TKEY & x) const
	{
		if ( !basic_set::m_Root ) {
			return basic_set::m_Root;
		}
		node * q = NULL;
		for ( node * p = basic_set::m_Root; p; ) {
			if ( !m_Comp(x, p->m_Value) ) {
				p = p->m_pRight;
			} else {
				q = p;
				p = p->m_pLeft;
			}
		}
		if ( !q ) {
			return basic_set::end();
		}
		return q;
	}

	/*!
	The member function returns a pair of iterators <tt>p</tt> such that
	<tt>p.first == lower_bound(x) and p.second == upper_bound(x)</tt>.
	*/
	pair<const_iterator, const_iterator> equal_range(const TKEY & x) const
	{
		pair<const_iterator, const_iterator> p(lower_bound(x), upper_bound(x));
		return p;
	}
};

} // namespace SCL

#endif // __SCL_SET_H__

