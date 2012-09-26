/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_TREE_H__
#define __SCL_TREE_H__

namespace SCL
{

template <class T> 
struct TreeNode
{
	typedef TreeNode<T> node;

	node * m_pLeft, * m_pParent, * m_pRight;
	int m_nBalance : 3;
	unsigned m_nCount : 29;

	T m_Value;

	template <class U>
	TreeNode(const U & x, node * parent)
		: m_Value(x), m_pParent(parent), m_pLeft(NULL), m_pRight(NULL), m_nBalance(0), m_nCount(1)
	{
	}

	node * rightmost() const
	{
		node * retval = (node *)this;
		while ( retval->m_pRight ) {
			retval = retval->m_pRight;
		}
		return retval;
	}

	node * leftmost() const
	{
		node * retval = (node *)this;
		while ( retval->m_pLeft ) {
			retval = retval->m_pLeft;
		}
		return retval;
	}

	node * prev() const
	{
		if ( m_pLeft ) {
			return m_pLeft->rightmost();
		}
		node * retval = (node *)this;
		for ( bool b = true; b && retval; ) {
			node * p = retval->m_pParent;
			b = (p && retval == p->m_pLeft);
			retval = p;
		}
		return retval;
	}

	node * next() const
	{
		if ( m_pRight ) {
			return m_pRight->leftmost();
		}
		node * retval = (node *)this;
		for ( bool b = true; b && retval; ) {
			node * p = retval->m_pParent;
			b = (p && retval == p->m_pRight);
			retval = p;
		}
		return retval;
	}
};

template <class T> class TreeIterator;

template <class T> 
class TreeCIterator
{
public:
	typedef TreeNode<T> node;
	typedef TreeIterator<T> iterator;
	typedef TreeCIterator<T> const_iterator;

	friend class TreeIterator<T>;

protected:
	node * m_Focus;

public:
	TreeCIterator()
	{
	}

	TreeCIterator(node * x)
	 : m_Focus(x)
	{
	}

	TreeCIterator(const iterator & x)
	 : m_Focus(x.m_Focus)
	{
	}

	const T & operator * () const
	{
		return m_Focus->m_Value;
	}

	const T * operator -> () const
	{
		return &m_Focus->m_Value;
	}

	const T & get_value() const
	{
		return m_Focus->m_Value;
	}

	handle get_handle() const
	{
		return (handle)m_Focus;
	}

	void set_handle(handle a_handle)
	{
		m_Focus = (node *)a_handle;
	}

	// prefix versions
	const_iterator & operator ++ ()
	{
		m_Focus = m_Focus->next();
		return *this;
	}
	const_iterator & operator -- ()
	{
		m_Focus = m_Focus->prev();
		return *this;
	}

	// postfix version
	const_iterator operator ++ (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->next();
		return retval;
	}
	const_iterator operator -- (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->prev();
		return retval;
	}

	bool operator == (const const_iterator & x) const
	{
		return m_Focus == x.m_Focus;
	}

	bool operator != (const const_iterator & x) const
	{
		return m_Focus != x.m_Focus;
	}
};

template <class T> 
class TreeIterator
{
public:
	typedef TreeNode<T> node;
	typedef TreeIterator<T> iterator;
	typedef TreeCIterator<T> const_iterator;

	friend class TreeCIterator<T>;

	node * m_Focus;

	TreeIterator()
	{
	}

	TreeIterator(node * x)
	 : m_Focus(x)
	{
	}

	TreeIterator(const iterator & x)
	 : m_Focus(x.m_Focus)
	{
	}

	T & operator * () const
	{
		return m_Focus->m_Value;
	}

	T * operator -> () const
	{
		return &m_Focus->m_Value;
	}

	const T & get_value() const
	{
		return m_Focus->m_Value;
	}

	handle get_handle() const
	{
		return m_Focus;
	}

	void set_handle(handle a_handle)
	{
		m_Focus = (node *)a_handle;
	}

	// prefix versions
	iterator & operator ++ ()
	{
		m_Focus = m_Focus->next();
		return *this;
	}
	iterator & operator -- ()
	{
		m_Focus = m_Focus->prev();
		return *this;
	}

