/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYS_POINTER_H__
#define __SYS_POINTER_H__

namespace SCL
{

class SYSCORE_API shared
{
protected:
	int m_nRefCount;

public:
	shared()
	{
		m_nRefCount = 0;
	}

	shared(const shared &)
	{
		m_nRefCount = 0;
	}

	~shared()
	{
	}

	shared & operator = (const shared & other)
	{
		return *this;
	}

	int AddRef()
	{
		return ++m_nRefCount;
	}

	int Release()
	{
		return --m_nRefCount;
	}
};

template <typename T>
class shared_ptr
{
protected:
	T * m_Ptr;

	void Assign(T * pNew)
	{
		T * pOld = m_Ptr;
		if ( (m_Ptr = pNew) != NULL ) {
			m_Ptr->shared::AddRef();
		}
		if ( pOld && pOld->shared::Release() == 0 ) {
			delete pOld;
		}
	}

public:
	shared_ptr(T * other = NULL)
	{
		m_Ptr = NULL;
		Assign(other);
	}

	shared_ptr(const shared_ptr & other)
	{
		m_Ptr = NULL;
		Assign(other.m_Ptr);
	}

	~shared_ptr()
	{
		Assign(NULL);
	}

	const shared_ptr & operator = (const T * other)
	{
		if ( m_Ptr != other ) {
			Assign((T *)other);
		}
		return *this;
	}

	shared_ptr & operator = (const shared_ptr & other)
	{
		if ( m_Ptr != other.m_Ptr ) {
			Assign(other.m_Ptr);
		}
		return *this;
	}

	bool operator ! () const
	{
		return m_Ptr == NULL;
	}

	template <class T1>
	bool operator == (const T1 x) const
	{
		return m_Ptr == (T *)x;
	}

	template <class T1>
	bool operator != (const T1 x) const
	{
		return m_Ptr != (T *)x;
	}

	template <class T1>
	bool operator == (const shared_ptr<T1> x) const
	{
		return m_Ptr == x.m_Ptr;
	}

	template <class T1>
	bool operator != (const shared_ptr<T1> x) const
	{
		return m_Ptr != x.m_Ptr;
	}

	const T * get() const
	{
		return m_Ptr;
	}

	T * get()
	{
		return m_Ptr;
	}

	const T * operator -> () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	T * operator -> ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	template <class R>
	const R & operator ->* (R T::* p) const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr->*p;
	}

	template <class R>
	R & operator ->* (R T::* p)
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr->*p;
	}

	const T & operator * () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}

	T & operator * ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}
};

/*!
The class describes an object that stores a pointer to an allocated object of type <tt><b>T</b></tt>.
The stored pointer must either be <tt>NULL</tt> or designate an object allocated by a <tt>new</tt>
expression. An object constructed with 
a <tt>non-NULL</tt> pointer owns the pointer. It transfers ownership if its stored value is
assigned to another object. The destructor for <tt>AutoPtr&lt;T></tt> deletes the allocated object
if it owns it.<br>
Hence, an object of class <tt>AutoPtr&lt;T></tt> ensures that an allocated object is automatically deleted 
when control leaves a block.
*/
template <class T>
class AutoPtr
{
protected:
	T * m_Ptr;

public:
	/// The type is a synonym for the <tt>AutoPtr&lt;T></tt>.
	typedef AutoPtr<T> ptr;

	/*!
	The member function sets the ownership indicator to <tt>NULL</tt>,
	then returns the stored pointer.
	*/
	T * release()
	{
	    T * x = m_Ptr;
		m_Ptr = NULL;
		return x;
	}
  
	void reset(T * x = NULL)
	{
		if ( x != m_Ptr ) {
			delete m_Ptr;
			m_Ptr = x; 
		}
	}

	/*!
	The member function returns the stored pointer.
	*/
	T * get() const
	{
		return m_Ptr;
	}

	/*!
	The constructor inits stored pointer with <tt>NULL</tt>.
	*/
	AutoPtr()
	{
		m_Ptr = NULL;
	}

  	/*!
	The constructor stores <tt>x</tt> as the pointer to the allocated object.
	*/
	explicit AutoPtr(T * x)
	{
		m_Ptr = x;
	}

	/*! 
	The constructor transfers ownership of the pointer stored in <tt>x</tt>,
	by storing both the pointer value and the ownership indicator from <tt>x</tt>
	in the constructed object. It effectively releases the pointer by
	calling <tt>x.release()</tt>.
	*/
	AutoPtr(const ptr & x)
	{
		m_Ptr = x.release();
	}

