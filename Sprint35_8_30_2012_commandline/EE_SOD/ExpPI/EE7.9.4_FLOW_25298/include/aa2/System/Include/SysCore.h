/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SYSCORE_H__
#define __SYSCORE_H__

#if !defined(UNIX)
# ifdef SYSCORE_EXPORTS
#  define SYSCORE_API __declspec(dllexport)
# else
#  define SYSCORE_API __declspec(dllimport)
# endif
#else 
# define SYSCORE_API  
#endif // !UNIX

#ifdef _MSC_VER
# pragma warning(disable:4035)
# pragma warning(disable:4786)
# pragma warning(disable:4251)
# pragma warning(disable:4275)
#endif 

#include "../compatibility/compat.h"

#include "../Core/SCL_Utils.h"

#include "../Core/SCL_Assert.h"

#include "../Core/SCL_Allocators.h"

#include "../Core/SCL_Functional.h"

#include "../Core/SCL_Event.h"
#include "../Core/SCL_NullNotifier.h"

#include "../Core/SCL_Pointer.h"
#include "../Core/SCL_String.h"

#include "../Core/SCL_Pair.h"
#include "../Core/SCL_DList.h"
#include "../Core/SCL_CList.h"
#include "../Core/SCL_BitSet.h"
#include "../Core/SCL_Vector.h"
#include "../Core/SCL_HashMap.h"
#include "../Core/SCL_HashMultiMap.h"
#include "../Core/SCL_HashTable.h"
#include "../Core/SCL_Tree.h"
#include "../Core/SCL_Set.h"
#include "../Core/SCL_MultiSet.h"
#include "../Core/SCL_Map.h"
#include "../Core/SCL_MultiMap.h"

#include "../Core/SCL_FilePath.h"
#include "../Core/SCL_File.h"

#endif // __SYSCORE_H__

