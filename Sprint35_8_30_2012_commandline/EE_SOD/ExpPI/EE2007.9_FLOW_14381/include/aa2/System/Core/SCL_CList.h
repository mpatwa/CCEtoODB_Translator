/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_CLIST_H__
#define __SCL_CLIST_H__

# include <new>

#include "SCL_Swapper.h"

namespace SCL
{

template <class T> struct list_node;

template <class T> struct list_node;
template <class T, class A, class N> class list2;

template <class T>
struct list_node_base
{
	/// The type describes a <tt>node</tt>.	
	typedef list_node<T> node;

	/// Pointers to <tt>next</tt> and <tt>previous</tt> <tt>nodes</tt> in SCL::list.
	node * m_pNext, * m_pPrev;
};

/*!
The template class list_node is an internal class for the SCL::list template collection. 
list_node is a double linked list node; it has contains an data object of
type <tt>T</tt> and two pointers: 
<ol>
	<li>Pointer to the previous node. 
	<li>Pointer to the next node. 
</ol>
Sometimes it might be practical to have direct access to the list nodes,
but it is seldom required.

\warning 
Be very careful if you want to access the list nodes.
The heap can easily get corrupted if you make a mistake.
*/
template <class T>
struct list_node : public list_node_base<T>
{
	/// Data object of type <tt>T</tt>
	T m_Value;

	~list_node()
	{
		this->m_pPrev->m_pNext = this->m_pNext;
		this->m_pNext->m_pPrev = this->m_pPrev;
	}

	/*!
	Removes a <tt>node</tt> from list.
	\note The data object m_Value is not destroyed!
	*/
	void remove()
	{
		this->m_pPrev->m_pNext = this->m_pNext;
		this->m_pNext->m_pPrev = this->m_pPrev;
		this->m_pNext = this->m_pPrev = (list_node<T> *)this;
	}
};

#ifdef _MSC_VER
#pragma warning(disable: 4284) // iterator::operator -> () returns pointer to a simple type
#endif

template <class T> struct list_const_iter;

/*!
The template class list_iter represents a bidirectional iterator 
for SCL::list template class of type <tt>T</tt>.
<p>list_iter contains a pointer to list_node which represents a data
and provides a set of basic operators to get access to data and
previous and next list's iterators.<br>
Also it is possible to compare two iterators.
*/
template <class T>
struct list_iter
{
	/// The type describes an object that represents a <tt>node</tt> of type <tt>T</tt>.	
	typedef list_node<T> node;
	/// The type represents <tt>iterator</tt>.
	typedef list_iter<T> iterator;
	/// The type represents <tt>constant iterator - const_iterator</tt>.
	typedef list_const_iter<T> const_iterator;

	/// Pointer to data node.
	node * m_Focus;

public:
	/// Default constructor.
	list_iter()
	{
	}

	/// Constructor from <tt>node</tt>. Inits <tt>m_Focus</tt>.
	list_iter(node * x)
		: m_Focus(x)
	{
	}

	/// Comparison operator. Compares <tt>m_Focus</tt>.
	bool operator == (const const_iterator & x) const
	{
		return m_Focus == x.m_Focus;
	}

	/// Not equal to operator.
	bool operator != (const const_iterator & x) const
	{
		return m_Focus != x.m_Focus;
	}

	T & get_value() const
	{
		return m_Focus->m_Value;
	}

	void set_value(const T & x)
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

	/// Asterix operator. Returns a value of <tt>node</tt>.
	T & operator * () const
	{
		return m_Focus->m_Value;
	}
	/// This operator returns a pointer to a value of <tt>node</tt>.
	T * operator -> () const
	{
		return &m_Focus->m_Value;
	}

	/// Prefix ++ returns a next <tt>node</tt>.
	iterator & operator ++ ()
	{
		m_Focus = m_Focus->m_pNext;
		return *this;
	}
	/// Prefix -- returns a previous <tt>node</tt>.
	iterator & operator -- ()
	{
		m_Focus = m_Focus->m_pPrev;
		return *this;
	}