	// postfix version
	iterator operator ++ (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->next();
		return retval;
	}
	iterator operator -- (int)
	{
		node * retval = m_Focus;
		m_Focus = m_Focus->prev();
		return retval;
	}

	bool operator == (const const_iterator & x) const
	{
		return m_Focus == x.m_Focus;
	}

	bool operator != (const const_iterator & x) const
	{
		return m_Focus != x.m_Focus;
	}
};

/*!
The template class describes an object that controls a varying-length sequence of
elements of type <tt><b>TKEY</b></tt>. Each element serves as both a sort key and a value.
The sequence is represented in a way that permits lookup, insertion, and removal of
an arbitrary element with a number of operations proportional to the logarithm 
of the number of elements in the sequence (logarithmic time). Moreover, inserting 
an element invalidates no iterators, and removing an element invalidates only those 
iterators that point at the removed element.

\note
The template class stores the data using an <b>AVL tree</b> structure.<br>
<tt>AVL tree</tt> is a binary tree in which the difference between the height of the right
 and left subtrees (or the root node) is never more than one.

&nbsp;
\note
To encrease performance of the class Tree there are some limitations on <tt>end</tt>'s
iterators: it's impossible to call <tt>operator -- () </tt> for iterators returned
by <tt>end()</tt> and <tt>rend()</tt>.

The object orders the sequence it controls by calling a stored function object
of type <tt>PRED</tt>. You access this stored object by calling the member
function m_Comp(). Such a function object must impose a total order on sort keys.
For any element <tt>x</tt> that precedes <tt>y</tt> in the sequence,
<tt>m_Comp()(y, x)</tt> is <tt>false</tt>.
(For the default function object <tt>SCL::less<TKEY></tt>, sort keys never decrease
in value.)
\note By default the class ensures that all keys are uniqie. But it's possible to
change this behaviour by setting a field <tt>m_bMulti</tt> to <tt>true</tt>.

The object allocates and frees storage for the sequence it controls through a protected 
object named <tt>m_Allocator</tt>, of <tt>class A</tt>. Such a <tt>m_Allocator</tt> object
must have the same external interface as an object of template class <tt>SCL::alloc</tt>.

\note that <tt>m_Allocator</tt> is not copied when the object is assigned.
*/
template <class TKEY, class PRED = SCL::less<TKEY>, class A = alloc, class N = NullNotifier>
class Tree : public N
{
public:
	/// The type describes a <tt>node</tt> of the <tt>tree</tt>.
	typedef TreeNode<TKEY> node;
	/// The type describes a pointer to the <tt>node</tt>.
	typedef TreeNode<TKEY> * nodeptr;
	/// The type is synonym for <tt>Tree<TKEY, PRED, A></tt>.
	typedef Tree<TKEY, PRED, A> tree;
	/// The type describes an object that can serve as a bidirectional iterator for the controlled sequence.
	typedef TreeIterator<TKEY> iterator;
	/// The type describes an object that can serve as a constant bidirectional iterator for the controlled sequence.
	typedef TreeCIterator<TKEY> const_iterator;
	/// The unsigned integer type describes an object that can represent the length of any controlled sequence.
	typedef typename A::size_type size_type;

	node * m_Root;
	iterator m_Min, m_Max;

	/// If the field is <tt>true</tt> than the <tt>tree</tt> can contains equal keys, otherwise it cannot.
	bool m_bMulti;

	A m_Allocator;
	/// Compare function.
	PRED m_Comp;

	/// The type is a synonym for <tt>pair<iterator, bool></tt>.
	typedef pair<iterator, bool> ibpair;

protected:
	// Return a reference to pointer to *pNode
	// Handle boundary conditions properly
	nodeptr & get_pptr(nodeptr pNode)
	{
		nodeptr pParent = pNode->m_pParent;
		if ( !pParent ) {
			return m_Root;
		} else if ( pNode == pParent->m_pLeft ) {
			return pParent->m_pLeft;
		}
		return pParent->m_pRight;
	}