	/*! 
	The constructor transfers ownership of the pointer stored in <tt>x</tt>,
	by storing both the pointer value and the ownership indicator from <tt>x</tt>
	in the constructed object. It effectively releases the pointer by
	calling <tt>x.release()</tt>.
	*/
	template <class T1>
	AutoPtr(AutoPtr<T1> & x)
	{
		m_Ptr = x.release();
	}

	/*!
	The destructor deletes the object designated by the stored pointer <tt>p</tt> by evaluating
	the delete expression <tt>delete p</tt>.
	*/
	~AutoPtr()
	{
		delete m_Ptr;
	}

	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is not equal to <tt>NULL</tt>.
	*/
	operator bool () const
	{
		return m_Ptr != NULL;
	}
	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is equal to <tt>NULL</tt>.
	*/
	bool operator ! () const
	{
		return m_Ptr == NULL;
	}

	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is the same as the pointer <tt>x</tt> of type <tt>T1</tt>.
	\note It uses <tt>(T *)x</tt>.
	*/
	template <class T1>
	bool operator == (const T1 x) const
	{
		return m_Ptr == (T *)x;
	}

	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is not equal to the pointer <tt>x</tt> of type <tt>T1</tt>.
	\note It uses <tt>(T *)x</tt>.
	*/
	template <class T1>
	bool operator != (const T1 x) const
	{
		return m_Ptr != (T *)x;
	}

	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is equal to the stored pointer of object <tt>x</tt>.
	*/
	template <class T1>
	bool operator == (const AutoPtr<T1> x) const
	{
		return m_Ptr == x.m_Ptr;
	}

	/*!
	The member operator function returns <tt>true</tt> only if stored pointer 
	is not equal to the stored pointer of object <tt>x</tt>.
	*/
	template <class T1>
	bool operator != (const AutoPtr<T1> x) const
	{
		return m_Ptr != x.m_Ptr;
	}

	/*!
	The assignment operator deletes any pointer <tt>p</tt> that it owns, by evaluating
	the delete expression <tt>delete p</tt>. It then transfers ownership of the pointer
	stored in <tt>x</tt>, by storing both the pointer value and the ownership indicator
	from <tt>x</tt> in <tt>*this</tt>.<br>
	It effectively releases the pointer by calling <tt>x.release()</tt>.<br>
	The function returns <tt>*this</tt>.
	*/
	template <class T1>
	ptr & operator = (AutoPtr<T1> & x)
	{
		reset(x.release());
		return *this;
	}
 
	/*!
	The assignment operator deletes any pointer <tt>p</tt> that it owns, by evaluating
	the delete expression <tt>delete p</tt>. It then transfers ownership of the pointer
	stored in <tt>x</tt>, by storing both the pointer value and the ownership indicator
	from <tt>x</tt> in <tt>*this</tt>.<br>
	It effectively releases the pointer by calling <tt>x.release()</tt>.<br>
	The function returns <tt>*this</tt>.
	*/
	ptr & operator = (ptr & x)
	{
		reset(x.release());
		return *this;
	}

	/*!
	The assignment operator deletes any pointer <tt>p</tt> that it owns, by evaluating
	the delete expression <tt>delete p</tt>. It then transfers ownership of the pointer
	stored in <tt>x</tt>, by storing both the pointer value and the ownership indicator
	from <tt>x</tt> in <tt>*this</tt>.<br>
	It effectively releases the pointer by calling <tt>x.release()</tt>.<br>
	The function returns <tt>*this</tt>.
	*/
	ptr & operator = (T * x)
	{
		reset(x);
		return *this;
	}

	/*!
	The member operator function sets the ownership indicator to <tt>NULL</tt>,
	then returns the stored pointer casted to <tt>AutoPtr&lt;<b>T1</b>></tt>.
	*/
	template <class T1>
	operator AutoPtr<T1> ()
	{
		return AutoPtr<T1>(release());
	}

	/*!
	The selection operator effectively returns get(), so that the expression
	<tt>al->m</tt> behaves the same as <tt>(al.get())->m</tt>, where <tt>al</tt>
	is an object of class <tt>AutoPtr&lt;T></tt>.<br>
	Hence, the stored pointer <b>must not be NULL</b>, and <tt>T</tt> must be a class,
	structure, or union type.
	*/
	T * operator -> () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	/*!
	The indirection operator effectively returns *get().<br>
	Hence, the stored pointer <b>must not be NULL</b>.
	*/
	T & operator * () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}
};

class trackable : protected Event
{
public:
	// for use by weak_ptr<T> only! Better would be to make weak_ptr<T> a friend,
	// but this seems to be syntactically impossible in MSVC 6.0. Ideas?

	void RemovePointer(EventNode * pPointer)
	{
		Event::Disconnect(*pPointer);
	}