	/// Postfix ++ returns a next <tt>node</tt>.
	iterator operator ++ (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->m_pNext;
		return retval;
	}
	/// Postfix -- returns a previous <tt>node</tt>.
	iterator operator -- (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->m_pPrev;
		return retval;
	}
};

/*!
The template class list_iter represents a bidirectional const iterator 
for SCL::list template class of type <tt>T</tt>.
<p>list_const_iter contains a pointer to list_node which represents a data
and provides a set of basic operators to get access to data and
previous and next list's iterators.<br>
Also it is possible to compare two iterators.
*/
template <class T>
struct list_const_iter
{
	/// The type describes an object that represents a <tt>node</tt> of type <tt>T</tt>.
	typedef typename list_node<T>::node node;
	/// The type represents <tt>iterator</tt>.
	typedef list_iter<T> iterator;
	/// The type represents <tt>constant iterator - const_iterator</tt>.
	typedef list_const_iter<T> const_iterator;

	/// Pointer to data node.
	const node * m_Focus;

public:
	/// Default constructor.
	list_const_iter()
	{
	}

	/// Constructor from <tt>node</tt>. Inits <tt>m_Focus</tt>.
	list_const_iter(const node * x)
		: m_Focus(x)
	{
	}

	/// Constructor from <tt>iterator</tt>. Inits <tt>m_Focus</tt>.
	list_const_iter(const iterator & x)
		: m_Focus(x.m_Focus)
	{
	}

	/// Comparison operator. Compares <tt>m_Focus</tt>.
	bool operator == (const const_iterator & x) const
	{
		return m_Focus == x.m_Focus;
	}

	/// Not equal to operator.
	bool operator != (const const_iterator & x) const
	{
		return m_Focus != x.m_Focus;
	}

	T & get_value() const
	{
		return m_Focus->m_Value;
	}

	handle get_handle() const
	{
		return (handle)m_Focus;
	}

	void set_handle(handle a_handle)
	{
		m_Focus = (node *)a_handle;
	}

	/// Asterix operator. Returns a value of <tt>node</tt>.
	const T & operator * () const
	{
		return m_Focus->m_Value;
	}
	/// This operator returns a pointer to a value of <tt>node</tt>.
	const T * operator -> () const
	{
		return &m_Focus->m_Value;
	}

	/// Prefix ++ returns a next <tt>node</tt>.
	const_iterator & operator ++ ()
	{
		m_Focus = m_Focus->m_pNext;
		return *this;
	}
	/// Prefix -- returns a previous <tt>node</tt>.
	const_iterator & operator -- ()
	{
		m_Focus = m_Focus->m_pPrev;
		return *this;
	}

	/// Postfix ++ returns a next <tt>node</tt>.
	const_iterator operator ++ (int)
	{
		const node * retval = m_Focus;
		m_Focus = m_Focus->m_pNext;
		return retval;
	}
	/// Postfix -- returns a previous <tt>node</tt>.
	const_iterator operator -- (int)
	{
		const node * retval = m_Focus;
		m_Focus = m_Focus->m_pPrev;
		return retval;
	}
};

/*!
The template class describes an object that controls a varying-length sequence of elements
of type <b>T</b>. The sequence is stored as a bidirectional linked list of elements,
each containing a member of type <tt>T</tt>.

The object allocates and frees storage for the sequence it controls through
a protected object named <b>allocator</b>, of <b>class A</b>. Such an allocator object
must have the same external interface as an object of template class  SCL::alloc. 
Note that <tt>allocator</tt> is not copied when the object is assigned.

List reallocation occurs when a member function must insert or erase elements of
the controlled sequence. In all such cases, only iterators that point 
at erased portions of the controlled sequence become invalid.

List items are stored as list_node, which also holds pointers to the next and
previous list items.

When inserting an item into a list, the item is copied. So, the item has to support
a copy constructor.

\note 
SCL::list does not support reverse_iterator and references.

\note
There are two classes of list:
<ol>
	<li>SCL::list
	<li>SCL::list2
</ol>
The first one has a minimum needed number of functions and developers shall
prefer to use it to increase an application performance. Due to this reasons
<b>class SCL::list does not support copy constructor and operator = </b>.<br>
The second one has more functionality, but it is slower than the first one, and
takes more compilation time.
*/

template <class T, class A = alloc, class N = NullNotifier>
class list : public list_node_base<T>, protected N
{
	friend class list2<T, A, N>;

