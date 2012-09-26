/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_VECTOR_H__
#define __SCL_VECTOR_H__

#include "stdlib.h"
#include "limits.h"

# include <new>

#include "SCL_Swapper.h"

namespace SCL
{

/*!
The template class describes vector traits for type <b>T</b>.
The template class <tt>vector</tt> uses this information to manipulate elements 
of type <tt>T</tt>. Such an element type must not require explicit construction
or destruction.
\note It's better to use class SCL::vector_traits_simple for simple types (embedded types and pointers).
*/
template <class T>
struct vector_traits
{
	/*!
	The static member function inits data in the range pointed by <tt>[start, end)</tt>
	with the values of corresponding range beginnning from <tt>from</tt> pointer.
	The function also calls a destructor for each element from the second range.
	\note The function uses <tt>operator ++ () </tt> to walk through pointer range.
	*/
    static void move_forward(T * & to, T * end, T * & from)
    {
        for ( ; to != end; ++from, ++to ) {
	        new (to) T(*from);
		    from->~T();
        }
    }
	/*!
	The static member function inits data in the range pointed by <tt>[start, end)</tt>
	with the values of corresponding range beginnning from <tt>from</tt> pointer.
	The function also calls a destructor for each element from the second range.
	\note The function uses <tt>operator -- () </tt> to walk through pointer range.
	*/
    static void move_backward(T * & to, T * end, T * & from)
    {
        for ( ; to != end; --from, --to ) {
	        new (to) T(*from);
		    from->~T();
        }
    }
};

/*!
The template class describes vector traits for type <b>T</b>.
The template class <tt>vector</tt> uses this information to manipulate elements 
of type <tt>T</tt>. Such an element type must not require explicit construction
or destruction.
vector_traits_simple is optimized for simple types (embedded types and pointers).
\note It's better to use class SCL::vector_traits for complex types.

There is a macro #SIMPLE_TRAITS(type) to create vector_traits_simple for simple type.
*/
template <class T>
struct vector_traits_simple
{
	/*!
	The static member function inits data in the range pointed by <tt>[start, end)</tt>
	with the values of corresponding range beginnning from <tt>from</tt> pointer.
	\note The function uses <tt>operator ++ () </tt> to walk through pointer range.
	*/
    static void move_forward(T * & start, T * end, T * & from)
    {
        if ( start != end ) {
            memmove(start, from, sizeof(T) * (end - start));
            from += end - start;
            start = end;
        }
    }
	/*!
	The static member function inits data in the range pointed by <tt>[start, end)</tt>
	with the values of corresponding range beginnning from <tt>from</tt> pointer.
	\note The function uses <tt>operator -- () </tt> to walk through pointer range.
	*/
    static void move_backward(T * & start, T * end, T * & from)
    {
        if ( start != end ) {
            int n = start - end;
            from -= n - 1;
            start -= n - 1;
            memmove(start, from, sizeof(T) * n);
			--from, --start;
        }
    }
};

/*!
The macro creates a child class <tt>template <> struct vector_traits<type></tt> 
from vector_traits_simple.<br>
The following classes are predefined using this macro:
<ul>
<li>template <> struct vector_traits<int>
<li>template <> struct vector_traits<unsigned int>
<li>template <> struct vector_traits<long>
<li>template <> struct vector_traits<unsigned long>
<li>template <> struct vector_traits<char>
<li>template <> struct vector_traits<unsigned char>
<li>template <> struct vector_traits<double>
<li>template <> struct vector_traits<float>
<li>template <> struct vector_traits<long double>
<li>template <> struct vector_traits<char *>
<li>template <> struct vector_traits<const char *>
<li>template <> struct vector_traits<void *>
<li>template <> struct vector_traits<const void *>
</ul>
*/
#define SIMPLE_TRAITS(type) template <> struct vector_traits<type> : public vector_traits_simple<type> {}

SIMPLE_TRAITS(int);
SIMPLE_TRAITS(unsigned int);
SIMPLE_TRAITS(long);
SIMPLE_TRAITS(unsigned long);
SIMPLE_TRAITS(char);
SIMPLE_TRAITS(unsigned char);
SIMPLE_TRAITS(double);
SIMPLE_TRAITS(float);
SIMPLE_TRAITS(long double);
SIMPLE_TRAITS(char *);
SIMPLE_TRAITS(const char *);
SIMPLE_TRAITS(void *);
SIMPLE_TRAITS(const void *);

#undef SIMPLE_TRAITS

template <class T> class const_vector_iter;

template <class T>
class vector_iter
{
protected:
	T * m_pFocus;

public:
	typedef vector_iter<T> iterator;
	typedef const_vector_iter<T> const_iterator;

