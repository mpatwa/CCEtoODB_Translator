/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_EVENT_H__
#define __SCL_EVENT_H__

namespace SCL
{

class SYSCORE_API EventNodeBase
{
	friend class Event;
	friend class EventNode;

protected:
	EventNodeBase * m_pPrev, * m_pNext;

	void Remove()
	{
		OnRemoveNext();

		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
		m_pNext = m_pPrev = this;
	}

	void InsertBefore(EventNodeBase * pOther)
	{
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
		m_pPrev = pOther->m_pPrev;
		m_pNext = pOther;
		pOther->m_pPrev->m_pNext = this;
		pOther->m_pPrev = this;
	}

	void InsertAfter(EventNodeBase * pOther)
	{  
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
		m_pNext = pOther->m_pNext;
		m_pPrev = pOther;
		pOther->m_pNext->m_pPrev = this;
		pOther->m_pNext = this;
	}

	EventNodeBase(EventNodeBase * pOther)
	{
		m_pNext = pOther->m_pNext;
		m_pPrev = pOther;
		pOther->m_pNext->m_pPrev = this;
		pOther->m_pNext = this;
	}

	virtual void OnRemoveNext()
	{
	}

public:
	EventNodeBase()
	{
		m_pNext = m_pPrev = this;
	}

	EventNodeBase(const EventNodeBase & other)
	{
		if ( other.empty() ) {
			m_pNext = m_pPrev = this;
		} else {
			m_pNext = other.m_pNext;
			m_pPrev = (EventNodeBase *)&other;
			((EventNodeBase &)other).m_pNext->m_pPrev = this;
			((EventNodeBase &)other).m_pNext = this;
		}
	}

	virtual ~EventNodeBase()
	{
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
	}

	bool empty() const
	{
		return m_pNext == this;
	}

	const EventNodeBase & operator = (const EventNodeBase & other)
	{
		if ( this != &other ) {
			if ( other.empty() ) {
				Remove();
			} else {
				InsertAfter((EventNodeBase *)&other);
			}
		}
		return *this;
	}

	virtual void OnEvent(int message, void * param1, void * param2)
	{
	}
};

class SYSCORE_API EventNode : public EventNodeBase
{
public:
	EventNode()
	{
	}

	EventNode(const EventNode & other)
		: EventNodeBase(other)
	{
	}

	virtual ~EventNode()
	{
		m_pPrev->OnRemoveNext();
	}

	virtual void OnEvent(int message, void * param1, void * param2)
	{
	}

	void DisconnectEvent()
	{
		Remove();
	}
};

class SYSCORE_API Event : protected EventNodeBase
{
public:
	Event()
	{
	}

	Event(const Event & other)
		: EventNodeBase(other)
	{
	}

	bool empty() const
	{
		return m_pNext == (EventNodeBase *)this;
	}

	void Connect(EventNode & pNode)
	{
		pNode.InsertBefore(this);
	}

	void Disconnect(EventNode & pNode)
	{
		pNode.Remove();
	}

	void operator () (int message, void * param1 = NULL, void * param2 = NULL) const
	{	if ( !empty() ) for ( EventNodeBase iter(m_pNext), end(m_pPrev); iter.m_pPrev != &end; iter.InsertAfter(iter.m_pNext) )
			iter.m_pPrev->OnEvent(message, param1, param2); // code formatting here is intentional, for ease of debugging
	}
};

template <typename T>
class EventHook : public EventNode
{
protected:
	T * m_pObject;
	void (T::* m_pHandler)(int, void *, void *);

	virtual void OnEvent(int message, void * param1, void * param2)
	{
		SCL_ASSERT(m_pObject && m_pHandler);
		(m_pObject->*m_pHandler)(message, param1, param2);
	}

public:
	EventHook(T * pObject = NULL, void (T::* pHandler)(int, void *) = NULL)
	{
		m_pObject = pObject;
		m_pHandler = pHandler;
	}

	EventHook(const EventHook<T> & other)
		: EventNode(other)
	{
		m_pObject = other.m_pObject;
		m_pHandler = other.m_pHandler;
	}
};

} // namespace SYS

#endif // __SCL_EVENT_H__