	friend struct list_iter<T>;
	friend struct list_iter<const T>;

protected:
	typedef typename list_node<T>::node node;

public:
	/*!
	The type describes an object that can serve as a bidirectional iterator
	for the controlled sequence. It is described here as a synonym for the 
	type list_iter.
	*/
	typedef list_iter<T> iterator;
	/*!
	The type describes an object that can serve as a constant bidirectional
	iterator for the controlled sequence. It is described here as a synonym
	for the type list_const_iter.
	*/
	typedef list_const_iter<T> const_iterator;

private:
	void operator = (const list & x)
	{
	}

	list(const list & x)
	{
	}

protected:
	int m_nSize;

	A m_Allocator;

	void erase(node * p)
	{
		Notify(EventCode::Delete, p);
		SCL_ASSERT(p != cast());
		--m_nSize;
		p->~node();
		m_Allocator.deallocate(p, sizeof(node));
	}

	node * cast() const
	{
		return (node *)(list_node_base<T> *)this;
	}

public:
	/*!
	The constructor stores the allocator object <tt>a</tt>
	and initialize an empty initial controlled sequence.
	*/
	explicit list(const A & a = A())
		: m_Allocator(a), m_nSize(0)
	{
		this->m_pNext = this->m_pPrev = cast();
	}

	/*!
	Destructor. Deletes all elements and frees allocated memory.
	*/
	~list()
	{
		clear();
	}

	/*!
	The member function returns the length of the controlled sequence.
	*/
	int size() const
	{
		return m_nSize;
	}

	/*!
	The member function returns true for an empty controlled sequence.
	*/
	bool empty() const
	{
		return this->m_pNext == cast();
	}

	/*!
	The member function returns a reference to the first element of the controlled sequence,
	which must be non-empty.
	*/
	T & front()
	{
		SCL_ASSERT(this->m_pNext != cast() && this->m_pPrev != cast());
		return this->m_pNext->m_Value;
	}

	/*!
	The member function returns a reference to the last element of the controlled sequence,
	which must be non-empty.
	*/
	T & back()
	{
		SCL_ASSERT(this->m_pNext != cast() && this->m_pPrev != cast());
		return this->m_pPrev->m_Value;
	}

	/*!
	The member function returns a reference to the first element of the controlled sequence,
	which must be non-empty.
	*/
	const T & front() const
	{
		SCL_ASSERT(this->m_pNext != cast() && this->m_pPrev != cast());
		return this->m_pNext->m_Value;
	}

	/*!
	The member function returns a reference to the last element of the controlled sequence,
	which must be non-empty.
	*/
	const T & back() const
	{
		SCL_ASSERT(this->m_pNext != cast() && this->m_pPrev != cast());
		return this->m_pPrev->m_Value;
	}

	/*!
	The member function returns a bidirectional iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	iterator begin()
	{
		return this->m_pNext;
	}

	/*!
	The member function returns a bidirectional iterator that
	points just beyond the end of the sequence.
	*/
	iterator end()
	{
		return cast();
	}

	/*!
	The member function returns a bidirectional iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	const_iterator begin() const
	{
		return this->m_pNext;
	}

	/*!
	The member function returns a bidirectional iterator that
	points just beyond the end of the sequence.
	*/
	const_iterator end() const
	{
		return cast();
	}

	/*!
	The member function returns a bidirectional iterator that points at
	the end element of the controlled sequence. Hence, it designates the beginning of
	the reverse sequence.
	*/
	iterator rbegin()
	{
		return this->m_pPrev;
	}

	/*!
	The member function returns a bidirectional iterator that points at the
	element before the first element of the sequence (or just beyond the end of an 
	empty sequence).
	Hence, it designates the end of the reverse sequence.
	*/
	iterator rend()
	{
		return cast();
	}

	/*!
	The member function returns a bidirectional const_iterator that points at
	the end element of the controlled sequence. Hence, it designates the beginning of
	the reverse sequence.
	*/
	const_iterator rbegin() const
	{
		return this->m_pPrev;
	}

	/*!
	The member function returns a bidirectional const_iterator that points at the
	element before the first element of the sequence (or just beyond the end of an 
	empty sequence).
	Hence, it designates the end of the reverse sequence.
	*/
	const_iterator rend() const
	{
		return cast();
	}