	void rotate_left(nodeptr p)
	{
		nodeptr & parent = get_pptr(p), child = p->m_pRight;

		int tmp = child->m_nCount;
		child->m_nCount = p->m_nCount;
		p->m_nCount -= tmp;

		if ( (p->m_pRight = child->m_pLeft) != NULL ) {
			p->m_pRight->m_pParent = p;
			p->m_nCount += p->m_pRight->m_nCount;
		}

		child->m_pParent = p->m_pParent;
		parent = child;

		child->m_pLeft = p;
		p->m_pParent = child;

		--p->m_nBalance;
		if ( child->m_nBalance > 0 ) {
			p->m_nBalance -= child->m_nBalance;
		}
		--child->m_nBalance;
		if ( p->m_nBalance < 0 ) {
			child->m_nBalance += p->m_nBalance;
		}
	}

	void rotate_right(nodeptr p)
	{
		nodeptr & parent = get_pptr(p), child = p->m_pLeft;
	
		int tmp = child->m_nCount;
		child->m_nCount = p->m_nCount;
		p->m_nCount -= tmp;

		if ( (p->m_pLeft = child->m_pRight) != NULL ) {
			p->m_pLeft->m_pParent = p;
			p->m_nCount += p->m_pLeft->m_nCount;
		}

		child->m_pParent = p->m_pParent;
		parent = child;

		child->m_pRight = p;
		p->m_pParent = child;

		++p->m_nBalance;
		if ( child->m_nBalance < 0 ) {
			p->m_nBalance -= child->m_nBalance;
		}
		++child->m_nBalance;
		if ( p->m_nBalance > 0 ) {
			child->m_nBalance += p->m_nBalance;
		}
	}

	void balance(nodeptr pParent)
	{
		if ( pParent ) {
			nodeptr pCheck = (pParent->m_nBalance > 0) ? pParent->m_pRight : pParent->m_pLeft;
			if ( pParent->m_nBalance == -2 ) {
				if ( pCheck->m_nBalance == 1 ) {
					rotate_left(pCheck);
				}
				rotate_right(pParent);
			} else if ( pParent->m_nBalance == 2 ) {
				if ( pCheck->m_nBalance == -1 ) {
					rotate_right(pCheck);
				}
				rotate_left(pParent);
			}
		}
	}

	// Set pNode's parent to pOld's parent.
	// Return true if pOld was its parent's left child
	bool reparent(nodeptr pNode, nodeptr pOld)
	{
		nodeptr pParent = pOld->m_pParent;
		bool bLeft = (pParent && pParent->m_pLeft == pOld);

		nodeptr & p = get_pptr(pOld);
		p = pNode;

		if ( pNode ) {
			pNode->m_pParent = pParent;
		}
		return bLeft;
	}

	void copy(const tree & x)
	{
		clear();
		m_Comp = x.m_Comp;
		m_bMulti = x.m_bMulti;

		if ( x.m_Root ) {

			nodeptr p, xp, stack[34], xstack[34];
			int stack_ptr = 0;

			xp = x.m_Root;

			p = (nodeptr)m_Allocator._Charalloc(sizeof(node));
			new (p) node(xp->m_Value, NULL);
			p->m_nBalance = xp->m_nBalance;
			p->m_nCount = xp->m_nCount;
			if ( xp == x.m_Min.m_Focus ) {
				m_Min.m_Focus = p;
			}
			if ( xp == x.m_Max.m_Focus ) {
				m_Max.m_Focus = p;
			}

			m_Root = p;

			xstack[++stack_ptr] = x.m_Root;
			stack[stack_ptr] = m_Root;

			while ( stack_ptr ) {

				xp = xstack[stack_ptr];
				p = stack[stack_ptr];

				if ( xp->m_pLeft && !p->m_pLeft ) {
					nodeptr pn = (nodeptr)m_Allocator._Charalloc(sizeof(node));
					new (pn) node(xp->m_pLeft->m_Value, p);
					pn->m_nBalance = xp->m_pLeft->m_nBalance;
					pn->m_nCount = xp->m_pLeft->m_nCount;
					p->m_pLeft = pn;
					if ( xp->m_pLeft == x.m_Min.m_Focus ) {
						m_Min.m_Focus = pn;
					}

					SCL_ASSERT(stack_ptr < COUNTOF(stack));
					xstack[++stack_ptr] = xp->m_pLeft;
					stack[stack_ptr] = pn;
					continue;
				}

				if ( xp->m_pRight && !p->m_pRight ) {
					nodeptr pn = (nodeptr)m_Allocator._Charalloc(sizeof(node));
					new (pn) node(xp->m_pRight->m_Value, p);
					pn->m_nBalance = xp->m_pRight->m_nBalance;
					pn->m_nCount = xp->m_pRight->m_nCount;
					p->m_pRight = pn;
					if ( xp->m_pRight == x.m_Max.m_Focus ) {
						m_Max.m_Focus = pn;
					}

					SCL_ASSERT(stack_ptr < COUNTOF(stack));
					xstack[++stack_ptr] = xp->m_pRight;
					stack[stack_ptr] = pn;
					continue;
				}

				--stack_ptr;
			} // while ( stack_ptr )

			this->Notify(EventCode::Reload);
		}
	}

