/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_BITSET_H__
#define __SCL_BITSET_H__

namespace SCL
{
/*!

The template class describes an object that stores a sequence of <b>K</b> bits indexed
from \b 0 through \b K-1, where \b K is known at compile time.<br>
A class <b>bitset<K></b> is an array of K bits. A bitset differs from a class
 SCL::vector &lt;bool> by being of
fixed size, from class SCL::set by having its bits indexed by integers rather
than associatively by value, and from both SCL::vector &ltbool> and SCL::set by providing operations
to manipulate the bits.
<p> 
A bit is <b>set</b> if its value is 1, <b>reset</b> if its value is 0.
To <b>flip</b> a bit is to change its value from 1 to 0 or from 0 to 1.<br>
When converting between an object of <tt>class bitset<K></tt> and an object of some integral type,
bit position <tt>j</tt> corresponds to the bit value <tt>1 << j</tt>.<br>
The integral value corresponding to two or more bits is the sum of their bit values.

*/
template <size_t K>
class bitset
{
public:
	/*!

		<b>_bitset</b> is the same as <b>bitset<K></b>

	*/
    typedef bitset<K> _bitset;
	/*! 
	The type is a synonym for bool.
	*/
    typedef bool element_type;

protected:
	unsigned long m_Data[(K+8*sizeof(unsigned long)-1)/(8*sizeof(unsigned long))];

    bitset(bool, bool)
	{
	}

public:
	/*!
	Resets all bits in the bit sequence.
	*/
    bitset()
	{
		memset(m_Data, 0, sizeof(m_Data));
	}
	/*!
	Specifies a copy of the sequence controlled by x.
	*/
    bitset(const _bitset & x)
	{
		memcpy(m_Data, x.m_Data, sizeof(m_Data));
	}

	/*!
	Sets only those bits at position <tt>j</tt> for which <tt>val & 1 << j</tt> is nonzero.
	*/
	bitset(unsigned long val)
	{
		*m_Data = val;
	}

	/*!
	Determines the initial bit values from elements of a string determined from str.
	If <tt>pos < 0</tt> or <tt>str.size() < pos</tt>, the constructor will produce assertion failed.
	Otherwise, the effective length of the string <tt>rlen</tt> is the smaller of
	<tt>n</tt> and <tt>str.size() - pos</tt>.
	If any of the <tt>rlen</tt> elements beginning at position <tt>pos</tt> is other than <tt>0</tt> or <tt>1</tt>,
	the constructor will produce assertion failed.
	Otherwise, the constructor sets only those bits at position <tt>j</tt> for which the element at
	position <tt>pos + j</tt> is <tt>1</tt>.
	\sa \ref s1
	*/
	explicit bitset(const string &str, string::size_type pos = 0, string::size_type n = string::npos)
	{
		SCL_ASSERT(pos >= 0 && pos < str.size());

		if ( n == string::npos ) {
			n = str.size() - pos;
		}
		SCL_ASSERT(n >= 0 && n <= str.size() - pos);

		memset(m_Data, 0, sizeof(m_Data));
		for ( string::size_type i = 0, j = pos + n - 1; i < n && i < K; i++, j--)
		{
			switch( str[(int)j] )
			{
			case string::value_type('0'):
				set(i, false);
				break;

			case string::value_type('1'):
				set(i, true);
				break;

			default:
				SCL_ASSERT(FALSE);
				break;
			}
		}
	}