	void AddPointer(EventNode * pPointer)
	{
		Event::Connect(*pPointer);
	}

public:
	trackable()
	{
	}

	trackable(const trackable & other)
	{
	}

	const trackable & operator = (const trackable & other)
	{
		return *this;
	}

	~trackable()
	{
		Event::operator ()(EventCode::Delete);
	}
};

template <typename T> // T must be trackable derivative
class weak_ptr : protected EventNode
{
protected:
	T * m_Ptr;

	void OnEvent(int code, void * param1, void * param2)
	{
		if ( code == EventCode::Delete ) {
			m_Ptr = NULL;
		}
	}

public:
	weak_ptr()
	{
		m_Ptr = NULL;
	}

	weak_ptr(const weak_ptr & other)
		: EventNode(other)
	{
		m_Ptr = other.m_Ptr;
	}

	weak_ptr(T * pObject)
	{
		if ( (m_Ptr = pObject) != NULL ) {
			m_Ptr->AddPointer(this);
		}
	}

	~weak_ptr()
	{
	}

	T * get() const
	{
		return m_Ptr;
	}

	void set(T * x)
	{
		if ( m_Ptr != x ) {
			Remove();
			if ( (m_Ptr = x) != NULL ) {
				m_Ptr->AddPointer(this);
			}
		} 
	}

	operator bool () const
	{
		return m_Ptr != NULL;
	}

	bool operator ! () const
	{
		return m_Ptr == NULL;
	}

	template <class T1>
	bool operator == (const T1 & x) const
	{
		return m_Ptr == (T *)x;
	}

	template <class T1>
	bool operator != (const T1 & x) const
	{
		return m_Ptr != (T *)x;
	}

	template <class T1>
	bool operator == (const weak_ptr<T1> & x) const
	{
		return m_Ptr == x.m_Ptr;
	}

	template <class T1>
	bool operator != (const weak_ptr<T1> & x) const
	{
		return m_Ptr != x.m_Ptr;
	}

	const weak_ptr & operator = (T * x)
	{
		set(x);
		return *this;
	}

	const T * operator -> () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	T * operator -> ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	const T & operator * () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}

	T & operator * ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}
};

class shared_trackable : public shared, public trackable
{
public:
};

template <typename T>
class smart_ptr : public EventNode
{
protected:
	T * m_Ptr;

	void Assign(T * pNew)
	{
		T * pOld = m_Ptr;
		if ( (m_Ptr = pNew) != NULL ) {
			m_Ptr->AddRef();
		}
		if ( pOld && pOld->Release() == 0 ) {
			delete pOld;
		}
	}

	void OnEvent(int code, void * param1, void * param2)
	{
		m_Ptr = (T *)param1;
	}

public:
	smart_ptr(T * x = NULL)
	{
		m_Ptr = NULL;
		Assign(x);
		if ( m_Ptr ) {
			m_Ptr->AddPointer(this);
		}
	}

	smart_ptr(const smart_ptr & x)
		: EventNode(x)
	{
		m_Ptr = NULL;
		Assign(x.m_Ptr);
	}

	~smart_ptr()
	{
		Assign(NULL);
	}

	bool operator ! ()
	{
		return m_Ptr == NULL;
	}

	template <class T1>
	bool operator == (const T1 & x) const
	{
		return m_Ptr == (T *)x;
	}

	template <class T1>
	bool operator != (const T1 & x) const
	{
		return m_Ptr != (T *)x;
	}

	template <class T1>
	bool operator == (const smart_ptr<T1> & x) const
	{
		return m_Ptr == x.m_Ptr;
	}

	template <class T1>
	bool operator != (const smart_ptr<T1> & x) const
	{
		return m_Ptr != x.m_Ptr;
	}

	const smart_ptr & operator = (const T * x)
	{
		if ( m_Ptr != x ) {
			Remove();
			Assign((T *)x);
			if ( m_Ptr ) {
				m_Ptr->AddPointer(this);
			}
		}
		return *this;
	}

	smart_ptr & operator = (const smart_ptr & other)
	{
		if ( m_Ptr != other.m_Ptr ) {
			Assign(other.m_Ptr);
			if ( m_Ptr ) {
				m_Ptr->AddPointer(this);
			}
		}
		return *this;
	}

	const T * get() const
	{
		return m_Ptr;
	}

	T * get()
	{
		return m_Ptr;
	}

	const T * operator -> () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	T * operator -> ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return m_Ptr;
	}

	const T & operator * () const
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}

	T & operator * ()
	{
		SCL_ASSERT(m_Ptr != NULL);
		return *m_Ptr;
	}
};

} // namespace SCL

#endif // __SYS_POINTER_H__

