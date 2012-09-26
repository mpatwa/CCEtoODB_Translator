/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYS_STRING_H__
#define __SYS_STRING_H__

#include "wchar.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#include "SCL_HashComp.h"
#include "SCL_Swapper.h"

// HACK: some compilers define these symbols, which messes up everything below.
// this should be in compat.h, but the includes above mess this up.
#undef _U
#undef _N
#undef _C
#undef _V

namespace SCL
{

/*!
The template class describes various character traits for type <b>CHAR</b>.
The template class basic_string use this information to manipulate elements of type <b>E</b>.
Such an element type must not require explicit construction or destruction. 
*/
template<class CHAR>
struct char_traits
{
	/*!
	The type is a synonym for the template parameter <tt>CHAR</tt>.
	*/
    typedef CHAR char_type;
	/*!
	The type is (typically) an integer type that describes an object that can 
	represent any element of the controlled sequence as well as the value returned by 
	<tt>eof()</tt>. It must be possible to type cast a value of type <tt>CHAR</tt>
	to <tt>int_type</tt> then back to <tt>CHAR</tt> without altering the original value.
	In addition, the expression <tt>int_type('\\0')</tt> must yield the code that 
	terminates a null-terminated string for elements of type <tt>CHAR</tt>.
	Also, the expression <tt>int_type('\\n')</tt> must yield a suitable newline
	character of type <tt>CHAR</tt>.
	*/
    typedef CHAR int_type;