	ibpair _insert(const TKEY & x)
	{
		if ( !m_Root ) {
			m_Root = (nodeptr)m_Allocator._Charalloc(sizeof(node));
			new (m_Root) node(x, NULL);
			m_Min = m_Max = m_Root;
			return ibpair(iterator(m_Root), true);
		}
		nodeptr pNode = m_Root, * pNext;
		while ( true ) {
			if ( m_Comp(x, pNode->m_Value) ) {
				pNext = &pNode->m_pLeft;
			} else if ( m_bMulti || m_Comp(pNode->m_Value, x) ) {
				pNext = &pNode->m_pRight;
			} else {
				return ibpair(iterator(pNode), false);
			}
			if ( !*pNext ) {
				nodeptr pParent = pNode;
				*pNext = (nodeptr)m_Allocator._Charalloc(sizeof(node));
				new (*pNext) node(x, pParent);
				if ( pNext == &m_Min.m_Focus->m_pLeft ) {
					m_Min = *pNext;
				}
				if ( pNext == &m_Max.m_Focus->m_pRight ) {
					m_Max = *pNext;
				}
				pNode = *pNext;
				break;
			}
			pNode = *pNext;
		} // while ( true )
		bool bBalance = true;
		nodeptr pCheck = pNode, pParent = pNode->m_pParent;
		for ( ; pParent; pParent = pParent->m_pParent ) {
			if ( bBalance ) {
				if ( pParent->m_pLeft == pCheck ) {
					--pParent->m_nBalance;
				} else {
					++pParent->m_nBalance;
				}
				if ( abs(pParent->m_nBalance) != 1 ) {
					bBalance = false;
				}
				pCheck = pParent;
			} // if ( bBalance )
			++pParent->m_nCount;
		}
		balance(pCheck);
		return ibpair(iterator(pNode), true);
	}

public:
	/*!
	The constructor with an argument named <tt>comp</tt> stores the function to compare 
	<tt>keys</tt> (<tt>m_Comp</tt>()).<br>
	An argument named <tt>bMulti</tt> indicates wheteher the <tt>tree</tt> can
	save equal <tt>keys</tt>.<br>
	The constructor also stores the allocator object <tt>al</tt> 
	in <tt>m_Allocator</tt>.<br>
	The constructor specifies an empty initial controlled sequence. 
	*/
	explicit Tree(const PRED & comp = PRED(), bool bMulti = true, const A & al = A())
		: m_Allocator(al), m_Comp(comp), m_Root(NULL), m_Min(NULL), m_Max(NULL), m_bMulti(bMulti)
	{
	}

	/*!
	The constructor specifies a copy of the sequence controlled by <tt>x</tt>.<br>
	It uses <tt>x.m_Allocator</tt> to init <tt>m_Allocator</tt>.
	*/
	Tree(const tree & x)
		: m_Allocator(x.m_Allocator), m_Root(NULL), m_Min(NULL), m_Max(NULL)
	{
		copy(x);
	}

