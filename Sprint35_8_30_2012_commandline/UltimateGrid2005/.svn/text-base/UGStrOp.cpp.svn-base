// $Header: /CAMCAD/4.6/UltimateGrid2005/UGStrOp.cpp 1     5/08/06 8:28p Kurt Van Ness $

#include "stdafx.h"
#include "UGStrOp.h"

#include <stdio.h>
#include <stdarg.h>

void UGStr::tcscpy(TCHAR * dest, SIZE_T length, const TCHAR* src)
{
#if _MSC_VER >= 1400
	::_tcscpy_s(dest, length, src);
#else
	UNREFERENCED_PARAMETER(length);
	::_tcscpy(dest,src);
# endif
}

char* UGStr::fcvt(double val, int count, int * dec, int * sign)
{
#if _MSC_VER >= 1400
	UNREFERENCED_PARAMETER(sign);
	char * dest = new char[count * 2 + 10];
	::_fcvt_s(dest, count * 2 + 9, val, count, dec, sign);
	return dest;
#else
	return ::_fcvt(val, count, dec, sign);
# endif		
}

void UGStr::strncpy(char * dest, size_t size, const char * src, size_t count)
{
#if _MSC_VER >= 1400
	::strncpy_s(dest, size, src, count);
#else
	UNREFERENCED_PARAMETER(size);
	::strncpy(dest, src, count);
# endif
}

void UGStr::stprintf(TCHAR * dest, size_t size, const TCHAR * src, ...)
{
	va_list vl;
	va_start( vl, src );

#ifdef _UNICODE
#if _MSC_VER >= 1400
	_vswprintf_s_l(dest, size, src, NULL, vl);
#else
	UNREFERENCED_PARAMETER(size);
	_vstprintf(dest, src, vl);
# endif
#else
#if _MSC_VER >= 1400
    _vsprintf_s_l(dest, size, src, NULL, vl);
#else
	UNREFERENCED_PARAMETER(size);
	_vstprintf(dest, src, vl);
# endif
#endif

	va_end( vl );
}

void UGStr::tcscat(TCHAR * dest, SIZE_T length, const TCHAR* src)
{
#if _MSC_VER >= 1400
	_tcscat_s(dest, length, src);
#else
	UNREFERENCED_PARAMETER(length);
	_tcscat(dest, src);
# endif
}

void UGStr::tcsncat(TCHAR * dest, size_t size, const TCHAR * src, size_t count)
{
#if _MSC_VER >= 1400
	_tcsncat_s(dest, size, src, count);
#else
	UNREFERENCED_PARAMETER(size);
	_tcsncat(dest, src, count);
# endif
}