	/*!
	The member operator function replaces each element of the bit sequence stored
	in <tt>*this</tt> with the logical <tt>AND</tt> of its previous value and
	the corresponding bit in <tt>x</tt>. The function returns <tt>*this</tt>.
	*/
	_bitset & operator &= (const _bitset& x)
	{
		unsigned long * p1 = m_Data, * p2 = x.m_pData, * _end = m_Data + COUNTOF(m_Data);
		for ( ; p1 != _end; ++p1, ++p2 ) {
			*p1 &= *p2;
		}
		return *this;
	}
	/*!
	The member operator function replaces each element of the bit sequence stored
	in <tt>*this</tt> with the logical <tt>OR</tt> of its previous value and the
	corresponding bit in <tt>x</tt>. The function returns <tt>*this</tt>.
	*/
    _bitset & operator |= (const _bitset & x)
	{
		unsigned long * p1 = m_Data;
		const unsigned long * p2 = x.m_Data, * _end = m_Data + COUNTOF(m_Data);
		for ( ; p1 != _end; ++p1, ++p2 ) {
			*p1 |= *p2;
		}
		return *this;
	}
	/*!
	The member operator function replaces each element of the bit sequence stored
	in <tt>*this</tt> with the logical <tt>EXCLUSIVE OR</tt> of its previous value and
	the corresponding bit in <tt>x</tt>. The function returns <tt>*this</tt>.
	*/
    _bitset & operator ^= (const _bitset & x)
	{
		unsigned long * p1 = m_Data;
		const unsigned long * p2 = x.m_Data, * _end = m_Data + COUNTOF(m_Data);
		for ( ; p1 != _end; ++p1, ++p2 ) {
			*p1 ^= *p2;
		}
		return *this;
	}
	/*!
	The member operator function replaces each element of the bit sequence stored
	in <tt>*this</tt> with the element <tt>pos</tt> positions earlier in the sequence.
	If no such earlier element exists, the function clears the bit.
	The function returns <tt>*this</tt>.
	*/
    _bitset & operator <<= (const size_t pos)
	{
		if ( pos >= K ) {
			memset(m_Data, 0, sizeof(m_Data));
		} else {
			int i;
			for ( i = K - 1; i >= static_cast<int>(pos); --i ) {
				set(i, at(i - pos));
			}
			for ( ; i >= 0; i--) {
				set(i, false);
			}
		}
		return *this;
	}
	/*!
	The member function replaces each element of the bit sequence stored
	in <tt>*this</tt> with the element <tt>pos</tt> positions later in the sequence.
	If no such later element exists, the function clears the bit.
	The function returns <tt>*this</tt>.
	*/
    _bitset & operator >>= (const size_t  pos)
	{
		if ( pos >= K ) {
			memset(m_Data, 0, sizeof(m_Data));
		} else {
			size_t i;
			for ( i = 0; i < K - pos; ++i ) {
				set(i, at(i + pos));
			}
			for ( ; i < K; ++i ) {
				set(i, false);
			}
		}
		return *this;
	}
	/*!
	The first function resets all bits in the bit sequence, then returns <tt>*this</tt>.
	*/
    _bitset & set()
	{
		memset(m_Data, ~0, sizeof(m_Data));
		return *this;
	}
	/*!
	The member function will produce assertion fail if <tt>size() <= pos</tt>.
	Otherwise, it stores <tt>val</tt> in the bit at position <tt>pos</tt>,
	then returns <tt>*this</tt>.
	*/
    _bitset & set(size_t pos, bool val = true)
	{
		SCL_ASSERT(pos >= 0 && pos < K);
		unsigned mask = (1 << (pos & (8 * sizeof(unsigned) - 1)));
		pos /= 8 * sizeof(unsigned);
		if ( val ) {
			m_Data[pos] |= mask;
		} else {
			m_Data[pos] &= ~mask;
		}
		return *this;
	}
	/*!
	The first member function resets all bits in the bit sequence, then returns <tt>*this</tt>.
	*/
    _bitset & reset()
	{
		memset(m_Data, 0, sizeof(m_Data));
		return *this;
	}
	/*!
	The member function will produce assertion fail if <tt>size() <= pos</tt>.
	Otherwise, it resets the bit at position <tt>pos</tt>, then returns <tt>*this</tt>.
	*/
    _bitset & reset(size_t pos)
	{
		SCL_ASSERT(pos >= 0 && pos < K);
		unsigned mask = (1 << (pos & (8 * sizeof(unsigned) - 1)));
		pos /= 8 * sizeof(unsigned);
		m_Data[pos] &= ~mask;
		return *this;
	}
	/*!
	The member function flips all bits in the bit sequence, then returns <tt>*this</tt>.
	*/
    _bitset & flip()
	{
		unsigned long * p1 = m_Data, * _end = m_Data + COUNTOF(m_Data);
		for ( ; p1 != _end; ++p1 ) {
			*p1 ^= ~0;
		}
		return *this;
	}
	/*!
	The member function will produce assertion fail if <tt>size() <= pos</tt>.
	Otherwise, it flips the bit at position pos, then returns <tt>*this</tt>.
	*/
    _bitset & flip(size_t pos)
	{
		SCL_ASSERT(pos >= 0 && pos < K);
		unsigned mask = (1 << (pos & (8 * sizeof(unsigned) - 1)));
		pos /= 8 * sizeof(unsigned);
		m_Data[pos] ^= mask;
		return *this;
	}