	/*!
	The member functions inserts a single element with value <tt>x</tt> before the
	element pointed to by iterator <tt>pos</tt> in the controlled sequence.
	The function returns an iterator that points to the newly inserted element. 
	*/
	iterator insert(const iterator & pos, const T & x)
	{
		node * before_node = pos.m_Focus;
		SCL_ASSERT(before_node);

		node * new_node = (node *)m_Allocator.allocate(sizeof(node), NULL);
		new (&new_node->m_Value) T(x);

		before_node->m_pPrev->m_pNext = new_node;
		new_node->m_pPrev = before_node->m_pPrev;
		before_node->m_pPrev = new_node;
		new_node->m_pNext = before_node;

		++m_nSize;

		Notify(EventCode::Add, new_node);

		return new_node;
	}

	/*!
	The member function removes the element of the controlled sequence pointed to by
	iterator <tt>pos</tt>.
	\note
	No reallocation occurs, so iterators become invalid only for the erased elements.
	*/
	void erase(const iterator & pos)
	{
		erase(pos.m_Focus);
	}

	/*!
	The member function calls destructors of all elements stored in the controlled sequence
	and deallocates used memory for each element.
	*/
	void clear()
	{
		this->Notify(EventCode::Clear);
		while ( this->m_pNext != cast() ) {
			node * p = this->m_pNext;
			this->m_pNext = p->m_pNext;
			p->m_Value.~T();
			m_Allocator.deallocate(p, sizeof(node));
		}
		this->m_pPrev = cast();
		m_nSize = 0;
	}

	/*!
	The member function inserts an element with value
	<tt>x</tt> at the beginning of the controlled sequence.
	*/
	void push_front(const T & x)
	{
		node * new_node = (node *)m_Allocator.allocate(sizeof(node), NULL);
		new (&new_node->m_Value) T(x);

		this->m_pNext->m_pPrev = new_node;
		new_node->m_pNext = this->m_pNext;
		this->m_pNext = new_node;
		new_node->m_pPrev = cast();

		++m_nSize;

		Notify(EventCode::Add, new_node);
	}

	/*!
	The member function removes the first element of the controlled sequence,
	which must be non-empty.
	*/
	void pop_front()
	{
		erase(this->m_pNext);
	}

	/*!
	The member function inserts an element with value x at the end of
	the controlled sequence.
	*/
	void push_back(const T & x)
	{
		node * new_node = (node *)m_Allocator.allocate(sizeof(node), NULL);
		new (&new_node->m_Value) T(x);

		this->m_pPrev->m_pNext = new_node;
		new_node->m_pPrev = this->m_pPrev;
		this->m_pPrev = new_node;
		new_node->m_pNext = cast();

		++m_nSize;

		Notify(EventCode::Add, new_node);
	}

	/*!
	The member function removes the last element of the controlled sequence,
	which must be non-empty.
	*/
	void pop_back()
	{
		erase(this->m_pPrev);
	}
};

/*!
The template class SCL::list2 is a child of SCL::list and supports all parent's functions.
list2 enlarges the list possibilities but looses in performance.
\see
SCL::list documentation.
*/
template <class T, class A = alloc, class N = NullNotifier>
class list2 : public list<T, A, N>
{
public:
	typedef list<T, A, N> base_list;
	typedef list_iter<T> iterator;

protected:
	typedef list_node<T> node;

	void _splice(node * p, base_list & x, node * f, node * l)
	{
		l->m_pPrev->m_pNext = p;
		f->m_pPrev->m_pNext = l;
		p->m_pPrev->m_pNext = f;
		node * s = p->m_pPrev;
		p->m_pPrev = l->m_pPrev;
		l->m_pPrev = f->m_pPrev;
		f->m_pPrev = s;
	}

	void splice(node * p, base_list & x, node * f, node * l)
	{
		if ( f != l && &x != this ) {
			int n = 0;
			for ( node * t = f; t != l && t != (node *)&x; t = t->m_pNext ) {
				++n;
			}
			this->m_nSize += n;
			x.m_nSize -= n;
			_splice(p, x, f, l);

			x.Notify(EventCode::Reload);
			this->Notify(EventCode::Reload);
		}
	}

	void splice(node * p, base_list & x, node * f)
	{
		node * l = f->m_pNext;
		if ( p != f && p != l ) {
			_splice(p, x, f, l);
			++this->m_nSize;
			--x.m_nSize;

			x.Notify(EventCode::Reload);
			this->Notify(EventCode::Reload);
		}
	}

