// $Header: /CAMCAD/4.6/UltimateGrid2005/UG64Bit.h 1     5/08/06 8:28p Kurt Van Ness $

#if _MSC_VER < 1400

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GetClassLongPtr
#define GetClassLongPtr GetClassLong
#endif

#ifndef SetClassLongPtr
#define SetClassLongPtr SetClassLong
#endif

#ifndef ULongToPtr
#define ULongToPtr(x) x
#endif

#ifndef LongToPtr
#define LongToPtr(x) x
#endif

#ifndef PtrToUint
#define PtrToUint(x) x
#endif

#ifndef PtrToInt
#define PtrToInt(x) x
#endif

typedef DWORD DWORD_PTR;
typedef LONG LONG_PTR;
typedef ULONG ULONG_PTR;

// Frustratingly, INT_PTR is an int in VC2005, and a long in VC6, so we can't define it here, the code needs to just handle it elsewhere

#endif