/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_HASHTABLE_H__
#define __SCL_HASHTABLE_H__

#include "SCL_HashComp.h"
#include "SCL_Utils.h"

namespace SCL
{

template <class TKEY> class hash_table_iter;

template <class TKEY, class TRAITS = _hash_comp<TKEY>, class A = allocator<TKEY> >
class hash_table
{
	friend class hash_table_iter<TKEY>;

public:
	typedef TKEY key_type;
	typedef const TKEY const_key_type;

	typedef TKEY value_type;
	typedef const TKEY const_value_type;

	typedef TRAITS key_compare;

	typedef typename A::size_type size_type;
	typedef typename A::difference_type difference_type;

	typedef A allocator_type;

	typedef hash_table<TKEY, TRAITS, A> set;
	typedef hash_table_iter<key_type> iterator;
	typedef hash_table_iter<const_key_type> const_iterator;

protected:
	TRAITS m_HashComp;
	A m_Allocator;

	size_type m_nSize, m_nTableSize;

	char * m_Bitmap;
	TKEY * m_Table;

	void init(int nSize)
	{
		m_Table = NULL;
		m_Bitmap = NULL;
		m_nSize = m_nTableSize = 0;
		if ( nSize ) {
			resize(nSize);
		}
	}

	void destroy_table(TKEY * pTable, size_type nSize)
	{
		for ( size_type i = 0; i < nSize; ++i ) {
			pTable++->~TKEY();
		}
	}

	void construct_table(TKEY * pTable, size_type nSize)
	{
		for ( size_type i = 0; i < nSize; ++i ) {
			new (pTable++) TKEY();
		}
	}

	bool _find(const typename TRAITS::key_type & k, size_type & idx) const
	{
		if ( !m_nTableSize ) {
			idx = 0;
			return false;
		}
		unsigned hash = m_HashComp(k);
		idx = hash % m_nTableSize;
		size_type step = 1 + hash % (m_nTableSize - 2), freed_idx = -1;

		for ( size_type i = 0; i < m_nTableSize; ++i ) {
			char state = m_Bitmap[idx];
			if ( state & HashHelper::EMPTY ) {
				break;
			}
			if ( !(state & HashHelper::FREED) && m_HashComp.equal(m_Table[idx], k) ) {
				return true;
			}
			// as m_nTableSize is a prime, all idx values are different while 0 <= i < m_nSize
			idx = (idx + step) % m_nTableSize;
		}
		idx = m_nTableSize;
		return false;
	}

	bool _find_add(const typename TRAITS::key_type & k, size_type & idx) const
	{
		unsigned hash = m_HashComp(k);
		idx = hash % m_nTableSize;
		size_type step = 1 + hash % (m_nTableSize - 2), freed_idx = -1;

		bool bEqual = false;
		for ( size_type i = 0; i < m_nTableSize; ++i ) {
			char state = m_Bitmap[idx];
			if ( (state & HashHelper::FREED) && freed_idx == -1 ) {
				freed_idx = idx;
			}

			if ( (state & HashHelper::EMPTY) || !(state & HashHelper::FREED) && (bEqual = m_HashComp.equal(m_Table[idx], k)) /*assignment!*/ ) {
				break;
			}
			// as m_nTableSize is a prime, all idx values are different while 0 <= i < m_nSize
			idx = (idx + step) % m_nTableSize;
		}
		if ( bEqual ) {
			return true;
		}
		if ( freed_idx != -1 ) {
			idx = freed_idx;
		}
		return false;
	}

public:
	hash_table(int nTableSize = 0)
	{
		init(nTableSize);
	}

	explicit hash_table(const A & al, int nTableSize = 0)
		: m_Allocator(al)
	{
		init(nTableSize);
	}

	~hash_table()
	{
		if ( m_nTableSize ) {
			destroy_table(m_Table, m_nTableSize);
			m_Allocator.deallocate(m_Table, m_nTableSize);
			m_Allocator._Chardealloc(m_Bitmap - 1, m_nTableSize + 2);
		}
	}

	int size() const
	{
		return m_nSize;
	}

	void resize(size_type nSize)
	{
		if ( (int)nSize < 0 ) {
			SCL_ASSERT(FALSE);
			return;
		}

		size_type i;
		size_type old_size = m_nTableSize;
		m_nTableSize = nSize ? HashHelper::GetNewTableSize(nSize) : 0;
		SCL_ASSERT(m_nTableSize >= 0);
		m_nSize = SCL_MIN(nSize, m_nSize);

		TKEY * old_table = m_Table;
		char * old_bitmap = m_Bitmap;

		if ( m_nTableSize > 0 ) {
			m_Bitmap = m_Allocator._Charalloc(m_nTableSize + 2);
			*m_Bitmap = m_Bitmap[m_nTableSize] = 0;
			memset(++m_Bitmap, HashHelper::EMPTY, m_nTableSize);

			m_Table = m_Allocator.allocate(m_nTableSize, NULL);
			construct_table(m_Table, m_nTableSize);

			for ( i = 0; i < old_size; ++i ) {
				if ( !old_bitmap[i] ) {
					unsigned hash = m_HashComp(old_table[i]);
					size_type idx = hash % m_nTableSize;
					size_type j, step = 1 + hash % (m_nTableSize - 2);
					for ( j = 0; j < m_nTableSize; ++j ) {
						if ( m_Bitmap[idx] & HashHelper::EMPTY ) {
							break;
						}
						// as m_nTableSize is a prime, all idx values are different while 0 <= i < m_nSize
						idx = (idx + step) % m_nTableSize;
					}
					m_Bitmap[idx] = 0;
					m_Table[idx] = old_table[i];
				}
			}
		}
		if ( old_size ) {
			destroy_table(old_table, old_size);
			m_Allocator.deallocate(old_table, old_size);
			m_Allocator._Chardealloc(old_bitmap - 1, old_size + 2);
		}
	}