	vector_iter()
	{
	}

	vector_iter(T * x)
	 : m_pFocus(x)
	{
	}

	vector_iter(const iterator & x)
	 : m_pFocus(x.m_pFocus)
	{
	}

	operator T * ()
	{
		return m_pFocus;
	}

	operator const T * () const
	{
		return m_pFocus;
	}

	// prefix versions
	iterator & operator ++ ()
	{
		++m_pFocus;
		return *this;
	}
	iterator & operator -- ()
	{
		--m_pFocus;
		return *this;
	}

	// postfix version
	iterator operator ++ (int)
	{
		T * retval = m_pFocus;
		++m_pFocus;
		return retval;
	}
	iterator operator -- (int)
	{
		T * retval = m_pFocus;
		++m_pFocus;
		return retval;
	}

	const T & get_value() const
	{
		return *m_pFocus;
	}

	void set_value(const T & a_value)
	{
		*m_pFocus = a_value;
	}

	handle get_handle() const
	{
		return (handle)m_pFocus;
	}

	void set_handle(handle a_handle)
	{
		m_pFocus = (T *)a_handle;
	}

};

template <class T>
class const_vector_iter
{
protected:
	const T * m_pFocus;

public:
	typedef vector_iter<T> iterator;
	typedef const_vector_iter<T> const_iterator;

	const_vector_iter()
	{
	}

	const_vector_iter(const T * x)
	 : m_pFocus(x)
	{
	}

	const_vector_iter(const iterator & x)
	 : m_pFocus(x.m_pFocus)
	{
	}

	const T & operator * () const
	{
		return *m_pFocus;
	}

	const T * operator -> () const
	{
		return m_pFocus;
	}

	const T & get_value() const
	{
		return *m_pFocus;
	}

	operator const T * () const
	{
		return m_pFocus;
	}

	// prefix versions
	const_iterator & operator ++ ()
	{
		++m_pFocus;
		return *this;
	}
	const_iterator & operator -- ()
	{
		--m_pFocus;
		return *this;
	}

	// postfix version
	const_iterator operator ++ (int)
	{
		const T * retval = m_pFocus;
		++m_pFocus;
		return retval;
	}
	const_iterator operator -- (int)
	{
		const T * retval = m_pFocus;
		++m_pFocus;
		return retval;
	}

	handle get_handle()
	{
		return (handle)m_pFocus;
	}

	void set_handle(handle a_handle)
	{
		m_pFocus = (const T *)a_handle;
	}
};

class vector_handle_update : public IHandleUpdate
{
protected:
	handle m_first, m_last;
	int m_offset;

	handle update(SCL::handle a_handle)
	{
		if ( a_handle >= m_first && a_handle < m_last ) {
			return ((char *)a_handle) + m_offset;
		}
		return a_handle;
	}

public:
	vector_handle_update(handle a_first, handle a_last, int a_offset)
		: m_first(a_first), m_last(a_last), m_offset(a_offset)
	{
	}
};

/*!
The template class describes an object that controls a varying-length sequence of elements
of type <b>T</b>. The sequence is stored as an array of <b>T</b>.

The object allocates and frees storage for the sequence it controls through a protected
object named <b>allocator</b>, <b>of class A</b>. Such an allocator object must have the 
same external interface as an object of template class SCL::alloc. 
\note that allocator is not copied when the object is assigned.

When inserting an item into a vector, the item is copied. So, the item has to support
a copy constructor.

<b>Vector reallocation</b> occurs when a member function must grow the controlled sequence 
beyond its current storage capacity. Other insertions and erasures may alter various storage 
addresses within the sequence. In all such cases, iterators or references that point at 
altered portions of the controlled sequence become invalid.
\note  SCL::vector does not support reverse_iterator.
*/
template <class T, class TRAITS = vector_traits<T>, class A = alloc, class N = NullNotifier>
class vector : public A, public N
{

public:
	/*!
	The type describes an object that can serve as a random-access iterator for the 
	controlled sequence.
	*/
    typedef vector_iter<T> iterator;
	/*!
	The type describes an object that can serve as a constant random-access iterator 
	for the controlled sequence.
	*/
    typedef const_vector_iter<T> const_iterator;