	void splice(node * p, base_list & x)
	{
		x.Notify(EventCode::Clear);

		this->m_nSize += x.m_nSize;
		x.m_nSize = 0;

		p->m_pPrev->m_pNext = x.m_pNext;
		x.m_pPrev->m_pNext = p;

		SCL::swap(x.m_pPrev, p->m_pPrev);

		x.m_pPrev->m_pNext = x.m_pNext;
		x.m_pNext->m_pPrev = x.m_pPrev;

		x.m_pNext = x.m_pPrev = (node *)&x;

		this->Notify(EventCode::Reload);
	}

public:
	/*!
	The constructor stores the allocator object <tt>a</tt>
	and initialize an empty initial controlled sequence.
	*/
	explicit list2(const A & a = A())
		: base_list(a)
	{
	}

	/*!
	The constructor specifies a copy of the sequence controlled by <tt>x</tt>.
	*/
	list2(const list2 & x)
	{
		node * p = x.m_pNext, * new_node = NULL;
		while ( p != x.cast() ) {
			new_node = (node *)base_list::m_Allocator.allocate(sizeof(node), NULL);
			new (&new_node->m_Value) T(p->m_Value);

			this->m_pPrev->m_pNext = new_node;
			new_node->m_pPrev = this->m_pPrev;
			this->m_pPrev = new_node;

			p = p->m_pNext;
		}
		if ( new_node ) {
			new_node->m_pNext = base_list::cast();
		}
		base_list::m_nSize = x.m_nSize;
	}

	/*!
	The member operator specifies a copy of the sequence controlled by <tt>x</tt>.
	*/
	list2 & operator = (const base_list & x)
	{
		if ( &x != this ) {
			base_list::clear();
			node * p = x.m_pNext, * new_node = NULL;
			while ( p != x.cast() ) {
				new_node = (node *)base_list::m_Allocator.allocate(sizeof(node), NULL);
				new (&new_node->m_Value) T(p->m_Value);

				this->m_pPrev->m_pNext = new_node;
				new_node->m_pPrev = this->m_pPrev;
				this->m_pPrev = new_node;

				p = p->m_pNext;
			}
			if ( new_node ) {
				new_node->m_pNext = base_list::cast();
			}
			base_list::m_nSize = x.m_nSize;

			this->Notify(EventCode::Reload);
		}
		return *this;
	}

	/*!
	The member function removes from the controlled sequence all elements,
	designated by the iterator <tt>P</tt>, for which <tt>*P == x</tt>.
	*/
	void remove(const T & x)
	{
		node * it = this->m_pNext;
		do {
			node * tmp = it->m_pNext;
			if ( it->m_Value == x ) {
				erase(it);
			}
			it = tmp;
		} while (it != base_list::cast());
	}

	/*!
	The member function removes from the controlled sequence all elements,
	designated by the iterator <tt>P</tt>, for which <tt>pr(*P)</tt> is <tt>true</tt>.
	*/
	template<class Pred>
	void remove_if(Pred pr)
	{
		node * it = this->m_pNext;
		do {
			node * tmp = it->m_pNext;
			if ( pr(it->m_Value) ) {
				erase(it);
			}
			it = tmp;
		} while (it != base_list::cast());
	}

	/*!
	The member function swaps the controlled sequences between <tt>*this</tt> and
	<tt>x</tt>.
	*/
	void swap(base_list & x)
	{
		SCL::swap(this->m_pNext, x.m_pNext);
		SCL::swap(this->m_pPrev, x.m_pPrev);
		if ( x.m_pNext == base_list::cast() ) {
			x.m_pNext = x.m_pPrev = (node *)&x;
		} else {
			x.m_pNext->m_pPrev = (node *)&x;
			x.m_pPrev->m_pNext = (node *)&x;
		}
		if ( this->m_pNext == (node *)&x ) {
			this->m_pNext = this->m_pPrev = base_list::cast();
		} else {
			this->m_pNext->m_pPrev = base_list::cast();
			this->m_pPrev->m_pNext = base_list::cast();
		}
		SCL::swap(this->m_nSize, x.m_nSize);

		this->Notify(EventCode::Reload);
		x.Notify(EventCode::Reload);
	}

