/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_LOCK_H__
#define __SCL_LOCK_H__

namespace SCL
{

class Lock
{
protected:
	int & m_value;

public:
	Lock(int & a_value)
		: m_value(a_value)
	{
		SCL_ASSERT(m_value >= 0);
		++m_value;
	}

	~Lock()
	{
		--m_value;
		SCL_ASSERT(m_value >= 0);
	}
};

} // namespace SCL

#endif // __SCL_LOCK_H__