	/*!
	The type describes an object that can serve as a reference to an element of the 
	controlled sequence. It is described here as a synonym for the type <b>T &</b>.
	*/
    typedef T & reference;
	/*!
	The type describes an object that can serve as a constant reference to an element 
	of the controlled sequence.
	It is described here as a synonym for the type <b>const T &</b>.
	*/
    typedef const T & const_reference;

	/*!
	The unsigned integer type describes an object that can represent the length of 
	any controlled sequence.
	*/
    typedef typename A::size_type size_type;

protected:
	size_type m_nSize, m_nCapacity;
	T * m_pData;

	T * alloc_buffer(size_type size)
	{
        return (T *)A::_Charalloc(size * sizeof(T));
	}
	void free_buffer(T * buf)
	{
        if ( buf ) {
            A::deallocate((void *)buf, m_nCapacity * sizeof(T));
        }
	}

    void _insert(const size_type p, size_type n = 1, const T& x = T())
    {
        SCL_ASSERT(p <= m_nSize && n > 0);
        size_type old_size = m_nSize;
        size_type new_size = m_nSize + n;
        if ( m_nCapacity - m_nSize < n ) {
            size_type new_cap = 2*(n + m_nCapacity); // double the buffer size
		    T * pData = alloc_buffer(new_cap), * pd = pData, * pd1 = m_pData, * _end = pData + p;
            TRAITS::move_forward(pd, _end, pd1);
            _end += n;
            for ( ; pd != _end; ++pd ) {
                new (pd) T(x);
            }
            _end += m_nSize - p;
            TRAITS::move_forward(pd, _end, pd1);
			SCL::swap(m_pData, pData);
			m_nSize = new_size;
			if ( this->get_event() ) {
				vector_handle_update vhu(pData + p, pData + old_size, (m_pData - pData + n) * sizeof(T));
				this->Notify(EventCode::UpdateHandle, &vhu);
				for ( size_type i = 0; i < n; ++i ) {
					Notify(EventCode::Add, m_pData + p + i);
				}
			}
		    free_buffer(pData);
            m_nCapacity = new_cap;
        } else {
    	    T  * e = m_pData - 1, * pd1 = e + m_nSize, * pd = pd1 + n, * _end = e + p + n;
			SCL_ASSERT(p <= m_nSize);
			if ( p == m_nSize ) { // inserting at the very end
				for ( ; pd1 != _end; ++pd1 ) {
					new (pd1 + 1) T(x);
				}
			} else {
				TRAITS::move_backward(pd, _end, pd1);
				while ( pd != m_pData + p - 1 ) {
					*pd-- = x;
				}
			}
			m_nSize = new_size;
			if ( this->get_event() ) {
				vector_handle_update vhu(m_pData + p, m_pData + old_size, n * sizeof(T));
				this->Notify(EventCode::UpdateHandle, &vhu);
				for ( size_type i = 0; i < n; ++i ) {
					this->Notify(EventCode::Add, m_pData + p + i);
				}
			}
        }
    }

    void _erase(size_type p, size_type n = 1)
    {
		if ( this->get_event() ) {
			if ( p == 0 && n == m_nSize ) {
				this->Notify(EventCode::Clear);
			} else {
				for ( size_type i = 0; i < n; ++i ) {
					Notify(EventCode::Delete, m_pData + p + i);
				}
				vector_handle_update vhu(m_pData + p + n, m_pData + m_nSize, - (int)(n * sizeof(T)));
				this->Notify(EventCode::UpdateHandle, &vhu);
			}
		}
        SCL_ASSERT(p >= 0 && p <= m_nSize && n >= 0 && p + n <= m_nSize);
	    T * pd = m_pData + p, * pd1 = pd + n, * _end = m_pData + m_nSize - n;
        for ( ; pd != pd1; ++pd ) {
		    pd->~T();
        }
		pd = m_pData + p;
        TRAITS::move_forward(pd, _end, pd1);
        m_nSize -= n;
    }

public:
	/*!
	Default constructor. Constructs an empty vector.
	*/
    explicit vector()
	{
		m_nSize = m_nCapacity = 0;
		m_pData = NULL;
	}