	bool empty() const
	{
		return (m_nSize == 0);
	}

	void clear()
	{
		destroy_table(m_Table, m_nTableSize);
		construct_table(m_Table, m_nTableSize);
		memset(m_Bitmap, HashHelper::EMPTY, m_nTableSize);
		m_nSize = 0;
	}

	iterator begin()
	{
		iterator retval(m_Table, m_Bitmap, 0);
		if ( m_Bitmap && *m_Bitmap ) {
			++retval;
		}
		return retval;
	}

	iterator end()
	{
		return iterator(m_Table, m_Bitmap, m_nTableSize);
	}

	const_iterator begin() const
	{
		const_iterator retval(m_Table, m_Bitmap, 0);
		if ( m_Bitmap && *m_Bitmap ) {
			++retval;
		}
		return retval;
	}

	const_iterator end() const
	{
		return const_iterator(m_Table, m_Bitmap, m_nTableSize);
	}

	pair <iterator, bool> insert(const typename TRAITS::key_type & k)
	{
		if ( m_nSize >= m_nTableSize * 0.7 ) {
			resize(m_nTableSize + 1);
		}

		size_type idx;
		bool bFound = _find_add(k, idx);
		SCL_ASSERT(idx >= 0 && idx < m_nTableSize);

		if ( !bFound ) {
			++m_nSize;
			m_Bitmap[idx] = 0;
			m_Table[idx] = k;
		}

		return pair<iterator, bool>(iterator(m_Table, m_Bitmap, idx), !bFound);
	}

	iterator erase(iterator pos)
	{
		size_type idx = (TKEY *)pos - m_Table;
		SCL_ASSERT(idx >= 0 && idx < m_nTableSize);
		char & state = m_Bitmap[idx];
		if ( !state ) {
			state = HashHelper::FREED;
			--m_nSize;
		}
		return ++pos;
	}

	int erase(const typename TRAITS::key_type & k)
	{
		size_type idx;
		if ( _find(k, idx) && idx < m_nTableSize ) {
			m_Bitmap[idx] = HashHelper::FREED;
			--m_nSize;
			return 1;
		}
		return 0;
	}

	iterator find(const typename TRAITS::key_type & k)
	{
		size_type idx = m_nTableSize;
		if ( m_nSize ) {
			_find(k, idx);
		}
		return iterator(m_Table, m_Bitmap, idx);
	}

	const_iterator find(const typename TRAITS::key_type & k) const
	{
		size_type idx;
		_find(k, idx);
		return const_iterator(m_Table, m_Bitmap, idx);
	}

	int max_size() const
	{
		return INT_MAX;
	}

	void swap(set & x)
	{
		SCL::swap(m_Table, x.m_Table);
		SCL::swap(m_Bitmap, x.m_Bitmap);
		SCL::swap(m_nSize, x.m_nSize);
		SCL::swap(m_nTableSize, x.m_nTableSize);
		SCL::swap(m_HashComp, x.m_HashComp);
	}
};

template <class TKEY>
class hash_table_iter
{
protected:
	TKEY * m_Focus;
	char * m_Flags;

public:
	typedef hash_table_iter<TKEY> iterator;

	hash_table_iter()
	{
		m_Focus = NULL;
		m_Flags = NULL;
	}

	hash_table_iter(TKEY * p, char * f, size_t idx)
	{
		m_Focus = p + idx;
		m_Flags = f + idx;
	}

	hash_table_iter(const iterator & x)
	{
		m_Focus = x.m_Focus;
		m_Flags = x.m_Flags;
	}

	iterator & operator = (const iterator & x)
	{
		m_Focus = x.m_Focus;
		m_Flags = x.m_Flags;
		return *this;
	}

	operator TKEY * () const
	{
		return m_Focus;
	}

	TKEY * operator -> () const
	{
		return m_Focus;
	}

	// prefix versions
	iterator & operator ++ ()
	{
		SCL_ASSERT(m_Flags);
		while ( ++m_Focus, *++m_Flags ) {
		}
		return *this;
	}
	iterator & operator -- ()
	{
		SCL_ASSERT(m_Flags);
		while ( --m_Focus, *--m_Flags ) {
		}
		return *this;
	}

	// postfix version
	iterator operator ++ (int)
	{
		iterator retval = *this;
		SCL_ASSERT(m_Flags);
		while ( ++m_Focus, *++m_Flags ) {
		}
		return retval;
	}
	iterator operator -- (int)
	{
		iterator retval = *this;
		SCL_ASSERT(m_Flags);
		while ( --m_Focus, *--m_Flags ) {
		}
		return retval;
	}
};

} // namespace SCL

#endif // __SCL_HASHTABLE_H__

