/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_NULL_NOTIFIER_H__
#define __SCL_NULL_NOTIFIER_H__

namespace SCL
{

typedef void * handle;

class SYSCORE_API IHandleUpdate
{
public:
	virtual handle update(SCL::handle a_handle) = 0;
};

class Event;

namespace EventCode
{
	enum {
		Add			= 0,
		Delete		= 1000000,
		Change		= 1000002,
		Reload		= 1000003,
		Clear		= 1000004,
		Reorder		= 1000005,
		UpdateHandle= 1000006,
		User		= 1000007 // first one available for user's events
	};
}

struct NullNotifier
{
	void Notify(int message, void * param1 = NULL, void * param2 = NULL)
	{
	}

	void LockNotify(bool bLock)
	{
	}

	Event * get_event()
	{
		return NULL;
	}
};

} // namespace SCL

#endif // __SCL_NULL_NOTIFIER_H__