	/*!
	The static member function assigns <tt>_Y</tt> to <tt>x</tt>.
	*/
    static void assign(CHAR& x, const CHAR& _Y)
    {
		x = _Y;
	}
	/*!
	The static member function assigns <tt>_C</tt> to each element <tt>_U[n]</tt>
	for <tt>n</tt> in the range <tt>[0, _N)</tt> and returns <tt>_U</tt>.
	*/
    static CHAR * assign(CHAR *_U, size_t _N, const CHAR& _C)
	{
		CHAR *_Ans = _U;
		for (; 0 < _N; --_N, ++_U) {
			assign(*_U, _C);
		}
		return (_Ans);
	}
	/*!
	The static member function returns true if <tt>x</tt> compares equal to <tt>_Y</tt>.
	*/
    static bool eq(const CHAR& x, const CHAR& _Y)
	{
		return (x == _Y);
	}
	/*!
	The static member function returns <tt>true</tt> if <tt>x</tt> compares less than
	<tt>_Y</tt>, otherwise - <tt>false</tt>.
	*/
    static bool lt(const CHAR& x, const CHAR& _Y)
	{
		return (x < _Y);
	}
	/*!
	The static member function compares the sequence of length <tt>N</tt> beginning
	at <tt>_U</tt> to the sequence of the same length beginning at <tt>_V</tt>.
	The function returns:
	<ul>
		<li>A negative value if the first differing element in <tt>_U</tt>
		(as determined by <tt>eq()</tt>) compares less than the corresponding element
		in <tt>_V</tt> (as determined by <tt>lt()</tt>). 

		<li>Zero if the two compare equal element by element. 

		<li>A positive value otherwise. 
	</ul>
	*/
    static int compare(const CHAR *_U, const CHAR *_V, size_t _N)
	{
		for (size_t _I = 0; _I < _N; ++_I, ++_U, ++_V)
			if (!eq(*_U, *_V))
				return (lt(*_U, *_V) ? -1 : +1);
            return (0);
	}
	/*!
	The static member function returns the number of elements <tt>N</tt> in the
	sequence beginning at <tt>_U</tt> up to but not including the element <tt>_U[N]</tt>
	that compares equal to <tt>CHAR(0)</tt>.
	*/
    static size_t length(const CHAR *_U)
    {
		size_t _N;
        for (_N = 0; !eq(*_U, CHAR(0)); ++_U) {
			++_N;
		}
		return (_N);
	}
	/*!
	The static member function copies the sequence of <tt>_N</tt> elements beginning
	at <tt>_V</tt> to the array beginning at <tt>_U</tt>, then returns <tt>_U</tt>.
	The source and destination must not overlap.
	\note To copy overlap ranges you should use <tt>move()</tt>.
	*/
    static CHAR * copy(CHAR *_U, const CHAR *_V, size_t _N)
	{
		CHAR *s = _U;
		for (; 0 < _N; --_N, ++_U, ++_V) {
			assign(*_U, *_V);
		}
		return (s);
	}
	/*!
	The static member function determines the lowest <tt>n</tt> in the range 
	<tt>[0, _N)</tt> for which <tt>eq(_U[n], _C)</tt> is <tt>true</tt>.
	If successful, it returns <tt>_U + n</tt>. Otherwise, it returns a <tt>null</tt> pointer.
	*/
	static const CHAR * find(const CHAR *_U, size_t _N, const CHAR& _C)
	{
		for (; 0 < _N; --_N, ++_U) {
			if (eq(*_U, _C)) {
				return (_U);
			}
            return (0);
		}
	}
	/*!
	The static member function copies the sequence of <tt>_N</tt> elements beginning
	at <tt>_V</tt> to the array beginning at <tt>_U</tt>, and then returns <tt>_U</tt>.
	\note The difference betwen <tt>copy()</tt> and <tt>move()</tt> is in that
	<tt>move()</tt> works correctly even if <tt>_V</tt> is in the <tt>[_U, _U+_N)</tt> range.
	So <tt>copy()</tt> can be faster than <tt>move()</tt> for non-overlapping ranges.
	*/
	static CHAR * move(CHAR *_U, const CHAR *_V, size_t _N)
	{
		CHAR *_Ans = _U;
		if (_V < _U && _U < _V + _N) {
			for (_U += _N, _V += _N; 0 < _N; --_N) {
				assign(*--_U, *--_V);
			}
		} else {
			for (; 0 < _N; --_N, ++_U, ++_V) {
				assign(*_U, *_V);
			}
		}
		return (_Ans);
	}
	/*!
	The static member function returns <tt>_C</tt>, represented as type <tt>CHAR</tt>.
	A value of <tt>_C</tt> that cannot be so represented yields an unspecified result.
	*/
    static CHAR to_char_type(const int_type& _C)
	{
		return ((CHAR)_C);
	}
	/*!
	The static member function returns <tt>_C</tt>, represented as type <tt>int_type</tt>.
	It should always be true that <tt>to_char_type(to_int_type(_C)) == _C</tt>  for any
	value of <tt>_C</tt>.
	*/
    static int_type to_int_type(const CHAR& _C)
	{
		return ((int_type)_C);
	}
	/*!
	The static member function returns <tt>true</tt> if <tt>x == _Y</tt>,
	<tt>false</tt> otherwise.
	*/
    static bool eq_int_type(const int_type& x, const int_type& _Y)
	{
		return (x == _Y);
	}
	/*!
	The static member function returns a value that represents <tt>end-of-file</tt>
	(such as <tt>EOF</tt>). If the value is also representable as type <tt>CHAR</tt>,
	it must correspond to no valid value of that type.
	*/
	static int_type eof()
	{
		return (EOF);
	}
	/*!
	If <tt>!eq_int_type(eof(), _C)</tt>, the static member function returns <tt>_C</tt>.
	Otherwise, it returns a value other than <tt>eof()</tt>.
	*/
	static int_type not_eof(const int_type& _C)
	{
		return (_C != eof() ? _C : !eof());
	}
};

/*!
The class is an explicit specialization of template class <tt>char_traits</tt> for
elements of type <tt>wchar_t</tt> (so it can take advantage of library functions
that manipulate objects of this type).
*/
template<> struct
char_traits<wchar_t>
{
	/*!
	The type is a synonym for the <tt>wchar_t</tt>.
	*/
	typedef wchar_t CHAR;
	/*!
	The type is a synonym for the <tt>CHAR</tt>.
	*/
	typedef CHAR char_type;   // for overloads
	/*!
	The type is (typically) an integer type that describes an object that can 
	represent any element of the controlled sequence as well as the value returned by 
	<tt>eof()</tt>. It must be possible to type cast a value of type <tt>wchar_t</tt>
	to <tt>int_type</tt> then back to <tt>wchar_t</tt> without altering the original value.
	In addition, the expression <tt>int_type('\\0')</tt> must yield the code that 
	terminates a null-terminated string for elements of type <tt>wchar_t</tt>.
	Also, the expression <tt>int_type('\\n')</tt> must yield a suitable newline
	character of type <tt>wchar_t</tt>.
	*/
	typedef wint_t int_type;

	/*!
	\see SCL::char_traits< CHAR >::assign(CHAR& x, const CHAR& _Y)
	*/
	static void assign(CHAR& x, const CHAR& _Y)
	{
		x = _Y;
	}
	/*!
	\see SCL::char_traits< CHAR >::eq(const CHAR& x, const CHAR& _Y)
	*/
	static bool eq(const CHAR& x, const CHAR& _Y)
	{
		return (x == _Y);
	}
	/*!
	\see SCL::char_traits< CHAR >::lt(const CHAR& x, const CHAR& _Y)
	*/
	static bool lt(const CHAR& x, const CHAR& _Y)
	{
		return (x < _Y);
	}
	/*!
	\see SCL::char_traits< CHAR >::compare(const CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static int compare(const CHAR *_U, const CHAR *_V, size_t _N)
	{
		return (wmemcmp(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::compare(const CHAR *_U, const CHAR *_V)
	*/
	static int compare(const CHAR *_U, const CHAR *_V)
	{
		return (wcscmp(_U, _V));
	}
	/*!
	\see SCL::char_traits< CHAR >::length(const CHAR *_U)
	*/
	static size_t length(const CHAR *_U)
	{
		return (wcslen(_U));
	}
	/*!
	\see SCL::char_traits< CHAR >::copy(CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static CHAR *copy(CHAR *_U, const CHAR *_V, size_t _N)
	{
		return (wmemcpy(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::find(const CHAR *_U, size_t _N, const CHAR& _C)
	*/
	static const CHAR * find(const CHAR *_U, size_t _N, const CHAR& _C)
	{
		return ((const CHAR *)wmemchr(_U, _C, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::move(CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static CHAR * move(CHAR *_U, const CHAR *_V, size_t _N)
	{
		return (wmemmove(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::assign(CHAR *_U, size_t _N, const CHAR& _C)
	*/
	static CHAR * assign(CHAR *_U, size_t _N, const CHAR& _C)
	{
		return (wmemset(_U, _C, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::to_char_type(const int_type& _C)
	*/
	static CHAR to_char_type(const int_type& _C)
	{
		return ((CHAR)_C);
	}
	/*!
	\see SCL::char_traits< CHAR >::to_int_type(const CHAR& _C)
	*/
	static int_type to_int_type(const CHAR& _C)
	{
		return ((int_type)_C);
	}
	/*!
	\see SCL::char_traits< CHAR >::eq_int_type(const int_type& x, const int_type& _Y)
	*/
	static bool eq_int_type(const int_type& x, const int_type& _Y)
	{
		return (x == _Y);
	}
	/*!
	\see SCL::char_traits< CHAR >::int_type eof()
	*/
	static int_type eof()
	{
		return (WEOF);
	}
	/*!
	\see SCL::char_traits< CHAR >::not_eof(const int_type& _C)
	*/
	static int_type not_eof(const int_type& _C)
	{
		return (_C != eof() ? _C : !eof());
	}
	/*!
	The static member function takes a pointer to an argument list <tt>v</tt>,
	then formats it using format specification <tt>fmt</tt> and writes the given
	data to the memory pointed to by <tt>s</tt> but the maximum number of characters
	to write cannot be more than <tt>n</tt>.

	The function returns the number of characters written, not including the terminating null
	character, or a negative value if an output error occurs. If the number of bytes
	to write exceeds buffer, then count bytes are written and <tt>–1</tt> is returned.

	*/
	static int vprintf(CHAR * s, size_t n, const CHAR * fmt, va_list v)
	{
#ifndef UNIX
		CHAR *arg1 = s;
		const CHAR *arg3 = fmt;
#else
		char *arg1 = (char *) s;
		const char *arg3 = (const char *) fmt;
#endif // !UNIX

		return _vsnwprintf(arg1, n, arg3, v);
	}
};

/*!
The class is an explicit specialization of template class <tt>char_traits</tt> for
elements of type <tt>char</tt> (so it can take advantage of library functions
that manipulate objects of this type).
*/
template<> struct
char_traits<char>
{
	/*!
	The type is a synonym for the <tt>char</tt>.
	*/
	typedef char CHAR;
	/*!
	The type is a synonym for the <tt>CHAR</tt>.
	*/
	typedef CHAR char_type;
	/*!
	The type is (typically) an integer type that describes an object that can 
	represent any element of the controlled sequence as well as the value returned by 
	<tt>eof()</tt>. It must be possible to type cast a value of type <tt>char</tt>
	to <tt>int_type</tt> then back to <tt>char</tt> without altering the original value.
	In addition, the expression <tt>int_type('\\0')</tt> must yield the code that 
	terminates a null-terminated string for elements of type <tt>char</tt>.
	Also, the expression <tt>int_type('\\n')</tt> must yield a suitable newline
	character of type <tt>char</tt>.
	*/
	typedef int int_type;

	/*!
	\see SCL::char_traits< CHAR >::assign(CHAR& x, const CHAR& _Y)
	*/
	static void assign(CHAR& x, const CHAR& _Y)
	{
		x = _Y;
	}
	/*!
	\see SCL::char_traits< CHAR >::eq(const CHAR& x, const CHAR& _Y)
	*/
	static bool eq(const CHAR& x, const CHAR& _Y)
	{
		return (x == _Y);
	}
	/*!
	\see SCL::char_traits< CHAR >::lt(const CHAR& x, const CHAR& _Y)
	*/
	static bool lt(const CHAR& x, const CHAR& _Y)
	{
		return (x < _Y);
	}
	/*!
	\see SCL::char_traits< CHAR >::compare(const CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static int compare(const CHAR *_U, const CHAR *_V, size_t _N)
	{
		return (memcmp(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::compare(const CHAR *_U, const CHAR *_V)
	*/
	static int compare(const CHAR *_U, const CHAR *_V)
	{
		return (strcmp(_U, _V));
	}
	/*!
	\see SCL::char_traits< CHAR >::length(const CHAR *_U)
	*/
	static size_t length(const CHAR *_U)
	{
		return (strlen(_U));
	}
	/*!
	\see SCL::char_traits< CHAR >::copy(CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static CHAR * copy(CHAR *_U, const CHAR *_V, size_t _N)
	{
		return ((CHAR *)memcpy(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::find(const CHAR *_U, size_t _N, const CHAR& _C)
	*/
	static const CHAR * find(const CHAR *_U, size_t _N, const CHAR& _C)
	{
		return ((const CHAR *)memchr(_U, _C, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::move(CHAR *_U, const CHAR *_V, size_t _N)
	*/
	static CHAR * move(CHAR *_U, const CHAR *_V, size_t _N)
	{
		return ((CHAR *)memmove(_U, _V, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::assign(CHAR *_U, size_t _N, const CHAR& _C)
	*/
	static CHAR * assign(CHAR *_U, size_t _N, const CHAR& _C)
	{
		return ((CHAR *)memset(_U, _C, _N));
	}
	/*!
	\see SCL::char_traits< CHAR >::to_char_type(const int_type& _C)
	*/
	static CHAR to_char_type(const int_type& _C)
	{
		return ((CHAR)_C);
	}
	/*!
	\see SCL::char_traits< CHAR >::to_int_type(const CHAR& _C)
	*/
	static int_type to_int_type(const CHAR& _C)
	{
		return ((int_type)((unsigned char)_C));
	}
	/*!
	\see SCL::char_traits< CHAR >::eq_int_type(const int_type& x, const int_type& _Y)
	*/
	static bool eq_int_type(const int_type& x, const int_type& _Y)
	{
		return (x == _Y);
	}
	/*!
	The static member function takes a pointer to an argument list <tt>v</tt>,
	then formats it using format specification <tt>fmt</tt> and writes the given
	data to the memory pointed to by <tt>s</tt> but the maximum number of characters
	to write cannot be more than <tt>n</tt>.

	The function returns the number of characters written, not including the terminating null
	character, or a negative value if an output error occurs. If the number of bytes
	to write exceeds buffer, then count bytes are written and <tt>–1</tt> is returned.

	*/
	static int vprintf(CHAR * s, size_t n, const CHAR * fmt, va_list v)
	{
		return _vsnprintf(s, n, fmt, v);
	}
};

template <class CHAR, class TRAITS, class A> class basic_string2;

/*!
The template class describes an object that controls a varying-length sequence 
of elements of type <tt><b>CHAR</b></tt>. Such an element type must not require
explicit construction or destruction. 
\note 
There are two classes:
<ol>
	<li>SCL::basic_string
	<li>SCL::basic_string2
</ol>
The first one contains a minimum needed set of function and is more faster than
the second one which has a wide number of functions.<br>
Two typedefs provide conventional names for theese string types:
<ol>
	<li><tt>typedef basic_string<char> string;</tt>
	<li><tt>typedef basic_string2<char> string2</tt>;
</ol>

Various important properties of the elements in a basic_string specialization
are described by the class <tt><b>TRAITS</b></tt>. A class that specifies these character
traits must have the same external interface as an object of template class <tt>char_traits</tt>.

The object allocates and frees storage for the sequence it controls using interface
of allocator class <tt><b>A</b></tt>. Such an allocator class must have the same external 
interface as an object of template class <tt>allocator</tt>.

The sequences controlled by an object of template class <tt>basic_string</tt> are
usually called <tt>strings</tt>. These objects should not be confused, however, with
the null-terminated C strings.
*/
template<typename CHAR, typename TRAITS = char_traits<CHAR>, typename A = allocator<CHAR> >
class basic_string : public A
{
	friend /*template*/ class basic_string2<CHAR, TRAITS, A>;

public:
	/*!
	The typedef provides conventional name for common <tt>string</tt> type.
	*/
	typedef basic_string<CHAR, TRAITS, A> string;
	/*!
	The unsigned integer type describes an object that can represent the length
	of any controlled sequence.
	*/
	typedef typename A::size_type size_type;
	/*!
	The signed integer type describes an object that can represent the difference
	between the addresses of any two elements in the controlled sequence.
	*/
	typedef typename A::difference_type difference_type;
	/*!
	The type describes an object that can serve as a pointer to an element of 
	the controlled sequence.
	*/
	typedef typename A::pointer pointer;
	/*!
	The type describes an object that can serve as a constant pointer to
	an element of the controlled sequence.
	*/
	typedef typename A::const_pointer const_pointer;
	/*!
	The type describes an object that can serve as a reference to an 
	element of the controlled sequence.
	*/
	typedef typename A::reference reference;
	/*!
	The type describes an object that can serve as a constant reference to an
	element of the controlled sequence.
	*/
	typedef typename A::const_reference const_reference;
	/*!
	The type is a synonym for <tt>A::value_type</tt>.
	*/	
	typedef typename A::value_type value_type;
	/*!
	The type describes an object that can serve as a random-access iterator for
	the controlled sequence. It is described here as a synonym for the <tt>A::pointer</tt>.
	*/	
	typedef typename A::pointer iterator;
	/*!
	The type describes an object that can serve as a constant random-access iterator for
	the controlled sequence. It is described here as a synonym for the <tt>A::const_pointer</tt>.
	*/	
	typedef typename A::const_pointer const_iterator;

	/*!
	The type is a synonym for the template parameter <tt>TRAITS</tt>.
	*/
	typedef TRAITS traits_type;
	/*!
	The type is a synonym for the template parameter <tt>A</tt>.
	*/
	typedef A allocator_type;

protected:
	static CHAR nil[1];

	struct str_data
	{
		int m_nRefCount;
		size_type m_nLength, m_nAllocated;
	};

	pointer m_pData;

	str_data * get_data() const
	{
		str_data * data = ((str_data *)m_pData) - 1;
		return data;
	}

	size_type get_alloc_size(size_type n)
	{
		return n;
	}

	pointer alloc_buffer(size_type n)
	{
		size_type alloc_size = get_alloc_size(n);
		str_data * pData = (str_data *)A::_Charalloc((alloc_size + 1) * sizeof(CHAR) + sizeof(str_data));
		pData->m_nRefCount = 1;
		pData->m_nLength = n;
		pData->m_nAllocated = alloc_size;
		pointer p = (pointer)(pData + 1);
		p[n] = CHAR(0);
		return p;
	}

	void dealloc_buffer(str_data * pData)
	{
		if ( pData + 1 != NULL && --pData->m_nRefCount == 0 ) {
			A::deallocate(pData, (pData->m_nAllocated + 1) * sizeof(CHAR) + sizeof(str_data));
		}
	}

	void prepare_buffer(size_type n)
	{
		if ( m_pData ) {
			str_data * pData = get_data();
			if ( n > pData->m_nAllocated || pData->m_nRefCount > 1 ) {
				dealloc_buffer(pData);
				m_pData = NULL;
			}
		}
		if ( !m_pData ) {
			m_pData = alloc_buffer(n);
		} else {
			get_data()->m_nLength = n;
		}
		m_pData[n] = 0;
	}

	SYSCORE_API void _append(const_pointer s, size_type n);
	SYSCORE_API void _insert(size_type pos, size_type n);
	SYSCORE_API void _erase(size_type pos, size_type n = -1);
	SYSCORE_API void _replace(size_type pos, size_type n, size_type n1);
	SYSCORE_API int _compare(const_pointer s1, size_type n1, const_pointer s2, size_type n2) const;

public:

	enum {
		/*!
		The constant is the largest representable value of type size_type. It is assuredly
		larger than <tt>max_size()</tt>; hence it serves as either a very large value or as 
		a special code.
		*/
		npos = -1
	};

	/*!
	Default constructor. Inits an empty controlled sequence.
	*/
	basic_string()
	{
		m_pData = NULL;
	}
	/*!
	Copy constructor. In fact it does not copy a sequence controlled by <tt>x</tt>,
	it only increases a number of references to the controlled sequence.
	*/
	basic_string(const string & x)
	{
		if ( x.m_pData ) {
			++x.get_data()->m_nRefCount;
		}
		m_pData = x.m_pData;
	}
	/*!
	The constructor creates a controlled sequence from <tt>const_pointer s</tt>.
	*/
	basic_string(const_pointer s)
	{
		if ( !s ) {
			m_pData = NULL;
		} else {
			int n = TRAITS::length(s);
			m_pData = alloc_buffer(n);
			TRAITS::copy(m_pData, s, n + 1);
		}
	}
	/*!
	The constructor creates a controlled sequence from sequence controlled
	by <tt>x</tt> from start position <tt>pos</tt> and number of elements <tt>n</tt>.
	\note If <tt>pos == 0 and n == x.size()</tt> then the constructor acts like
	basic_string(const string & x).
	*/
	basic_string(const string & x, size_type pos, size_type n)
	{
		SCL_ASSERT(n >= 0);
		SCL_ASSERT(x.size() >= pos);

		size_type xn = x.size() - pos;
		if ( n > xn ) {
			n = xn;
		}
		if ( pos == 0 && n == xn ) {
			if ( (m_pData = x.m_pData) != NULL ) {
				++get_data()->m_nRefCount;
			}
		} else if ( n ) {
			m_pData = alloc_buffer(n);
			TRAITS::copy(m_pData, x.m_pData + pos, n);
		} else {
			m_pData = NULL;
		}
	}
	/*!
	The constructor creates a controlled sequence from <tt>const_pointer s</tt>.<br>
	The sequence can't be longer than <tt>n</tt>.
	*/
	basic_string(const_pointer s, size_type n)
	{
		if ( !s ) {
			m_pData = NULL;
		} else {
			SCL_ASSERT(n <= TRAITS::length(s));
			m_pData = alloc_buffer(n);
			TRAITS::copy(m_pData, s, n);
			m_pData[n] = 0;
		}
	}
	/*!
	The constructor creates a controlled sequence of length <tt>n</tt> filled
	with value <tt>c</tt>.
	*/
	basic_string(size_type n, CHAR c)
	{
		m_pData = alloc_buffer(n);
		TRAITS::assign(m_pData, n, c);
	}
	/*!
	The constructor inits the controlled sequence from sequence pointed by <tt>[first, last)</tt>.
	*/
	basic_string(const_iterator first, const_iterator last)
	{
		size_type n = last - first;
		m_pData = alloc_buffer(n);
		TRAITS::copy(m_pData, first, n);
	}

	/*!
	Destructor. If no other objects refer to the controlled sequence it frees allocated memory.
	*/
	virtual ~basic_string()
	{
		dealloc_buffer(get_data());
	}

	/*!
	The operator replaces the sequence controlled by <tt>*this</tt> with the operand
	sequence, then returns <tt>*this</tt>.
	\note If sequence from <tt>x</tt> is not empty then the operator increases
	a reference to the data without any allocation.
	*/
	string& operator = (const string & x)
	{
		if ( &x != this ) {
			if ( x.m_pData ) {
				++x.get_data()->m_nRefCount;
			}
			dealloc_buffer(get_data());
			m_pData = x.m_pData;
		}
		return *this;
	}
	/*!
	The operator replaces the sequence controlled by <tt>*this</tt> with the operand
	sequence, then returns <tt>*this</tt>.
	*/
	string& operator = (const_pointer s)
	{
		if ( !s || !*s ) {
			if ( m_pData ) {
				*m_pData = 0;
				get_data()->m_nLength = 0;
			}
		} else {
			size_type n = TRAITS::length(s);
			prepare_buffer(n);
			TRAITS::copy(m_pData, s, n + 1);
		}
		return *this;
	}
	/*!
	The operator replaces the sequence controlled by <tt>*this</tt> with the
	operand <tt>c</tt> of type <tt>CHAR</tt>, then returns <tt>*this</tt>.
	*/
	string& operator = (CHAR c)
	{
		prepare_buffer(1);
		*m_pData = c;
		return *this;
	}

	/*!
	The operator appends the operand sequence to the end of the sequence controlled
	by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& operator += (const string & x)
	{
		int n = x.size();
		if ( n ) {
			_append(x.c_str(), n);
		}
		return *this;
	}
	/*!
	The operator appends the operand sequence to the end of the sequence controlled
	by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& operator += (const_pointer s)
	{
		if ( s ) {
			int n = TRAITS::length(s);
			if ( n ) {
				_append(s, n);
			}
		}
		return *this;
	}
	/*!
	The operator appends the operand <tt>c</tt> of type <tt>CHAR</tt> to the end of
	the sequence controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& operator += (CHAR c)
	{
		_append(&c, 1);
		return *this;
	}

	/*!
	The member function inserts a sequence controlled by <tt>x</tt> before position
	<tt>pos</tt> in the controlled sequence.<br>
	The function that returns a value of <tt>*this</tt>.
	*/
	string& insert(size_type pos, const string& x)
	{
		size_type n = x.size();
		if ( n ) {
			_insert(pos, n);
			TRAITS::copy(m_pData + pos, x.c_str(), n);
		}
		return *this;
	}

	/*!
	The member function inserts a sequence from <tt>const_pointer s</tt> before position
	<tt>pos</tt> in the controlled sequence.<br>
	The function that returns a value of <tt>*this</tt>.
	*/
	string& insert(size_type pos, const_pointer s)
	{
		size_type n = TRAITS::length(s);
		if ( n ) {
			_insert(pos, n);
			TRAITS::copy(m_pData + pos, s, n);
		}
		return *this;
	}

	/*!
	The member function removes up to <tt>n</tt> elements of the controlled
	sequence beginning at position <tt>pos</tt>, then returns <tt>*this</tt>.
	\note If <tt>n == npos</tt> then it means the whole controlled sequence.
	*/
	string& erase(size_type pos = 0, size_type n = npos)
	{
		_erase(pos, n);
		return *this;
	}

	/*!
	The member function replaces up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt>. The replacement is the operand sequence 
	specified by <tt>const_pointer s</tt>. The function then returns <tt>*this</tt>.
	*/
	string& replace(size_type pos, size_type n, const_pointer s)
	{
		SCL_ASSERT(pos >= 0 && n >= 0 && size() >= pos + n);
		int sn = TRAITS::length(s);
		_replace(pos, n, sn);
		TRAITS::copy(m_pData + pos, s, sn);
		return *this;
	}

	/*!
	The member operator returns a reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	reference operator [] (unsigned int pos)
	{
		SCL_ASSERT(m_pData);
		SCL_ASSERT(pos >= 0 && pos < size());
		return m_pData[pos];
	}

	/*!
	The member operator returns a const reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	const_reference operator [] (unsigned int pos) const
	{
		SCL_ASSERT(m_pData);
		SCL_ASSERT(pos >= 0 && pos < size());
		return m_pData[pos];
	}

	/*!
	The member operator returns a reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	reference operator [] (int pos)
	{
		SCL_ASSERT(m_pData);
		SCL_ASSERT(pos >= 0 && pos < (int)size());
		return m_pData[pos];
	}

	/*!
	The member operator returns a const reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	const_reference operator [] (int pos) const
	{
		SCL_ASSERT(m_pData);
		SCL_ASSERT(pos >= 0 && pos < (int)size());
		return m_pData[pos];
	}

	/*!
	The member function returns a const pointer to a nonmodifiable <tt>C string</tt>
	constructed by adding a terminating <tt>null</tt> element to the controlled sequence.
	Calling any non-const member function for <tt>*this</tt> can invalidate the pointer.
	*/
	const_pointer c_str() const
	{
		return m_pData ? m_pData : nil;
	}

	/*!
	The member function returns the length of the controlled sequence (same as <tt>size()</tt>).
	*/
	size_type length() const
	{
		if ( !m_pData ) {
			return 0;
		}
		return get_data()->m_nLength;
	}
	/*!
	The member function returns the length of the controlled sequence.
	*/
	size_type size() const
	{
		return length();
	}

	SYSCORE_API void reserve(size_type n = 0);

	/*!
	The member function returns <tt>true</tt> for an empty controlled sequence,
	otherwise - <tt>false</tt>.
	*/
	bool empty() const
	{
		return length() == 0;
	}

	/*!
	The member function swaps the controlled sequences between <tt>*this</tt> and <tt>str</tt>.
	*/
	void swap(string& x)
	{
		SCL::swap(m_pData, x.m_pData);
	}

	SYSCORE_API size_type find(const_pointer s, size_type pos, size_type n) const;
	/*!
	The member function finds the first (lowest beginning position) subsequence
	in the controlled sequence, beginning on or after position <tt>pos</tt>,
	that matches the operand sequence specified by <tt>string x</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find(const string& x, size_type pos = 0) const
	{
		return find(x.c_str(), pos, x.size());
	}
	/*!
	The member function finds the first (lowest beginning position) subsequence
	in the controlled sequence, beginning on or after position <tt>pos</tt>,
	that matches the operand sequence specified by <tt>const_pointer s</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find(const_pointer s, size_type pos = 0) const
	{
		return find(s, pos, TRAITS::length(s));
	}
	/*!
	The member function finds the first (lowest beginning position) subsequence
	in the controlled sequence, beginning on or after position <tt>pos</tt>,
	that matches the symbol specified by <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find(CHAR c, size_type pos = 0) const
	{
		return find((const_pointer)&c, pos, 1);
	}

	SYSCORE_API size_type rfind(const_pointer s, size_type pos, size_type n) const;
	/*!
	The member function finds the last (highest beginning position) subsequence in
	the controlled sequence, beginning on or before position <tt>pos</tt>,
	that matches the operand sequence specified by <tt>string x</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	\note If <tt>pos == npos</tt> then it means the end of the controlled sequence.
	*/
	size_type rfind(const string& x, size_type pos = npos) const
	{
		return (rfind(x.c_str(), pos, x.size()));
	}
	/*!
	The member function finds the last (highest beginning position) subsequence in
	the controlled sequence, beginning on or before position <tt>pos</tt>,
	that matches the operand sequence specified by <tt>const_pointer s</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	\note If <tt>pos == npos</tt> then it means the end of the controlled sequence.
	*/
	size_type rfind(const_pointer s, size_type pos = npos) const
	{
		return (rfind(s, pos, TRAITS::length(s)));
	}
	/*!
	The member function finds the last (highest beginning position) subsequence in
	the controlled sequence, beginning on or before position <tt>pos</tt>,
	that matches the symbol specified by <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position where the matching subsequence begins.
	Otherwise, the function returns <tt>npos</tt>.
	\note If <tt>pos == npos</tt> then it means the end of the controlled sequence.
	*/
	size_type rfind(CHAR c, size_type pos = npos) const
	{
		return (rfind((const_pointer )&c, pos, 1));
	}

	/*!
	The member function compares the entire controlled sequence to the operand sequence
	specified by <tt>x</tt>. The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(const string& x) const
	{
		return _compare(c_str(), size(), x.c_str(), x.size());
	}
	/*!
	The member function compares up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt> to the operand sequence specified by <tt>x</tt>.
	The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(size_type pos, size_type n, const string& x) const
	{
		return _compare(c_str() + pos, n, x.c_str(), x.size());
	}
	/*!
	The member function compares up to <tt>n1</tt> elements of the controlled sequence
	beginning with position <tt>pos1</tt> to the up to <tt>n2</tt> elements of operand
	sequence specified by <tt>x</tt> with position <tt>pos2</tt>.
	The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(size_type pos1, size_type n1, const string& x, size_type pos2, size_type n2) const
	{
		return _compare(c_str() + pos1, n1, x.c_str() + pos2, n2);
	}
	/*!
	The member function compares the entire controlled sequence to the operand sequence
	specified by <tt>s</tt>. The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(const_pointer s) const
	{
		return _compare(c_str(), size(), s, TRAITS::length(s));
	}
	/*!
	The member function compares up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt> to the operand sequence specified by <tt>s</tt>.
	The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(size_type pos, size_type n, const_pointer s) const
	{
		return _compare(c_str() + pos, n, s, TRAITS::length(s));
	}
	/*!
	The member function compares up to <tt>n1</tt> elements of the controlled sequence
	beginning with position <tt>pos1</tt> to the up to <tt>n2</tt> elements of operand
	sequence specified by <tt>s</tt> with position <tt>pos2</tt>.
	The function returns:
	<ul>
		<li>A negative value if the first differing element in the controlled
		sequence compares less than the corresponding element in the operand sequence,
		or if the two have a common prefix but the operand sequence is longer.

		<li>Zero if the two compare equal element by element and are the same length.

		<li>A positive value otherwise. 
	</ul>
	*/
	int compare(size_type pos, size_type n1, const_pointer s, size_type n2) const
	{
		return _compare(c_str() + pos, n1, s, n2);
	}

	/*!
	The member function returns <tt>*(A *)this</tt>.
	*/
	A get_allocator() const
	{
		return *(A *)this;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	they are equal, otherwise - <tt>false</tt>.
	*/
	// CString-like interface
	bool operator == (const_pointer s) const
	{
		if ( !m_pData || !*m_pData ) {
			return !s || !*s;
		}
		if ( !s || !*s ) {
			return false;
		}
		return TRAITS::compare(c_str(), s) == 0;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	they are not equal, otherwise - <tt>false</tt>.
	*/
	bool operator != (const_pointer s) const
	{
		if ( !m_pData || !*m_pData ) {
			return s != NULL && *s != 0;
		}
		if ( !s || !*s ) {
			return true;
		}
		return TRAITS::compare(c_str(), s) != 0;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	<tt>this</tt> is lexicographically less than <tt>s</tt>, <tt>false</tt> otherwise.
	*/
	bool operator < (const_pointer s) const
	{
		if ( (!m_pData || !*m_pData) ) {
			return s != NULL && *s != 0;
		}
		if ( !s || !*s ) {
			return false;
		}
		return TRAITS::compare(c_str(), s) < 0;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	<tt>this</tt> is lexicographically less than or equal to <tt>s</tt>,
	<tt>false</tt> otherwise.
	*/
	bool operator <= (const_pointer s) const
	{
		if ( (!m_pData || !*m_pData) ) {
			return true;
		}
		if ( !s || !*s ) {
			return false;
		}
		return TRAITS::compare(c_str(), s) <= 0;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	<tt>this</tt> is lexicographically greater than <tt>s</tt>,	<tt>false</tt> otherwise.
	*/
	bool operator > (const_pointer s) const
	{
		if ( (!m_pData || !*m_pData) ) {
			return false;
		}
		if ( !s || !*s ) {
			return true;
		}
		return TRAITS::compare(c_str(), s) > 0;
	}

	/*!
	The member operator compares the controlled sequence with sequence
	specified by const pointer <tt>s</tt>. It returns <tt>true</tt> if
	<tt>this</tt> is lexicographically greater than or equal to <tt>s</tt>,
	<tt>false</tt> otherwise.
	*/
	bool operator >= (const_pointer s) const
	{
		if ( (!m_pData || !*m_pData) ) {
			return s == NULL || *s == 0;
		}
		if ( !s || !*s ) {
			return true;
		}
		return TRAITS::compare(c_str(), s) >= 0;
	}
	/*!
	The member operator acts like c_str().
	*/
	operator const_pointer () const
	{
		return c_str();
	}

	/*!
	The member function returns a pointer to the first element of the sequence
	\note The sequence must be non-empty.
	*/
	pointer get_buffer(size_type n)
	{
		SCL_ASSERT(n > 0);
		reserve(n);
		return m_pData;
	}

	/*!
	The member function cuts the controlled string up to a length of <tt>n</tt>.<br>
	If <tt>n == npos</tt> then the length is the value returned by <tt>length()</tt>.
	It adds a <tt>null</tt> symbol at the end of the sequence.
	*/
	void release_buffer(size_type n = npos)
	{
		if ( m_pData ) {
			if ( n == -1 ) {
				n = TRAITS::length(m_pData);
			}
			SCL_ASSERT(n >= 0);
			str_data * pData = get_data();
			pData->m_nLength = n;
			m_pData[n] = 0;
		}
	}

	/*!
	The memmber function formats the controlled sequence using format pattern
	specified by <tt>fmt</tt>.
	\note The rules for format pattern is the same as for the <tt>vprintf()</tt>.
	*/
	void format(const_pointer fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		for ( int max_size = 64; ; max_size *= 2 ) {
			reserve(max_size);
			if ( TRAITS::vprintf(m_pData, max_size, fmt, vl) >= 0 ) {
				break;
			}
		}
		release_buffer();
	}
};

/*!
The template class describes an object that controls a varying-length sequence of
elements of type <tt><b>CHAR</b></tt> and extends basic_string with all its
public functions and properties.

\see basic_string
*/
template<class CHAR, class TRAITS = char_traits<CHAR>, class A = allocator<CHAR> >
class basic_string2 : public basic_string<CHAR, TRAITS, A>
{
public:
	/*!
	The typedef provides conventional name for common <tt>string</tt> type.
	*/
	typedef basic_string<CHAR, TRAITS, A> string;
	/*!
	The unsigned integer type describes an object that can represent the length
	of any controlled sequence.
	*/
	typedef typename A::size_type size_type;
	/*!
	The signed integer type describes an object that can represent the difference
	between the addresses of any two elements in the controlled sequence.
	*/
	typedef typename A::difference_type difference_type;
	/*!
	The type describes an object that can serve as a pointer to an element of 
	the controlled sequence.
	*/
	typedef typename A::pointer pointer;
	/*!
	The type describes an object that can serve as a constant pointer to
	an element of the controlled sequence.
	*/
	typedef typename A::const_pointer const_pointer;
	/*!
	The type describes an object that can serve as a reference to an 
	element of the controlled sequence.
	*/
	typedef typename A::reference reference;
	/*!
	The type describes an object that can serve as a constant reference to an
	element of the controlled sequence.
	*/
	typedef typename A::const_reference const_reference;
	/*!
	The type is a synonym for <tt>A::value_type</tt>.
	*/	
	typedef typename A::value_type value_type;
	/*!
	The type describes an object that can serve as a random-access iterator for
	the controlled sequence. It is described here as a synonym for the <tt>A::pointer</tt>.
	*/	
	typedef typename A::pointer iterator;
	/*!
	The type describes an object that can serve as a constant random-access iterator for
	the controlled sequence. It is described here as a synonym for the <tt>A::const_pointer</tt>.
	*/	
	typedef typename A::const_pointer const_iterator;

	/*!
	The type is a synonym for the template parameter <tt>TRAITS</tt>.
	*/
	typedef TRAITS traits_type;
	/*!
	The type is a synonym for the template parameter <tt>A</tt>.
	*/
	typedef A allocator_type;
	/*!
	The typedef provides conventional name for common <tt>string2</tt> type.
	*/
	typedef basic_string2<CHAR, TRAITS, A> string2;

public:
	/*!
	\see basic_string()
	*/
	basic_string2()
		: basic_string<CHAR, TRAITS, A>()
	{
	}

	/*!
	\see basic_string(const string & x)
	*/
	basic_string2(const string & x)
		: basic_string<CHAR, TRAITS, A>(x)
	{
	}

	/*!
	\see basic_string(const_pointer s)
	*/
	basic_string2(const_pointer s)
		: basic_string<CHAR, TRAITS, A>(s)
	{
	}

	/*!
	\see basic_string(const string & x, size_type pos, size_type n)
	*/
	basic_string2(const string & x, size_type pos, size_type n)
		: basic_string<CHAR, TRAITS, A>(x, pos, n)
	{
	}

	/*!
	\see basic_string(const_pointer s, size_type n)
	*/
	basic_string2(const_pointer s, size_type n)
		: basic_string<CHAR, TRAITS, A>(s, n)
	{
	}

	/*!
	\see basic_string(size_type n, CHAR c)
	*/
	basic_string2(size_type n, CHAR c)
		: basic_string<CHAR, TRAITS, A>(n, c)
	{
	}

	/*!
	\see basic_string(const_iterator first, const_iterator last)
	*/
	basic_string2(const_iterator first, const_iterator last)
		: basic_string<CHAR, TRAITS, A>(first, last)
	{
	}

	string2& operator = (CHAR c)
	{
		string::prepare_buffer(1);
		*(string::m_pData) = c;
		return *this;
	}

	/*!
	The member function returns a pointer to the first element of the sequence
	(or, for an empty sequence, a non-null pointer that cannot be dereferenced).
	*/
	const_pointer data() const
	{
		return string::m_pData;
	}

	/*!
	The member function returns an object whose controlled sequence is a copy
	of up to <tt>n</tt> elements of the controlled sequence beginning at position <tt>pos</tt>.
	\note If <tt>pos == npos</tt> then the function will copy all elements from <tt>pos</tt> till
	the end of the sequence.
	*/
	string substr(size_type pos = 0, size_type n = string::npos) const
	{
		return string(*this, pos, n);
	}

	/*!
	The member function appends the operand sequence to the end of the sequence
	controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(const string & x)
	{
		int n = x.size();
		if ( n ) {
			_append(x.c_str(), n);
		}
		return *this;
	}
	/*!
	The member function appends up to <tt>n</tt> elements of the operand sequence
	to the end of the sequence controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(const_pointer s, size_type n)
	{
		SCL_ASSERT(n >= 0);
		if ( n ) {
			size_type n1 = TRAITS::length(s);
			if ( n1 ) {
				if ( n > n1 ) {
					n = n1;
				}
				_append(s, n);
			}
		}
		return *this;
	}
	/*!
	The member function appends up to <tt>n</tt> elements of the operand sequence
	beginning from position <tt>pos</tt> to the end of the sequence controlled by
	<tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(const string & x, size_type pos, size_type n)
	{
		SCL_ASSERT(pos >= 0 && n >= 0);
		if ( n ) {
			size_type n1 = x.size() - pos;
			if ( n1 > 0 ) {
				if ( n > n1 ) {
					n = n1;
				}
				_append(x.c_str() + pos, n);
			}
		}
		return *this;
	}
	/*!
	The member function appends the operand sequence to the end of the sequence
	controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(const_pointer s)
	{
		if ( s ) {
			int n = TRAITS::length(s);
			if ( n ) {
				_append(s, n);
			}
		}
		return *this;
	}
	/*!
	The member function appends <tt>n</tt> elements of <tt>CHAR c</tt>
	to the end of the sequence controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(size_type n, CHAR c)
	{
		int n1 = string::size();
		_append(NULL, n);
		TRAITS::assign(string::m_pData + n1, n, c);
		return *this;
	}
	/*!
	The member function appends the operand sequence from the range <tt>[first, last)</tt>
	to the end of the sequence controlled by <tt>*this</tt>, then returns <tt>*this</tt>.
	*/
	string& append(iterator first, iterator last)
	{
		SCL_ASSERT(first);
		return append(first, last - first);
	}

	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with the
	operand sequence, then returns <tt>*this</tt>.
	*/
	string& assign(const string& x)
	{
		if ( &x != this ) {
			if ( x.m_pData ) {
				++x.get_data()->m_nRefCount;
			}
			string::dealloc_buffer(string::get_data());
			string::m_pData = x.m_pData;
		}
		return *this;
	}
	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with the
	operand sequence which begins from position <tt>pos</tt> and has length <tt>n</tt>,
	then returns <tt>*this</tt>.
	*/
	string& assign(const string & x, size_type pos, size_type n)
	{
		SCL_ASSERT(pos >= 0 && n >= 0);
		size_type n1 = x.size() - pos;
		if ( n1 > 0 ) {
			if ( n > n1 ) {
				n = n1;
			}
		}
		if ( n ) {
			prepare_buffer(n);
			TRAITS::copy(string::m_pData, x.c_str() + pos, n);
		} else {
			if ( string::m_pData ) {
				*(string::m_pData) = 0;
			}
		}
		return *this;
	}
	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with the
	operand sequence which has length <tt>n</tt>, then returns <tt>*this</tt>.
	*/
	string& assign(const_pointer s, size_type n)
	{
		if ( !s ) {
			if ( string::m_pData ) {
				*string::m_pData = 0;
			}
		} else {
			size_type n1 = TRAITS::length(s);
			if ( n1 ) {
				if ( n > n1 ) {
					n = n1;
				}
				prepare_buffer(n);
				TRAITS::copy(string::m_pData, s, n);
			}
		}
		return *this;
	}
	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with the
	operand sequence, then returns <tt>*this</tt>.
	*/
	string& assign(const_pointer s)
	{
		if ( !s ) {
			if ( string::m_pData ) {
				*string::m_pData = 0;
			}
		} else {
			size_type n = TRAITS::length(s);
			prepare_buffer(n);
			TRAITS::copy(string::m_pData, s, n);
		}
		return *this;
	}
	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with
	<tt>n</tt> elements of <tt>CHAR c</tt>, then returns <tt>*this</tt>.
	*/
	string& assign(size_type n, CHAR c)
	{
		prepare_buffer(n);
		TRAITS::assign(string::m_pData, n, c);
		return *this;
	}
	/*!
	The member function replaces the sequence controlled by <tt>*this</tt> with the
	operand sequence in the range <tt>[first, last)</tt>, then returns <tt>*this</tt>.
	*/
	string& assign(const_iterator first, const_iterator last)
	{
		return assign(first, last - first);
	}

	/*!
	The member function ensures that <tt>size()</tt> henceforth returns <tt>n</tt>.
	If it must lengthen the controlled sequence, it appends elements with value <tt>c</tt>.
	*/
	void resize(size_type n, CHAR c = CHAR())
	{
		if ( n < string::size() ) {
			_erase(n);
		} else {
			append(n - string::size(), c);
		}
	}

	/*!
	The member function inserts before position <tt>tpos</tt> in the controlled sequence
	the operand sequence which begins from position <tt>pos</tt> and has length <tt>n</tt>.
	A function then returns <tt>*this</tt>.
	*/
	string& insert(size_type tpos, const string& x, size_type pos, size_type n)
	{
		SCL_ASSERT(tpos >= 0 && pos >= 0 && n >= 0);
		size_type n1 = x.size() - pos;
		if ( n1 > 0 ) {
			if ( n > n1 ) {
				n = n1;
			}
			if ( n ) {
				_insert(tpos, n);
				TRAITS::copy(string::m_pData + tpos, x.c_str() + pos, n);
			}
		}
		return *this;
	}
	/*!
	The member function inserts before position <tt>pos</tt> in the controlled sequence
	the operand sequence which has length <tt>n</tt>.
	A function then returns <tt>*this</tt>.
	*/
	string& insert(size_type pos, const_pointer s, size_type n)
	{
		SCL_ASSERT(pos >= 0 && n >= 0);
		size_type n1 = TRAITS::length(s);
		if ( n1 > 0 ) {
			if ( n > n1 ) {
				n = n1;
			}
			if ( n ) {
				_insert(pos, n);
				TRAITS::copy(string::m_pData + pos, s, n);
			}
		}
		return *this;
	}
	/*!
	The member function inserts before position <tt>pos</tt> in the controlled sequence
	the operand sequence which has length <tt>n</tt> and consits of elements of <tt>CHAR c</tt>.
	A function then returns <tt>*this</tt>.
	*/
	string& insert(size_type pos, size_type n, CHAR c)
	{
		if ( n ) {
			_insert(pos, n);
			TRAITS::assign(string::m_pData + pos, n, c);
		}
		return *this;
	}
	/*!
	The member function inserts before the element pointed to by <tt>i</tt>
	in the controlled sequence elements of <tt>CHAR c</tt>.
	A function then returns <tt>*this</tt>.
	*/
	iterator insert(iterator i, CHAR c)
	{
		size_type pos = i - string::m_pData;
		if ( !i ) {
			SCL_ASSERT(!string::m_pData);
			string::m_pData = string::alloc_buffer(1);
		} else {
			SCL_ASSERT(string::m_pData);
			_insert(pos, 1);
		}
		string::m_pData[pos] = c;
		return string::m_pData + pos;
	}
	/*!
	The member function inserts before the element pointed to by <tt>i</tt> in the
	controlled sequence the operand sequence which has length <tt>n</tt> and consits
	of elements of <tt>CHAR c</tt>.
	A function then returns <tt>*this</tt>.
	*/
	void insert(iterator i, size_type n, CHAR c)
	{
		size_type pos = i - string::m_pData;
		if ( !i ) {
			SCL_ASSERT(!string::m_pData);
			string::m_pData = alloc_buffer(n);
		} else {
			SCL_ASSERT(string::m_pData);
			_insert(pos, n);
		}
		TRAITS::assign(string::m_pData + pos, n, c);
	}
	/*!
	The member function inserts before the element pointed to by <tt>i</tt> in the
	controlled sequence the operand sequence from range <tt>[first, last)</tt>.
	A function then returns <tt>*this</tt>.
	*/
	void insert(iterator i, iterator first, iterator last)
	{
		size_type pos = i - string::m_pData, n = last - first;
		if ( !i ) {
			SCL_ASSERT(!string::m_pData);
			string::m_pData = alloc_buffer(n);
		} else {
			SCL_ASSERT(string::m_pData);
			_insert(pos, n);
		}
		TRAITS::copy(string::m_pData + pos, first, n);
	}

	/*!
	The member function removes the element of the controlled sequence pointed
	to by <tt>pos</tt> and then returns an iterator that designates the first
	element remaining beyond any elements removed, or <tt>end()</tt> if no such element exists.
	*/
	iterator erase(iterator pos)
	{
		if ( !string::m_pData ) {
			return NULL;
		}
		_erase(pos - string::m_pData);
		return pos;
	}
	/*!
	The member function removes the elements of the controlled sequence in the range
	<tt>[first, last)</tt>.
	*/
	iterator erase(iterator first, iterator last)
	{
		if ( !string::m_pData ) {
			return NULL;
		}
		_erase(first - string::m_pData, last - first);
		return first;
	}

	/*!
	The member function replaces up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt>. The replacement is the operand sequence from <tt>s</tt>
	which has length <tt>sn</tt>. The function then returns *this.
	*/
	string& replace(size_type pos, size_type n, const_pointer s, size_type sn)
	{
		SCL_ASSERT(pos >= 0 && n >= 0 && this->size() >= pos + n);
		_replace(pos, n, sn);
		TRAITS::copy(string::m_pData + pos, s, sn);
		return *this;
	}
	/*!
	The member function replaces up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt>. The replacement is the operand sequence from
	<tt>x</tt> which begins from position <tt>pos</tt> and has length <tt>sn</tt>.
	The function then returns *this.
	*/
	string& replace(size_type pos, size_type n, const string& x, size_type xpos, size_type xn)
	{
		SCL_ASSERT(pos >= 0 && n >= 0 && this->size() >= pos + n);
		SCL_ASSERT(xpos >= 0 && xn >= 0 && x.size() >= pos + n);
		_replace(pos, n, xn);
		TRAITS::copy(string::m_pData + pos, x.c_str() + xpos, xn);
		return *this;
	}
	/*!
	The member function replaces up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt>. The replacement is the operand sequence from
	<tt>x</tt>. The function then returns *this.
	*/
	string& replace(size_type pos, size_type n, const string& x)
	{
		SCL_ASSERT(pos >= 0 && n >= 0 && this->size() >= pos + n);
		int xn = x.size();
		_replace(pos, n, xn);
		TRAITS::copy(string::m_pData + pos, x.c_str(), xn);
		return *this;
	}
	/*!
	The member function replaces up to <tt>n</tt> elements of the controlled sequence
	beginning with position <tt>pos</tt>. The replacement is the operand sequence which
	consists of <tt>cm</tt> elements of <tt>CHAR c</tt>. The function then returns *this.
	*/
	string& replace(size_type pos, size_type n, size_type cm, CHAR c)
	{
		SCL_ASSERT(pos >= 0 && n >= 0 && this->size() >= pos + n);
		_replace(pos, n, cm);
		TRAITS::assign(string::m_pData + pos, cm, c);
		return *this;
	}
	/*!
	The member function replaces elements of the controlled sequence
	beginning with the one pointed to by <tt>first</tt> up to but not including
	<tt>last</tt>. The replacement is the operand sequence from
	<tt>x</tt>. The function then returns *this.
	*/
	string& replace(iterator first, iterator last, const string& x)
	{
		return replace(first - string::m_pData, last - first, x);
	}
	/*!
	The member function replaces elements of the controlled sequence
	beginning with the one pointed to by <tt>first</tt> up to but not including
	<tt>last</tt>. The replacement is the operand sequence from
	<tt>s</tt> which has length <tt>n</tt>. The function then returns *this.
	*/
	string& replace(iterator first, iterator last, const_pointer s, size_type n)
	{
		return replace(first - string::m_pData, last - first, s, n);
	}
	/*!
	The member function replaces elements of the controlled sequence
	beginning with the one pointed to by <tt>first</tt> up to but not including
	<tt>last</tt>. The replacement is the operand sequence from
	<tt>s</tt>. The function then returns *this.
	*/
	string& replace(iterator first, iterator last, const_pointer s)
	{
		return replace(first - string::m_pData, last - first, s);
	}
	/*!
	The member function replaces elements of the controlled sequence
	beginning with the one pointed to by <tt>first</tt> up to but not including
	<tt>last</tt>. The replacement is the <tt>n</tt> elements of <tt>CHAR c</tt>.
	The function then returns *this.
	*/
	string& replace(iterator first, iterator last, size_type n, CHAR c)
	{
		return replace(first - string::m_pData, last - first, n, c);
	}
	/*!
	The member function replaces elements of the controlled sequence
	beginning with the one pointed to by <tt>first</tt> up to but not including
	<tt>last</tt>. The replacement is the operand sequence from
	element pointed to by <tt>f</tt> up to but not including <tt>l</tt>.
	The function then returns *this.
	*/
	string& replace(iterator first, iterator last, const_iterator f, const_iterator l)
	{
		return replace(first - string::m_pData, last - first, f, l - f);
	}

	/*!
	The member function returns an iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	iterator begin()
	{
		return string::m_pData;
	}
	/*!
	The member function returns a constant iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	const_iterator begin() const
	{
		return string::m_pData;
	}
	/*!
	The member function returns an iterator that points just beyond
	the end of the sequence.
	*/
	iterator end()
	{
		return string::m_pData + string::size();
	}
	/*!
	The member function returns a constant iterator that points just beyond
	the end of the sequence.
	*/
	const_iterator end() const
	{
		return string::m_pData + string::size();
	}
	/*!
	The member function returns an iterator that points just beyond the
	end of the controlled sequence. Hence, it designates the beginning of the
	reverse sequence.
	*/
	iterator rbegin()
	{
		return string::m_pData ? string::m_pData + string::get_data()->m_nLength - 1 : NULL;
	}
	/*!
	The member function returns a constant iterator that points just beyond the
	end of the controlled sequence. Hence, it designates the beginning of the
	reverse sequence.
	*/
	const_iterator rbegin() const
	{
		return string::m_pData ? string::m_pData + string::get_data()->m_nLength - 1 : NULL;
	}
	/*!
	The member function returns an iterator that points at the first element
	of the sequence (or just beyond the end of an empty sequence).
	Hence, it designates the end of the reverse sequence.
	*/
	iterator rend()
	{
		return string::m_pData ? string::m_pData - 1 : NULL;
	}
	/*!
	The member function returns a constant iterator that points at the first element
	of the sequence (or just beyond the end of an empty sequence).
	Hence, it designates the end of the reverse sequence.
	*/
	const_iterator rend() const
	{
		return string::m_pData ? string::m_pData - 1 : NULL;
	}

	/*!
	The member function returns a reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	reference at(size_type pos)
	{
		SCL_ASSERT(string::m_pData);
		SCL_ASSERT(pos >= 0 && pos < this->size());
		return string::m_pData[pos];
	}

	/*!
	The member function returns a constant reference to the element of the controlled sequence
	at position <tt>pos</tt>. If that position is invalid it performs assertion failed.
	*/
	const_reference at(size_type pos) const
	{
		SCL_ASSERT(this->m_pData);
		SCL_ASSERT(pos >= 0 && pos < string::size());
		return string::m_pData[pos];
	}

	/*!
	The member function returns the length of the longest sequence that the object can control.
	*/
	size_type max_size() const
	{
		size_type n = A::max_size();
		return (n <= 2 ? 1 : n - 2);
	}

	/*!
	The member function returns the storage currently allocated to hold the controlled
	sequence, a value at least as large as <tt>size()</tt>.
	*/
	size_type capacity() const
	{
		return string::m_pData ? string::get_data()->m_nAllocated : 0;
	}

	/*!
	The member function copies up to <tt>n</tt> elements from the controlled sequence,
	beginning at position <tt>pos</tt>, to the array beginning at <tt>s</tt>.
	It returns the number of elements actually copied.
	*/
	size_type copy(pointer s, size_type n, size_type pos = 0) const
	{
		if ( !string::m_pData ) {
			return 0;
		}
		SCL_ASSERT(s);
		SCL_ASSERT(n >= 0);
		int len = string::size();
		if ( n > len - pos ) {
			n = len - pos;
		}
		TRAITS::copy(s, string::m_pData + pos, n);
		return n;
	}

/*	friend void swap(string& x, string& y)
	{
		SCL::swap(x.m_pData, y.m_pData);
	}*/

	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches any of the elements in
	the operand sequence by <tt>s</tt> in the range <tt>[0, n)</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_of(const_pointer s, size_type pos, size_type n) const
	{
		size_type len = string::size();
		if ( n > 0 && pos < len ) {
			const_pointer end = string::m_pData + len;
			for ( const_pointer p = string::m_pData + pos; p < end; ++p ) {
				if ( TRAITS::find(s, n, *p) != 0 ) {
					return p - string::m_pData;
				}
			}
		}
		return string::npos;
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches any of the elements in
	the operand sequence specified by <tt>x</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_of(const string& x, size_type pos = 0) const
	{
		return find_first_of(x.c_str(), pos, x.size());
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches any of the elements in
	the operand sequence specified by <tt>s</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_of(const_pointer s, size_type pos = 0) const
	{
		return find_first_of(s, pos, TRAITS::length(s));
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches the elements specified
	by <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_of(CHAR c, size_type pos = 0) const
	{
		return find((pointer)&c, pos, 1);
	}

	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches any of the elements in
	the operand sequence specified by <tt>s</tt> in the range <tt>[0, n)</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_of(const_pointer s, size_type pos, size_type n) const
	{
		size_type len = string::size();
		if ( pos == string::npos ) {
			pos = len - 1;
		}
		SCL_ASSERT(pos >= 0);
		if ( n > 0 && pos < len ) {
			const_pointer p = string::m_pData + (pos < len ? pos : len - 1);
			for ( ; p >= string::m_pData; --p) {
				if (TRAITS::find(s, n, *p) != 0) {
					return p - string::m_pData;
				}
			}
		}
		return string::npos;
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches any of the elements in
	the operand sequence specified by <tt>x</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_of(const string& x, size_type pos = string::npos) const
	{
		return find_last_of(x.c_str(), pos, x.size());
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches any of the elements in
	the operand sequence specified by <tt>s</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_of(const_pointer s, size_type pos = string::npos) const
	{
		return find_last_of(s, pos, TRAITS::length(s));
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches any of the elements specified
	by <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_of(CHAR c, size_type pos = string::npos) const
	{
		return find_last_of((pointer)&c, pos, 1);
	}

	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches none of the elements in
	the operand sequence specified by <tt>s</tt> in the range <tt>[0, n)</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_not_of(const_pointer s, size_type pos, size_type n) const
	{
		size_type len = string::size();
		if ( pos < len ) {
			const_pointer end = string::m_pData + len;
			for ( const_pointer p = string::m_pData + pos; p != end; ++p ) {
				if ( TRAITS::find(s, n, *p) == 0 ) {
					return p - string::m_pData;
				}
			}
		}
		return string::npos;
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches none of the elements in
	the operand sequence specified by <tt>x</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_not_of(const string& x, size_type pos = 0) const
	{
		return find_first_not_of(x.c_str(), pos, x.size());
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches none of the elements in
	the operand sequence specified by <tt>s</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_not_of(const_pointer s, size_type pos = 0) const
	{
		return find_first_not_of(s, pos, TRAITS::length(s));
	}
	/*!
	The member function finds the first (lowest position) element of the controlled
	sequence, at or after position <tt>pos</tt>, that matches none of the elements
	of <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_first_not_of(CHAR c, size_type pos = 0) const
	{
		return find_first_not_of((pointer)&c, pos, 1);
	}

	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches none of the elements
	in the operand sequence specified by <tt>s</tt> in the range <tt>[0, n)</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_not_of(const_pointer s, size_type pos, size_type n) const
	{
		size_type len = string::size();
		if ( pos == string::npos ) {
			pos = len - 1;
		}
		SCL_ASSERT(pos >= 0);
		if ( n >= 0 && pos < len ) {
			const_pointer p = string::m_pData + (pos < len ? pos : len - 1);
			for ( ; p >= string::m_pData; --p ) {
				if ( TRAITS::find(s, n, *p) == 0 ) {
					return p - string::m_pData;
				}
			}
		}
		return (string::npos);
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches none of the elements
	in the operand sequence specified by <tt>x</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_not_of(const string& x, size_type pos = string::npos) const
	{
		return find_last_not_of(x.c_str(), pos, x.size());
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches none of the elements
	in the operand sequence specified by <tt>s</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_not_of(const_pointer s, size_type pos = string::npos) const
	{
		return find_last_not_of(s, pos, TRAITS::length(s));
	}
	/*!
	The member function finds the last (highest position) element of the controlled
	sequence, at or before position <tt>pos</tt>, that matches none of the elements
	of <tt>CHAR c</tt>.<br>
	If it succeeds, it returns the position. Otherwise, the function returns <tt>npos</tt>.
	*/
	size_type find_last_not_of(CHAR c, size_type pos = string::npos) const
	{
		return find_last_not_of((pointer)&c, pos, 1);
	}
};

template<class E, class T, class A>
basic_string<E, T, A> operator + (const basic_string<E, T, A> & lstr, const basic_string<E, T, A> & rstr)
{
	return (basic_string<E, T, A>(lstr) += rstr);
}

template<class E, class T, class A>
basic_string<E, T, A> operator + (const basic_string<E, T, A> & lstr, const E * rstr)
{
	return (basic_string<E, T, A>(lstr) += rstr);
}

template<class E, class T, class A>
basic_string<E, T, A> operator + (const basic_string<E, T, A> & lstr, E rstr)
{
	return (basic_string<E, T, A>(lstr) += rstr);
}

template<class E, class T, class A>
basic_string<E, T, A> operator + (const E * lstr, const basic_string<E, T, A> & rstr)
{
	return (basic_string<E, T, A>(lstr) += rstr);
}

template<class E, class T, class A>
basic_string<E, T, A> operator + (E lstr, const basic_string<E, T, A> & rstr)
{
	return (basic_string<E, T, A>(lstr) += rstr);
}

template<class CHAR, class TRAITS, class A>
CHAR basic_string<CHAR, TRAITS, A>::nil[1] = {0};

typedef basic_string<char> string;
typedef basic_string2<char> string2;

template <>
struct _hash_comp<string> : public hash_comp<string>
{
	unsigned operator () (const string & x) const
	{
		unsigned sum = 0;
		const int LARGENUMBER = 821603;
		unsigned multiple = LARGENUMBER;

		int index = 1;

		for( const char * s = x.c_str(); *s; ++s )	{
			sum += multiple * (index++) * (*s);
			multiple *= LARGENUMBER;
		}
		return sum;
	}

	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const string & x1, const string & x2, PRED pr) const
	{
		return (hv1 == hv2) && !strcmp(x1.c_str(), x2.c_str());
	}
};

} // namespace SCL

#endif // __SYS_STRING_H__

//
// End of file