	/*!
	Destructor. Calls a destructor of each element in the controlled sequence
	and frees allocated memory.
	*/
	~Tree()
	{
		clear();
	}

	/*!
	The member operator specifies a copy of the sequence controlled by <tt>x</tt>.
	*/
	Tree & operator = (const tree & x)
	{
		copy(x);
		return *this;
	}

	/*!
	The member function returns the length of the controlled sequence.
	*/
	size_type size() const
	{
		return m_Root ? m_Root->m_nCount : 0;
	}

	/*!
	The member function returns the length of the longest sequence that the object can control.
	*/
    size_type max_size() const
	{
		return m_Allocator.max_size();
	}

	/*!
	The member function returns <tt>true</tt> for an empty controlled sequence,
	otherwise - <tt>false</tt>.
	*/
	bool empty() const
	{
		return m_Root == 0;
	}

	/*!
	The member function returns <tt>m_Allocator</tt>.
	*/
	A get_allocator() const
	{
		return m_Allocator;
	}

	/*!
	The member function returns a bidirectional iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	iterator begin()
	{
		return m_Min;
	}

	/*!
	The member function returns an iterator that points just beyond the end of the sequence.
	\warning 
	<b>It's impossible to call <tt>operator -- () </tt> for <tt>end()</tt></b>
	*/
	iterator end()
	{
		return NULL;
	}

	/*!
	The member function returns a const bidirectional iterator that points at the first
	element of the sequence (or just beyond the end of an empty sequence).
	*/
	const_iterator begin() const
	{
		return m_Min;
	}

	/*!
	The member function returns a const iterator that points just beyond the end of the sequence.
	\warning 
	<b>It's impossible to call <tt>operator -- () </tt> for <tt>end()</tt></b>
	*/
	const_iterator end() const
	{
		return NULL;
	}

	/*!
	The member function returns a bidirectional iterator that points at
	the end element of the controlled sequence. Hence, it designates the beginning of
	the reverse sequence.
	*/
	iterator rbegin()
	{
		return m_Max;
	}

	/*!
	The member function returns a bidirectional iterator that points at the
	element before the first element of the sequence (or just beyond the end of an 
	empty sequence).
	Hence, it designates the end of the reverse sequence.
	\warning 
	<b>It's impossible to call <tt>operator -- () </tt> for <tt>rend()</tt></b>
	*/
	iterator rend()
	{
		return NULL;
	}

	/*!
	The member function returns a const bidirectional iterator that points at
	the end element of the controlled sequence. Hence, it designates the beginning of
	the reverse sequence.
	*/
	const_iterator rbegin() const
	{
		return m_Max;
	}

	/*!
	The member function returns a const bidirectional iterator that points at the
	element before the first element of the sequence (or just beyond the end of an 
	empty sequence).
	Hence, it designates the end of the reverse sequence.
	\warning 
	<b>It's impossible to call <tt>operator -- () </tt> for <tt>rend()</tt></b>
	*/
	const_iterator rend() const
	{
		return NULL;
	}

	/*!
	The member function returns an iterator that designates the earliest element
	in the controlled sequence whose sort key equals <tt>x</tt>.
	If no such element exists, the iterator equals <tt>end()</tt>.
	*/
	iterator find(const TKEY & x)
	{
		if ( !m_Root ) {
			return m_Root;
		}
		node * q = NULL;
		for ( node * p = m_Root; p; ) {
			if ( m_Comp(p->m_Value, x) ) {
				p = p->m_pRight;
			} else {
				q = p;
						p = p->m_pLeft;
				}
		}
		if ( q && !m_Comp(x, q->m_Value) ) {
			return q;
		}
		return end();
	}

	/*!
	The member function returns a const iterator that designates the earliest element
	in the controlled sequence whose sort key equals <tt>x</tt>.
	If no such element exists, the iterator equals <tt>end()</tt>.
	*/
	const_iterator find(const TKEY & x) const
	{
		if ( !m_Root ) {
			return m_Root;
		}
		node * q = NULL;
		for ( node * p = m_Root; p; ) {
			if ( m_Comp(p->m_Value, x) ) {
				p = p->m_pRight;
			} else {
				q = p;
				p = p->m_pLeft;
			}
		}
		if ( q && !m_Comp(x, q->m_Value) ) {
			return q;
		}
		return end();
	}

