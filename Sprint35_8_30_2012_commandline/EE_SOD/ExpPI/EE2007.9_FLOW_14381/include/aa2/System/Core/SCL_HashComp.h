/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_HASHCOMP_H__
#define __SCL_HASHCOMP_H__

namespace SCL
{

namespace HashHelper
{

enum {
	EMPTY = 1,
	FREED = 2
};

SYSCORE_API extern int GetNewTableSize(int);
SYSCORE_API extern unsigned GetNewTableSize(unsigned);
SYSCORE_API extern int GetMaxTableSize();

} // namespace HashHelper

template <class TKEY>
struct hash_comp_econ
{
	typedef TKEY key_type;

	unsigned operator () (const TKEY & x) const
	{
		return unsigned(x);
	}
	bool operator () (const TKEY & x1, const TKEY & x2) const
	{
		return (x1 < x2);
	}
	bool equal(const TKEY & x1, const TKEY & x2) const
	{
		return x1 == x2;
	}
	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const TKEY & x1, const TKEY & x2, PRED pr) const
	{
		return (hv1 == hv2);
	}
	struct node
	{
		template <class PRED>
		unsigned get_hval(const TKEY & k, PRED pr)
		{
			return pr(k);
		}
		void set_hval(unsigned hv)
		{
		}
	};
};

template <class TKEY>
struct hash_comp
{
	typedef TKEY key_type;

	unsigned operator () (const TKEY & x) const
	{
		return unsigned(x);
	}
	bool operator () (const TKEY & x1, const TKEY & x2) const
	{
		return (x1 < x2);
	}
	static bool equal(const TKEY & x1, const TKEY & x2)
	{
		return x1 == x2;
	}
	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const TKEY & x1, const TKEY & x2, PRED pr) const
	{
		return (hv1 == hv2) && pr.equal(x1, x2);
	}
	struct node
	{
		unsigned m_hVal;

		template <class PRED>
		unsigned get_hval(const TKEY & k, PRED pr)
		{
			return m_hVal;
		}
		void set_hval(unsigned hv)
		{
			m_hVal = hv;
		}
	};
};

template <class TKEY> struct _hash_comp : public hash_comp<TKEY> {};

#define DEFINE_HASH_(type) template <> struct _hash_comp<type> : public hash_comp_econ<type> {}

DEFINE_HASH_(char);
DEFINE_HASH_(int);
DEFINE_HASH_(long);

DEFINE_HASH_(unsigned char);
DEFINE_HASH_(unsigned int);
DEFINE_HASH_(unsigned long);

DEFINE_HASH_(void *);

#undef DEFINE_HASH_

template <>
struct _hash_comp<const char *> : public hash_comp<const char *>
{
	unsigned operator () (const char * x) const
	{
		unsigned sum = 0;
		const int LARGENUMBER = 6293815;
		unsigned multiple = LARGENUMBER;

		int index = 1;

		for( const char * s = x; *s; ++s )	{
			sum += multiple * (index++) * (*s);
			multiple *= LARGENUMBER;
		}
		return sum;
	}

	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const char * x1, const char * x2, PRED pr) const
	{
		return (hv1 == hv2) && !strcmp(x1, x2);
	}
};

template <>
struct _hash_comp<unsigned __int64> : public hash_comp_econ<unsigned __int64>
{
	unsigned operator () (unsigned __int64 x) const
	{
		return unsigned((x >> 32) + (x & 0xFFFFFFFF));
	}
	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const unsigned __int64 & x1, const unsigned __int64 & x2, PRED pr) const
	{
		return x1 == x2;
	}
};

template <>
struct _hash_comp<__int64> : public hash_comp_econ<__int64>
{
	unsigned operator () (__int64 x) const
	{
		return unsigned((x >> 32) + (x & 0xFFFFFFFF));
	}
	template <class PRED>
	bool match(unsigned hv1, unsigned hv2, const __int64 & x1, const __int64 & x2, PRED pr) const
	{
		return x1 == x2;
	}
};

} // namespace SCL

#endif // __SCL_HASHCOMP_H__

