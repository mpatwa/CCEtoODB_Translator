/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_DLIST_H__
#define __SCL_DLIST_H__

#include "SCL_Swapper.h"

namespace SCL
{

template <class T> class DList;
template <class T, class T1> class DListIterator;

template <class T>
class DListNode
{
	friend class DList<T>;
	friend class DListIterator<T, T>;
	friend class DListIterator<const T, T>;

	void operator = (const DListNode<T> & other)
	{
	}

protected:
	T * m_pNext, * m_pPrev;

	void remove()
	{
		DListNode<T>::m_pPrev->DListNode<T>::m_pNext = DListNode<T>::m_pNext;
		DListNode<T>::m_pNext->DListNode<T>::m_pPrev = DListNode<T>::m_pPrev;
		DListNode<T>::m_pNext = DListNode<T>::m_pPrev = (T *)this;
	}

public:
	DListNode()
	{
		DListNode<T>::m_pNext = DListNode<T>::m_pPrev = (T *)this;
	}
	~DListNode()
	{
		DListNode<T>::m_pPrev->DListNode<T>::m_pNext = DListNode<T>::m_pNext;
		DListNode<T>::m_pNext->DListNode<T>::m_pPrev = DListNode<T>::m_pPrev;
	}
	T * GetNextNode()
	{
		return DListNode<T>::m_pNext;
	}
	T * GetPrevNode()
	{
		return DListNode<T>::m_pPrev;
	}
};

template <class T, class T1 = T>
class DListIterator
{
protected:
	T * m_Focus;

	typedef DListNode<T1> node;

public:
	typedef DListIterator<T, T1> iterator;

public:
	DListIterator(T * value = NULL)
	{
		m_Focus = value;
	}

	DListIterator(const iterator & other)
	{
		m_Focus = other.m_Focus;
	}

	void operator = (const iterator & other)
	{
		m_Focus = other.m_Focus;
	}
	void operator = (T * value)
	{
		m_Focus = value;
	}

	operator T * () const
	{
		return (T *)(node *)m_Focus;
	}
	T * operator -> () const
	{
		return (T *)(node *)m_Focus;
	}

	// prefix versions
	iterator & operator ++ ()
	{
		m_Focus = m_Focus->node::m_pNext;
		return *this;
	}
	iterator & operator -- ()
	{
		m_Focus = m_Focus->node::m_pPrev;
		return *this;
	}

	// postfix version
	T * operator ++ (int)
	{
		T * retval = m_Focus;
		m_Focus = m_Focus->node::m_pNext;
		return retval;
	}
	T * operator -- (int)
	{
		T * retval = m_Focus;
		m_Focus = m_Focus->node::m_pPrev;
		return retval;
	}
};

template <class T>
class DList : public DListNode<T>
{
protected:
	typedef DListNode<T> node;

	friend class DListIterator<T, T>;

public:
	typedef DListIterator<T, T> iterator;
	typedef DListIterator<const T, T> const_iterator;

public:
	void clear()
	{
		while ( this->DListNode<T>::m_pNext != (T *)(node *)this ) {
			node * tmp = this->DListNode<T>::m_pNext; // compile time check if T is a node derivative
			this->DListNode<T>::m_pNext = this->DListNode<T>::m_pNext->DListNode<T>::m_pNext;
			tmp->DListNode<T>::m_pNext = tmp->DListNode<T>::m_pPrev = (T *)tmp;
		}
		this->DListNode<T>::m_pPrev = (T *)(node *)this;
	}

	DList()
	{
	}

	~DList()
	{
		clear();
	}

	bool empty() const
	{
		return this->DListNode<T>::m_pNext == (T *)(node *)this;
	}

	T * front()
	{
		return this->DListNode<T>::m_pNext;
	}

	T * back()
	{
		return this->DListNode<T>::m_pPrev;
	}

	const T * front() const
	{
		return this->DListNode<T>::m_pNext;
	}