    TKEY & at(size_type pos)
	{
		nodeptr p = m_Root;
		while ( p && pos < p->m_nCount ) {
			size_type nl = p->m_pLeft ? p->m_pLeft->m_nCount : 0;
			if ( nl > pos ) {
				p = p->m_pLeft;
			} else if ( nl == pos ) {
				break;
			} else {
				p = p->m_pRight;
				pos -= nl + 1;
			}
		} // while ( p && x < p->m_nCount )
		SCL_ASSERT(p);
		return p->m_Value;
	}

	const TKEY & at(size_type pos) const
	{
		nodeptr p = m_Root;
		while ( p && pos < p->m_nCount ) {
			size_type nl = p->m_pLeft ? p->m_pLeft->m_nCount : 0;
			if ( nl > pos ) {
				p = p->m_pLeft;
			} else if ( nl == pos ) {
				break;
			} else {
				p = p->m_pRight;
				pos -= nl + 1;
			}
		} // while ( p && x < p->m_nCount )
		SCL_ASSERT(p);
		return p->m_Value;
	}

	/*!
	If <tt>m_bMulti</tt> is <tt>true</tt> then the member function returns
	a <tt>ibpair</tt> in which iterator points to inserted element and <tt>bool</tt> is <tt>true</tt>.
	<p>
	If <tt>m_bMulti</tt> is <tt>false</tt> then the member function determines 
	whether an element <tt>y</tt> exists in the sequence whose key matches that of <tt>x</tt>.
	(The keys match if <tt>!m_Comp(x.first, y.first) && !m_Comp(y.first, x.first)</tt>.)
	If not, it creates such an element <tt>y</tt> and initializes it with <tt>x</tt>.
	The function then determines the iterator <tt>it</tt> that designates <tt>y</tt>.
	The function returns <tt>ibpair</tt> in which iterator is <tt>it</tt> and 
	<tt>bool</tt> is <tt>true</tt> if an insertion occurred or <tt>false</tt> otherwise.
	*/
	ibpair insert(const TKEY & x)
	{
		ibpair retval = _insert(x);
		if ( !retval.second ) {
			TKEY * pVal = &*retval.first;
			if ( !(*pVal == x) ) {
				pVal->~TKEY();
				new (pVal) TKEY(x);
				Notify(EventCode::Change, retval.first.get_handle());
			}
		} else {
			Notify(EventCode::Add, retval.first.get_handle());
		}
		return retval;
	}