	/*!
	The member function returns the value of the bit at position <tt>pos</tt>
	in the bit sequence. If <tt>size() <= pos</tt> then the function will
	produce assertion fail.
	\note In current realization this operator is the same as function SCL::bitset< K >::at().
	*/
    bool operator [] (size_t pos) const
	{
		SCL_ASSERT(pos >= 0 && pos < K);
		unsigned mask = (1 << (pos & (8 * sizeof(unsigned) - 1)));
		pos /= 8 * sizeof(unsigned);
		return (m_Data[pos] & mask) != 0;
	}

	/*!
	The member function returns the value of the bit at position <tt>pos</tt>
	in the bit sequence. If <tt>size() <= pos</tt> then the function will
	produce assertion fail.
	*/
    bool at(size_t pos) const
	{
		SCL_ASSERT(pos >= 0 && pos < K);
		unsigned mask = (1 << (pos & (8 * sizeof(unsigned) - 1)));
		pos /= 8 * sizeof(unsigned);
		return (m_Data[pos] & mask) != 0;
	}

	/*!
	The member function returns <tt>unsigned long</tt> number that is represented
	by the bit sequence. If number of the bit sequence is greater than number of bits
	in <tt>unsigned long</tt> the result may be incorrect.
	*/
	unsigned long to_ulong() const
	{
		return *m_Data;
	}

	/*!
	The member function constructs and returns an object of class SCL::basic_string.<br>
	For each bit in the bit sequence, the function appends <tt>1</tt> if the bit is set, otherwise <tt>0</tt>.
	*/
	string to_string() const
	{
		string retval;
		string::pointer data = retval.get_buffer(K);
		for ( size_t i = 0; i < K; ++i ) {
			data[K - i - 1] = string::value_type(at(i) ? '1' : '0');
		}
		retval.release_buffer(K);
		return retval;
	}

	/*!
	The member function returns the number of bits set in the bit sequence.
	*/
    size_t count() const
	{
		size_t retval = 0;
		for ( int i = 0; i < K; ++i ) {
			if ( at(i) ) {
				++retval;
			}
		}
		return retval;
	}

	/*!
	The member function returns <tt>K</tt>.
	*/
    size_t size() const
	{
		return K;
	}