	/*!
	The constructor initializes the controlled sequence with a repetition of <tt>n</tt> elements 
	of value <tt>v</tt>.
	*/
    explicit vector(size_type n, const T & v = T())
	{
		m_nSize = m_nCapacity = n;
		m_pData = alloc_buffer(n);
	    T * pd = m_pData - 1, * _end = pd + n;
		while ( pd != _end ) {
			new (++pd) T(v);
		}
	}

	/*!
	The constructor initializes the controlled sequence with a copy of the sequence 
	controlled by <tt>x</tt>.
	*/
    vector(const vector & x)
	{
		m_nSize = x.m_nSize;
		m_nCapacity = x.m_nCapacity;
		SCL_ASSERT(m_nSize <= m_nCapacity);

		m_pData = alloc_buffer(m_nCapacity);
	    T * pd = m_pData - 1, * pd1 = x.m_pData - 1, * _end = pd + m_nSize;
        while ( pd != _end ) { // do not use traits here, as we are copying from another vector!
			*++pd = *++pd1;
		}
	}

	/*!
	The constructor initializes the controlled sequence with a sequence <tt>[first, last)</tt>.
	*/
    vector(const_iterator first, const_iterator last)
	{
		m_nCapacity = m_nSize = last - first;
		m_pData = alloc_buffer(m_nCapacity);
	    T * pd = m_pData - 1, * _end = pd + m_nSize;
		while ( pd != _end ) {
			*++pd = *first++;
		}
	}

	/*!
	Desctructor. Calls a destructor of each element of the controlled sequence and
	frees allocated memory.
	*/
    ~vector()
    {
		this->Notify(EventCode::Clear);
        	SCL_ASSERT(m_pData || !m_nSize);
        	for ( T * p = m_pData; p != m_pData + m_nSize; ++p ) {
			p->~T();
   		}
		free_buffer(m_pData);
    }

	/*!
	The member operator specifies a copy of the sequence controlled by x.
	*/
	vector & operator = (const vector & x)
	{
		if ( &x != this ) {
			this->Notify(EventCode::Clear);
			m_nSize = x.m_nSize;
			m_nCapacity = x.m_nCapacity;
			SCL_ASSERT(m_nSize <= m_nCapacity);

			m_pData = alloc_buffer(m_nCapacity);
			T * pd = m_pData - 1, * pd1 = x.m_pData - 1, * _end = pd + m_nSize;
			while ( pd != _end ) { // do not use traits here, as we are copying from another vector!
				*++pd = *++pd1;
			}
			this->Notify(EventCode::Reload);
		}
		return *this;
	}

	/*!
	The member function ensures that <tt>capacity()</tt> henceforth returns at least <tt>n</tt>.
	*/
    void reserve(size_type n)
	{
        if ( n > m_nCapacity ) {
		    m_nCapacity = n;
		    T * pData = alloc_buffer(m_nCapacity), * pd = pData, * pd1 = m_pData;
            TRAITS::move_forward(pd, pd + m_nSize, pd1);
		    free_buffer(m_pData);
		    m_pData = pData;
        }
	}

	/*!
	The member function returns the storage currently allocated to hold the controlled 
	sequence, a value at least as large as <tt>size()</tt>.
	*/
    size_type capacity() const
	{
		return m_nCapacity;
	}

	/*!
	The member function returns a random-access iterator that points at the first 
	element of the sequence (or just beyond the end of an empty sequence).
	*/
    iterator begin()
	{
		return m_pData;
	}

	/*!
	The member function returns a random-access const iterator that points at the first 
	element of the sequence (or just beyond the end of an empty sequence).
	*/
    const_iterator begin() const
	{
		return m_pData;
	}

	/*!
	The member function returns a random-access iterator that points just beyond the 
	end of the sequence.
	*/
    iterator end()
	{
		return m_pData + m_nSize;
	}

	/*!
	The member function returns a random-access const iterator that points just beyond the 
	end of the sequence.
	*/
    const_iterator end() const
	{
		return m_pData + m_nSize;
	}

	/*!
	The member function returns an iterator that points at the end of 
	the controlled sequence. Hence, it designates the beginning of the reverse sequence.
	*/
    iterator rbegin()
	{
		return m_pData + m_nSize - 1;
	}