	/*!
	The member function removes the element of the controlled sequence pointed to by
	<tt>x</tt> and returns an iterator that designates the first element remaining beyond
	any elements removed, or <tt>end()</tt> if no such element exists.
	*/
	iterator erase(iterator x)
	{
		if ( x == end() ) {
			return end();
		}

		Notify(EventCode::Delete, x.m_Focus);

		nodeptr pNode = x.m_Focus, pRepl = NULL, pDel;
		iterator retval = x;
		++retval;

		if ( m_Min == x ) {
			++m_Min;
		}
		if ( m_Max == x ) {
			--m_Max;
		}

		// See Cormen, chapter 13.3, TREE-DELETE
		// x: pRepl, y: pDel, z: pNode

		// set pDel to a node that is going to be spliced out of the tree
		if ( !pNode->m_pLeft || !pNode->m_pRight ) {
			pDel = pNode;
		} else {
			// take the successor/predecessor node,
			// depending on what subtree is taller
			if ( pNode->m_nBalance > 0 ) {
				pDel = retval.m_Focus;
			} else {
				pDel = pNode->m_pLeft->rightmost();
			}
		}
		// set pRepl to a node that will be replacing pDel
		if ( pDel->m_pLeft ) {
			pRepl = pDel->m_pLeft;
		} else {
			pRepl = pDel->m_pRight;
		}
		nodeptr pParent = pDel->m_pParent;
		bool bLeft = reparent(pRepl, pDel);
		if ( pDel != pNode ) {
			// This case corresponds to fig.13.4(c) in Cormen.
			// Instead of assigning pNode->m_Value = pDel->m_Value,
			// here we are replacing pNode with pDel in the tree.
			reparent(pDel, pNode);
			if ( (pDel->m_pLeft = pNode->m_pLeft) != NULL ) {
				pDel->m_pLeft->m_pParent = pDel;
			}
			if ( (pDel->m_pRight = pNode->m_pRight) != NULL ) {
				pDel->m_pRight->m_pParent = pDel;
			}
			pDel->m_nBalance = pNode->m_nBalance;
			pDel->m_nCount = pNode->m_nCount;
		}
		if ( pParent == pNode ) {
			pParent = pDel;
		}

		// pParent points to parent node of the removed one.
		// Climb up from pParent to the root, adjusting balance factors.
		for ( bool bBalance = true; pParent; ) {
			--pParent->m_nCount;
			nodeptr p = pParent->m_pParent;
			if ( bBalance ) {
				// if we got here, height of a subtree is changed
				if ( bLeft ) {
					++pParent->m_nBalance;
				} else {
					--pParent->m_nBalance;
				}
				bLeft = (p && p->m_pLeft == pParent); // are we in the left subtree of pParent's parent?
				// pp is a reference to pointer to *pParent, be it m_Root or pParent->m_pXXX
				nodeptr & pp = get_pptr(pParent);
				SCL_ASSERT(pp && pp == pParent);
				if ( abs(pParent->m_nBalance) == 2 ) {
					// the pParent is unbalanced
					balance(pParent);
				}
				// pp points to the place in the tree where pParent pointed.
				// The pParent itself may have been changed when balancing.
				if ( abs(pp->m_nBalance) == 1 ) {
					bBalance = false; // subtree height not changed, so upper nodes balance is not affected
				}
			}
			pParent = p; // step up
		}

		pNode->m_Value.~TKEY();
		m_Allocator._Chardealloc(pNode, sizeof(node));

		return retval;
	}

	/*!
	The member function removes the elements in the interval <tt>[first, last)</tt>
	and returns an iterator that designates the first element remaining beyond
	any elements removed, or <tt>end()</tt> if no such element exists.
	*/
	iterator erase(iterator first, iterator last)
	{
		if ( first == begin() && last == end() ) {
			clear();
			return end();
		}
		while ( first != last ) {
			first = erase(first);
		}
		return first;
	}

	/*!
	The member function calls destructors of each element stored in the controlled 
	sequence and deallocates used memory. 
	*/
	void clear()
	{
		if ( !m_Root ) {
			SCL_ASSERT(m_Min == 0);
			SCL_ASSERT(m_Max == 0);
			return;
		}

		this->Notify(EventCode::Clear);

		nodeptr stack[34];
		nodeptr * stack_ptr = stack;

		*stack_ptr = m_Root;

		while ( stack_ptr >= stack ) {
			SCL_ASSERT(stack_ptr < stack + COUNTOF(stack));
			nodeptr p = *stack_ptr--;
			if ( p->m_pLeft ) {
				*++stack_ptr = p->m_pLeft;
			}
			if ( p->m_pRight ) {
				*++stack_ptr = p->m_pRight;
			}
			p->m_Value.~TKEY();
			m_Allocator._Chardealloc(p, sizeof(node));
		}

		m_Root = NULL;
		m_Min = NULL;
		m_Max = NULL;
	}

	/*!
	The member function swaps the controlled sequences between <tt>*this</tt> and <tt>x</tt>. 
	*/
	void swap(tree & x)
	{
		SCL::swap(m_Root, x.m_Root);
		SCL::swap(m_bMulti, x.m_bMulti);
		SCL::swap(m_Min.m_Focus, x.m_Min.m_Focus);
		SCL::swap(m_Max.m_Focus, x.m_Max.m_Focus);

		this->Notify(EventCode::Reload);
		x.Notify(EventCode::Reload);
	}
};

} // namespace SCL

#endif // __SCL_TREE_H__