	/*!
	The member operator function returns true only if the bit sequence stored in
	<tt>*this</tt> is the same as the one stored in <tt>x</tt>.
	*/
    bool operator == (const _bitset & x) const
	{
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			if ( m_Data[i] != x.m_Data[i] ) {
				return false;
			}
		}
		return true;
	}
	/*!
	The member operator function returns true only if the bit sequence stored 
	in <tt>*this</tt> differs from the one stored in <tt>x</tt>.
	*/
    bool operator != (const _bitset & x) const
	{
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			if ( m_Data[i] != x.m_Data[i] ) {
				return true;
			}
		}
		return false;
	}
	/*!
	The member function returns true only if the bit at position <tt>pos</tt> is set.<br>
	If <tt>size() <= pos</tt> then the function will produce assertion fail.
	*/
    bool test(size_t pos) const
	{
		return at(pos);
	}
	/*!
	The member function returns true if any bit is set in the bit sequence.
	*/
    bool any() const
	{
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			if ( m_Data[i] ) {
				return true;
			}
		}
		return false;
	}
	/*!
	The member function returns true if none of the bits are set in the bit sequence.
	*/
    bool none() const
	{
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			if ( m_Data[i] ) {
				return false;
			}
		}
		return true;
	}
	/*!
	The member function returns true if none of the bits are set in the bit sequence.
	*/
	bool empty() const
	{
		return none();
	}
	/*!
	The member operator function returns bitset(*this) <tt><<= pos</tt>.
	*/
    _bitset operator << (size_t pos) const
	{
		SCL_ASSERT(pos >= 0);
		_bitset retval;
		for ( size_t i = K - 1; i >= pos; --i ) {
			retval.set(i, at(i - pos));
		}
		return retval;
	}
	/*!
	The member operator function returns bitset(*this) <tt>>>= pos</tt>.
	*/
    _bitset operator >> (size_t pos) const
	{
		SCL_ASSERT(pos >= 0);
		_bitset retval;
		for ( size_t i = 0; i < K - pos; ++i ) {
			retval.set(i, at(i + pos));
		}
		return retval;
	}
	/*!
	The member operator function returns bitset(*this).flip().
	*/
    _bitset operator ~ () const
	{
		_bitset retval(false, false);
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			retval.m_Data[i] = ~m_Data[i];
		}
		return retval;
	}
	/*!
	The member operator function creates new bit sequence <tt>retval</tt>
	and replaces each element of it with the logical <tt>OR</tt> of
	self correspondent bit's current value and the corresponding bit in <tt>x</tt>.
	The function returns <tt>retval</tt>.
	*/
    _bitset operator | (const _bitset & x) const
	{
		_bitset retval(false, false);
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			retval.m_Data[i] = m_Data[i] | x.m_Data[i];
		}
		return retval;
	}
	/*!
	The member operator function creates new bit sequence <tt>retval</tt>
	and replaces each element of it with the logical <tt>AND</tt> of
	self correspondent bit's current value and the corresponding bit in <tt>x</tt>.
	The function returns <tt>retval</tt>.
	*/
    _bitset operator & (const _bitset & x) const
	{
		_bitset retval(false, false);
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			retval.m_Data[i] = m_Data[i] & x.m_Data[i];
		}
		return retval;
	}
	/*!
	The member operator function creates new bit sequence <tt>retval</tt>
	and replaces each element of it with the logical <tt>EXCLUSIVE OR </tt> of
	self correspondent bit's current value and the corresponding bit in <tt>x</tt>.
	The function returns <tt>retval</tt>.
	*/
    _bitset operator ^ (const _bitset & x) const
	{
		_bitset retval(false, false);
		for ( int i = 0; i < COUNTOF(m_Data); ++i ) {
			retval.m_Data[i] = m_Data[i] ^ x.m_Data[i];
		}
		return retval;
	}
};
/*!
\page Example1
\section s1 string constructor usage examples
<tt>
<table border=0>
<tr><td>bitset<10>b4("1010101010") ;</td><td> / / 1010101010</td></tr>
<tr><td>bitset<10>b5("10110111011110",4) ;</td><td> / / 0111011110</td></tr>
<tr><td>bitset<10>b6("10110111011110",2,8) ;</td><td> / / 0011011101</td></tr>
<tr><td>bitset<10> b7("n0g00d") ;</td><td> / / invalid_argument thrown</td></tr>
<tr><td>bitset<10> b8= "n0g00d";</td><td> / / error: no char* to bitset conversion</td></tr>
</table>
</tt>
*/

} // namespace SCL

#endif // __SCL_BITSET_H__