	/*!
	The member function reverses the order in which elements appear in the controlled
	sequence.
	*/
	void reverse()
	{
		node * it = base_list::cast();
		do {
			SCL::swap(it->m_pNext, it->m_pPrev);
			it = it->m_pPrev; // Old next is now prev
		} while ( it != base_list::cast() );

		this->Notify(EventCode::Reorder);
	}

	/*!
	The member function removes from the controlled sequence every element that compares
	equal to its preceding element.
	*/
	void unique()
	{
		node * it = this->m_pNext;
		while ( it != base_list::cast() ) {
			node * tmp = it->m_pNext;
			while ( tmp != base_list::cast() && tmp->m_Value == it->m_Value ) {
				erase(tmp);
				tmp = it->m_pNext;
			}
			it = tmp;
		}
	}

	/*!
	For the iterators <tt>Pi</tt> and <tt>Pj</tt> designating elements at positions
	<tt>i</tt> and <tt>j</tt>, the member function removes every element for which
	<tt>i + 1 == j && pr(*Pi, *Pj)</tt>.
	*/
	template<class Pred>
	void unique(Pred pr)
	{
		node * it = this->m_pNext;
		while ( it != base_list::cast() ) {
			node * tmp = it->m_pNext;
			while ( tmp != base_list::cast() && pr(tmp->m_Value, it->m_Value) ) {
				erase(tmp);
				tmp = it->m_pNext;
			}
			it = tmp;
		}
	}

	/*!
	The member function inserts the sequence controlled by <tt>x</tt> before the element
	in the controlled sequence pointed to by <tt>pos</tt>. It also removes all elements
	from x. (&x must not equal this.)
	*/
	void splice(iterator pos, base_list & x)
	{
		splice(pos.m_Focus, x);
	}

	/*!
	The member function removes the element pointed to by <tt>first</tt> in the sequence
	controlled by <tt>x</tt> and inserts it before the element in the controlled
	sequence pointed to by <tt>pos</tt>.
	*/
	void splice(iterator pos, base_list & x, iterator first)
	{
		splice(pos.m_Focus, x, first.m_Focus);
	}

	/*!
	The member function inserts the subrange designated by <tt>[first, last)</tt>
	from the sequence controlled by <tt>x</tt> before the element in the controlled
	sequence pointed to by <tt>pos</tt>. It also removes the original subrange
	from the sequence controlled by <tt>x</tt>.
	*/
	void splice(iterator pos, base_list & x, iterator first, iterator last)
	{
		splice(pos.m_Focus, x, first.m_Focus, last.m_Focus);
	}

	/*!
	The member function removes all elements from the sequence controlled by <tt>x</tt>
	and insert them into the controlled sequence. Both sequences must be ordered by
	the same predicate, described below. The resulting sequence is also ordered by
	that predicate.

	For the iterators <tt>Pi</tt> and <tt>Pj</tt> designating elements at positions
	<tt>i</tt> and <tt>j</tt>, the member function imposes the order<br>
	<tt>!(*Pj < *Pi)</tt> whenever <tt>i < j</tt>.(The elements are sorted in ascending
	order.)

	No pairs of elements in the original controlled sequence are reversed in the
	resulting controlled sequence. If a pair of elements in the resulting controlled
	sequence compares equal <tt>(!(*Pi < *Pj) && !(*Pj < *Pi))</tt>, an element from
	the original controlled sequence appears before an element from the sequence
	controlled by <tt>x</tt>.
	*/
	void merge(base_list & x)
	{
		if ( &x != this ) {
			x.Notify(EventCode::Clear);

			node * f1 = this->m_pNext, * l1 = base_list::cast();
			node * f2 = x.m_pNext, * l2 = (node *)&x;
			while ( f1 != l1 && f2 != l2 ) {
				if ( f2->m_Value < f1->m_Value ) {
					node * m = f2->m_pNext;
					_splice(f1, x, f2, m);
					f2 = m;
				} else {
					f1 = f1->m_pNext;
				}
			}
			if ( f2 != l2 ) {
				_splice(l1, x, f2, l2);
			}
			this->m_nSize += x.m_nSize;
			x.m_nSize = 0;

			this->Notify(EventCode::Reload);
		}
	}