	const T * back() const
	{
		return this->DListNode<T>::m_pPrev;
	}

	T * begin()
	{
		return this->DListNode<T>::m_pNext;
	}

	T * end()
	{
		return (T *)(node *)this;
	}

	const T * begin() const
	{
		return this->DListNode<T>::m_pNext;
	}

	const T * end() const
	{
		return (T *)(node *)this;
	}

	T * rbegin()
	{
		return this->DListNode<T>::m_pPrev;
	}

	T * rend()
	{
		return (T *)(node *)this;
	}

	const T * rbegin() const
	{
		return this->DListNode<T>::m_pPrev;
	}

	const T * rend() const
	{
		return (T *)(node *)this;
	}

	T * insert(T * before_node, T & new_node)
	{
		SCL_ASSERT(before_node);
		SCL_ASSERT(new_node.DListNode<T>::m_pNext == &new_node && new_node.DListNode<T>::m_pPrev == &new_node);

		before_node->DListNode<T>::m_pPrev->DListNode<T>::m_pNext = &new_node;
		new_node.DListNode<T>::m_pPrev = before_node->DListNode<T>::m_pPrev;
		before_node->DListNode<T>::m_pPrev = &new_node;
		new_node.DListNode<T>::m_pNext = before_node;

		return &new_node;
	}

	void remove(T & a_node)
	{
		a_node.remove();
	}

	void push_front(T & new_node)
	{
		this->DListNode<T>::m_pNext->DListNode<T>::m_pPrev = &new_node;
		new_node.DListNode<T>::m_pNext = this->DListNode<T>::m_pNext;
		this->DListNode<T>::m_pNext = &new_node;
		new_node.DListNode<T>::m_pPrev = (T *)(node *)this;
	}

	T * pop_front()
	{
		T * retval = this->DListNode<T>::m_pNext;
		retval->remove();
		return retval;
	}

	void push_back(T & new_node)
	{
		this->DListNode<T>::m_pPrev->DListNode<T>::m_pNext = &new_node;
		new_node.DListNode<T>::m_pPrev = this->DListNode<T>::m_pPrev;
		this->DListNode<T>::m_pPrev = &new_node;
		new_node.DListNode<T>::m_pNext = (T *)(node *)this;
	}

	T * pop_back()
	{
		T * retval = this->DListNode<T>::m_pPrev;
		retval->remove();
		return retval;
	}

	void swap(DList<T> & other)
	{
		SCL::swap(this->DListNode<T>::m_pNext, other.DListNode<T>::m_pNext);
		SCL::swap(this->DListNode<T>::m_pPrev, other.DListNode<T>::m_pPrev);
	}

	void merge(DList<T> & other)
	{
		this->DListNode<T>::m_pPrev->DListNode<T>::m_pNext = (T *)(node *)&other;
		other.DListNode<T>::m_pPrev = this->DListNode<T>::m_pPrev;

		this->DListNode<T>::m_pPrev = other.DListNode<T>::m_pPrev;
		other.DListNode<T>::m_pPrev->DListNode<T>::m_pNext = (T *)(node *)this;

		other.node::remove();
	}

	void reverse()
	{
		node * it = this;
		do {
			SCL::swap(it->DListNode<T>::m_pNext, it->DListNode<T>::m_pPrev);
			it = it->DListNode<T>::m_pPrev; // Old next is now prev
		} while ( it != this );
	}
};

template <class T>
class DDList : public DList<T>
{
protected:
	typedef DListNode<T> node;

public:
	void clear()
	{
		while ( this->DListNode<T>::m_pNext != (T *)(node *)this ) {
			delete this->DListNode<T>::m_pNext; // updates DListNode<T>::m_pNext automatically
		}
	}

	~DDList()
	{
		clear();
	}

	void remove(T * a_node)
	{
		delete a_node;
	}
};

} // namespace SCL

#endif // __SCL_DLIST_H__

