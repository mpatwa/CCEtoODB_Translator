/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SDM_SCL_ASSERT_H__
#define __SDM_SCL_ASSERT_H__

#undef SCL_ASSERT_WRN 
#undef SDM_BREAK

#if defined(PRODUCTION_RELEASE) 
# define NO_SCL_ASSERTS
#endif

#if !defined(PRODUCTION_RELEASE)
# if !defined(UNIX) && !defined(unix)
#  define SDM_BREAK() _asm { int 3 }
# else
#  define SDM_BREAK() (void)0
# endif
#else
# define SDM_BREAK()
#endif

int SYSCORE_API SDM_AssertFailed(const char * msg, const char * fname, const int lineno);

#undef SCL_ASSERT

#ifdef NO_SCL_ASSERTS
# define SCL_ASSERT_MSG(cond,msg)
#else
# define SCL_ASSERT_MSG(cond,msg) {if ( !(cond) && !SDM_AssertFailed(msg, __FILE__, __LINE__) ) SDM_BREAK();}
#endif

#define SCL_ASSERT(cond) SCL_ASSERT_MSG(cond, "")

#endif // __SDM_SCL_ASSERT_H__