	/*!
	The member function removes all elements from the sequence controlled by <tt>x</tt>
	and insert them into the controlled sequence. Both sequences must be ordered by
	the same predicate, described below. The resulting sequence is also ordered by
	that predicate.

	For the iterators <tt>Pi</tt> and <tt>Pj</tt> designating elements at positions
	<tt>i</tt> and <tt>j</tt>, the member function imposes the order<br>
	<tt>!pr(*Pj, *Pi)</tt> whenever <tt>i < j</tt>.(The elements are sorted in ascending
	order.)

	No pairs of elements in the original controlled sequence are reversed in the
	resulting controlled sequence. If a pair of elements in the resulting controlled
	sequence compares equal <tt>(!(*Pi < *Pj) && !(*Pj < *Pi))</tt>, an element from
	the original controlled sequence appears before an element from the sequence
	controlled by <tt>x</tt>.
	*/
    template<class Pred>
	void merge(base_list & x, Pred pr)
	{
		if ( &x != this ) {
			x.Notify(EventCode::Clear);

			node * f1 = this->m_pNext, * l1 = base_list::cast();
			node * f2 = x.m_pNext, * l2 = x.cast();
			while ( f1 != l1 && f2 != l2 ) {
				if ( pr(f2->m_Value, f1->m_Value) ) {
					node * m = f2->m_pNext;
					_splice(f1, x, f2, m);
					f2 = m;
				} else {
					f1 = f1->m_pNext;
				}
			}
			if ( f2 != l2 ) {
				_splice(l1, x, f2, l2);
			}
			base_list::m_nSize += x.m_nSize;
			x.m_nSize = 0;

			this->Notify(EventCode::Reload);
		}
	}

	/*!
	The member function orders the elements in the controlled sequence by a predicate,
	described below.

	For the iterators <tt>Pi</tt> and <tt>Pj</tt> designating elements at positions
	<tt>i</tt> and <tt>j</tt>, the member function imposes the order<br>
	<tt>!(*Pj < *Pi)</tt> whenever <tt>i < j</tt>.
	(The elements are sorted in ascending order.) 
	 No pairs of elements in the original controlled sequence are reversed in the
	 resulting controlled sequence.
	*/
	void sort()
	{
		if ( base_list::size() >= 2 ) {
			this->LockNotify(true);

			const size_t _MAXN = 15;
			list2 x, arr[_MAXN + 1];
			size_t i, n = 0;
			while ( !base_list::empty() ) {
				x.splice(x.m_pNext, *this, this->m_pNext);
				for ( i = 0; i < n && !arr[i].empty(); ++i ) {
					arr[i].merge(x);
					arr[i].swap(x);
				}
				if ( i == _MAXN ) {
					arr[i].merge(x);
				} else {
					arr[i].swap(x);
					if (i == n) {
						++n;
					}
				}
			}
			while ( n > 0 ) {
				merge(arr[--n]); 
			}

			this->LockNotify(false);
			this->Notify(EventCode::Reorder);
		}
	}

	/*!
	The member function orders the elements in the controlled sequence by a predicate,
	described below.

	For the iterators <tt>Pi</tt> and <tt>Pj</tt> designating elements at positions
	<tt>i</tt> and <tt>j</tt>, the member function imposes the order<br>
	<tt>!pr(*Pj, *Pi)</tt> whenever <tt>i < j</tt>.
	(The elements are sorted in ascending order.) 
	 No pairs of elements in the original controlled sequence are reversed in the
	 resulting controlled sequence.
	*/
    template<class Pred>
	void sort(Pred pr)
	{
		if ( base_list::size() >= 2 ) {
			this->LockNotify(true);

			const size_t _MAXN = 15;
			list2 x, arr[_MAXN + 1];
			size_t i, n = 0;
			while (!base_list::empty()) {
				x.splice(x.m_pNext, *this, this->m_pNext);
				for (i = 0; i < n && !arr[i].empty(); ++i) {
					arr[i].merge(x, pr);
					arr[i].swap(x);
				}
				if ( i == _MAXN ) {
					arr[i].merge(x, pr);
				} else {
					arr[i].swap(x);
					if (i == n) {
						++n;
					}
				}
			}
			while ( n > 0 ) {
				merge(arr[--n], pr);
			}

			this->LockNotify(false);
			this->Notify(EventCode::Reorder);
		}
	}
};

} // namespace SCL

#endif // __SCL_CLIST_H__

