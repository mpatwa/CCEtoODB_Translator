/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYS_SWAPPER_H__
#define __SYS_SWAPPER_H__

namespace SCL
{

/*!
The template function swaps two elements <tt>x</tt> and <tt>y</tt> of type <b>T</b>.
It uses a local variable to temporary element saving.
*/
template <class T>
void swap(T & x, T & y)
{
	T z = x;
	x = y;
	y = z;
}

template <class T>
struct Swapper : public T
{
public:
	Swapper(T & a_obj)
	{
		swap(a_obj);
	}
	Swapper & operator = (T & a_obj)
	{
		swap(a_obj);
	}
};

} // namespace SCL

#endif // __SYS_SWAPPER_H__