	/*!
	The member function returns a const iterator that points at the end of 
	the controlled sequence. Hence, it designates the beginning of the reverse sequence.
	*/
    const_iterator rbegin() const
	{
		return m_pData + m_nSize - 1;
	}

	/*!
	The member function returns an iterator that points at the element before the first element 
	of the controlled sequence (or just beyond the end of an empty sequence). 
	Hence, it designates the end of the reverse sequence.
	*/
    iterator rend()
	{
		return m_pData - 1;
	}

	/*!
	The member function returns a const iterator that points at the element before the first element 
	of the controlled sequence (or just beyond the end of an empty sequence). 
	Hence, it designates the end of the reverse sequence.
	*/
    const_iterator rend() const
	{
		return m_pData - 1;
	}

	/*!
	The member function ensures that <tt>size()</tt> henceforth returns <tt>n</tt>. 
	If it must lengthen the controlled sequence, it appends elements with value <tt>x</tt>.
	*/
    void resize(size_type n, T x = T())
    {
        if ( n > m_nSize ) {
			_insert(m_nSize, n - m_nSize, x);
        } else if ( n < m_nSize ) {
			_erase(n, m_nSize);
        }
    }

	/*!
	The member function returns the length of the controlled sequence.
	*/
    size_type size() const
	{
		return m_nSize;
	}

	/*!
	The member function returns the length of the longest sequence that the object can control.
	*/    
    size_type max_size() const
	{
		return INT_MAX/sizeof(T);
	}

	/*!
	The member function returns true for an empty controlled sequence.
	*/
    bool empty() const
	{
		return m_nSize == 0;
	}

	/*!
	The member function returns a reference to the element of the controlled sequence 
	at position <tt>pos</tt>. If that position is invalid, the function performs assertion failed.
	*/
    T & at(size_type pos)
	{
		SCL_ASSERT(pos < m_nSize);
		return m_pData[pos];
	}

	/*!
	The member function returns a const reference to the element of the controlled sequence 
	at position <tt>pos</tt>. If that position is invalid, the function performs assertion failed.
	*/
    const T & at(size_type pos) const
	{
		SCL_ASSERT(pos < m_nSize);
		return m_pData[pos];
	}

	/*!
	The member function returns a reference to the element of the controlled sequence 
	at position <tt>pos</tt>. If that position is invalid, the function performs assertion failed.
	*/
    T & operator [](size_type pos)
	{
		SCL_ASSERT(pos < m_nSize);
		return m_pData[pos];
	}

	/*!
	The member function returns a const reference to the element of the controlled sequence 
	at position <tt>pos</tt>. If that position is invalid, the function performs assertion failed.
	*/
    const T & operator [](size_type pos) const
	{
		SCL_ASSERT(pos < m_nSize);
		return m_pData[pos];
	}

	/*!
	The member function returns a reference to the first element of the controlled sequence, 
	which must be non-empty.
	*/
    T & front()
	{
		SCL_ASSERT(m_nSize > 0);
		return m_pData[0];
	}

	/*!
	The member function returns a const reference to the first element of the controlled sequence, 
	which must be non-empty.
	*/
    const T & front() const
	{
		SCL_ASSERT(m_nSize > 0);
		return m_pData[0];
	}

	/*!
	The member function returns a reference to the last element of the controlled sequence,
	which must be non-empty.
	*/
    T & back()
	{
		SCL_ASSERT(m_nSize > 0);
		return m_pData[m_nSize - 1];
	}

	/*!
	The member function returns a const reference to the last element of the controlled sequence,
	which must be non-empty.
	*/
    const T & back() const
	{
		SCL_ASSERT(m_nSize > 0);
		return m_pData[m_nSize - 1];
	}

	/*!
	The member function inserts an element with value <tt>x</tt> at the end of the
	controlled sequence.
	*/
    void push_back(const T& x)
	{
		_insert(m_nSize, 1, x);
	}

	/*!
	The member function removes the last element of the controlled sequence,
	which must be non-empty.
	*/
    void pop_back()
	{
		_erase(m_nSize - 1);
	}

	/*!
	The member function inserts a single element with value <tt>x</tt> before the element
	at position <tt>pos</tt> in the controlled sequence and returns an iterator that
	points to the newly inserted element. 
	*/
    iterator insert(const size_type p, const T& x = T())
    {
        SCL_ASSERT(p >= 0 && p <= m_nSize);
        _insert(p, 1, x);
        return m_pData + p;
    }

