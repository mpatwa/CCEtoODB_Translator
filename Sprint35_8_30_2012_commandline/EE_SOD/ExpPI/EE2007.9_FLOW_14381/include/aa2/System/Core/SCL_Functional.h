/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYS_FUNCTIONAL_H__
#define __SYS_FUNCTIONAL_H__

namespace SCL
{

/*!
The template class of type <b>T</b> defines its member function as returning
<tt>x < y</tt>. The member function defines a total ordering, even if <tt>T</tt> is an 
object pointer type.
*/
template <class T>
struct less
{
	bool operator () (const T & x1, const T & x2) const
	{
		return x1 < x2;
	}
};

/*!
The template class of type <b>T</b> defines its member function as returning
<tt>x > y</tt>. The member function defines a total ordering, even if <tt>T</tt> is an
object pointer type.
*/
template <class T>
struct greater
{
	bool operator () (const T & x1, const T & x2) const
	{
		return x1 > x2;
	}
};

/*!
The template class of type <b>T</b> defines its member function as returning
<tt>x <= y</tt>. The member function defines a total ordering, even if <tt>T</tt> is an 
object pointer type.
*/
template <class T>
struct less_equal
{
	bool operator () (const T & x1, const T & x2) const
	{
		return x1 <= x2;
	}
};

/*!
The template class of type <b>T</b> defines its member function as returning
<tt>x >= y</tt>. The member function defines a total ordering, even if <tt>T</tt> is an
object pointer type.
*/
template <class T>
struct greater_equal
{
	bool operator () (const T & x1, const T & x2) const
	{
		return x1 >= x2;
	}
};

} // namespace SCL

#endif // __SYS_FUNCTIONAL_H__