	/*!
	The member function inserts a repetition of <tt>n</tt> elements of value <tt>x</tt>
	before the element at position <tt>pos</tt> in the controlled sequence and returns
	an iterator that points to the newly inserted element.
	*/
    iterator insert(const size_type p, size_type n, const T& x = T())
    {
        SCL_ASSERT(p >= 0 && p <= m_nSize);
        _insert(p, n, x);
        return m_pData + p;
    }

	/*!
	The member function inserts a single element with value <tt>x</tt> before the element
	pointed to by <tt>it</tt> in the controlled sequence and returns an iterator that
	points to the newly inserted element. 
	*/
    iterator insert(iterator it, const T& x = T())
    {
        SCL_ASSERT(size_type(it - m_pData) >= 0);
        SCL_ASSERT(size_type(it - m_pData) <= m_nSize);
        _insert(size_type(it - m_pData), 1, x);
        return it;
    }

	/*!
	The member function inserts a repetition of <tt>n</tt> elements of value <tt>x</tt>
	before the element pointed to by <tt>it</tt> in the controlled sequence and returns
	an iterator that points to the newly inserted element. 
	*/
    iterator insert(iterator it, size_type n, const T& x = T())
    {
        SCL_ASSERT(size_type(it - m_pData) >= 0);
        SCL_ASSERT(size_type(it - m_pData) <= m_nSize);
        _insert(size_type(it - m_pData), n, x);
        return it;
    }

	/*!
	The member function inserts the sequence <tt>[first, last)</tt>
	before the element pointed to by <tt>it</tt> in the controlled sequence and returns
	an iterator that points to the newly inserted element. 
	*/
	template <class In>
	void insert(iterator it, In first, In last)
	{
		SCL_ASSERT(size_type(it - m_pData) >= 0);
        SCL_ASSERT(size_type(it - m_pData) <= m_nSize);
		size_type pos = size_type(it - m_pData);
		for(In in_it = first; in_it != last; ++in_it)
		{
			_insert(pos++, 1, *in_it);
		}
	}

	/*!
	The member function removes the <tt>n</tt> elements of the controlled sequence
	beginning from element pointed at position <tt>p</tt> and returns an iterator that
	designates the first element remaining beyond
	any elements removed, or <tt>end()</tt> if no such element exists.
	\note The function calls a destructor of erased element.
	*/
    iterator erase(const size_type p, size_type n = 1)
    {
        SCL_ASSERT(p >= 0 && n > 0 && p + n <= m_nSize);
        _erase(p, n);
        return m_pData + p;
    }

	/*!
	The member function removes the element of the controlled sequence pointed to by 
	<tt>it</tt> and returns an iterator that designates the first element remaining beyond
	any elements removed, or <tt>end()</tt> if no such element exists.
	\note The function calls a destructor of erased element.
	*/
    iterator erase(iterator it)
    {
        SCL_ASSERT(size_type(it - m_pData) >= 0);
        SCL_ASSERT(size_type(it - m_pData) <= m_nSize);
        _erase((T *)it - m_pData);
        return it;
    }

	/*!
	The member function removes the elements of the controlled sequence in the range 
	<tt>[first, last)</tt> and returns an iterator that designates the first element
	remaining beyond any elements removed, or <tt>end()</tt> if no such element exists. 
	\note The function calls destructor of all erased elements.
	*/
    iterator erase(iterator first, iterator last)
    {
        SCL_ASSERT(size_type(last - m_pData) >= 0);
        SCL_ASSERT(size_type(last - m_pData) <= m_nSize);
        _erase(size_type(first - m_pData), size_type(last - first));
        return first;
    }

	/*!
	The member function calls destructors of all elements stored in the controlled 
	sequence and deallocates used memory for each element. 
	*/
    void clear()
	{
        _erase(0, m_nSize);
	}

	/*!
	The member function swaps the controlled sequences between <tt>*this</tt> and <tt>x</tt>. 
	*/
    void swap(vector & x)
	{
		if ( &x != this ) {
			SCL::swap(m_nSize, x.m_nSize);
			SCL::swap(m_nCapacity, x.m_nCapacity);
			SCL::swap(m_pData, x.m_pData);
			this->Notify(EventCode::Reload);
			x.Notify(EventCode::Reload);
		}
	}
};

} // namespace SCL

#endif // __SCL_VECTOR_H__

