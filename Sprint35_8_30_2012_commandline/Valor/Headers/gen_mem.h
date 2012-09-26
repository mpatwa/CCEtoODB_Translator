/***********************************************************
 *
 *  Module: gen_mem.h
 *
 *  Description:
 *
 *    This module includes declarations used by the gen_mem.c module.
 *
 *  Creation Date: 	18 Oct 93
 *  Author:		Ofer Shofman
 ***********************************************************/

#ifndef GEN_MEM_DEF
#define GEN_MEM_DEF

/* error codes (range : 1001 - 1999) */

#define E_GEN_MEM_INTERNAL 1001
#define E_GEN_MEM_ILL_BUF  1002
#define E_GEN_MEM_ILL_PNT  1003

/* Modes for allocation and free */

#define GEN_MEM_FAST_MODE	0
#define GEN_MEM_TABLE_MODE	1

typedef struct {
   int	alloc_num;
   int	alloc_size;
   int	highest_addr;
   int	table_alloc;
   int	serial;
   int  lfh_disable;
} gen_mem_info_struc;

/************* memcpy() &  memmove() can crash on bad data *************/
#define memcpy(_d, _s, _dim)  gen__mem_memcpy ((_d), (_s), (_dim))
#define memmove(_d, _s, _dim) gen__mem_memmove((_d), (_s), (_dim))

/* Macros used in calls to gen__mem */

#define MEMMOVE(s1,s2,n)  gen__mem_move((s1), (s2), (n))

/* change realloc so that it calls malloc if ptr is NULL */
/* this allows portability across to SOLARIS_1 platforms */

#define MALLOC(size) \
   ((void *) gen__mem_malloc((size),GEN_MEM_TABLE_MODE,__FILE__,__LINE__))
#define CALLOC(nelem,size) \
   ((void *) gen__mem_calloc((nelem),(size),GEN_MEM_TABLE_MODE,__FILE__,__LINE__))
#define CALLOC_CHECK(p,n,_err)		 CALLOC_CHECK2(p,n,_err,)
#define CALLOC_CHECK2(p,n,_err,_act_err) { ASSERT(n > 0, _act_err; return _err); \
					   p = CALLOC(n, sizeof(*(p))); \
					   ASSERT(p != NULL, _act_err; return _err); }

#define FREE(p) \
   gen__mem_free((p),GEN_MEM_TABLE_MODE,__FILE__,__LINE__)
#define REALLOC(p,size) \
   ((void *) gen__mem_realloc((p),(size),GEN_MEM_TABLE_MODE,__FILE__,__LINE__))
#define NALLOC(p,num,alloc) if (num > 0) { p = REALLOC(p, (num)*sizeof(*(p))); alloc = num; }
#define COND_FREE(p) do { if ((p)!= NULL) { FREE((p)); p=NULL; } } while(0)

#define FMALLOC(size) \
   ((void *) gen__mem_malloc((size),GEN_MEM_FAST_MODE,__FILE__,__LINE__))
#define FCALLOC(nelem,size) \
   ((void *) gen__mem_calloc((nelem),(size),GEN_MEM_FAST_MODE,__FILE__,__LINE__))
#define FFREE(p) \
   gen__mem_free((p),GEN_MEM_FAST_MODE,__FILE__,__LINE__)
#define FREALLOC(p,size) \
   ((void *) gen__mem_realloc((p),(size),GEN_MEM_FAST_MODE,__FILE__,__LINE__))
#define FCOND_FREE(p) do { if ((p)!= NULL) { FFREE((p)); p=NULL; } } while(0)

#define GROW_ALLOC(_buf, _size, _err, _act_err) \
   do { void *_new_buf = REALLOC((_buf), (_size)); \
     ASSERT(L_PTR(_new_buf), _act_err; return (_err); ); \
     (_buf) = _new_buf; } while(0)

#define ADD_ALLOC_F(_add,_alloc) MAX(_add, MIN(_alloc*2, 1000000))

#define EXTEND_ALLOC(_buf,_num,_alloc,_add,_err,_act_err) \
   do { if ((_num) >= (_alloc)) { \
     int _true_add  = ADD_ALLOC_F(_add, _alloc); \
     int _new_alloc = (_alloc) + _true_add; \
     GROW_ALLOC(_buf, _new_alloc * sizeof(*(_buf)), _err, _act_err); \
     (_alloc) = _new_alloc; \
   } } while(0)

#define EXTEND_ALLOC_S(_struc, _buf, _num, _alloc,_add,_err,_act_err) \
   EXTEND_ALLOC((_struc)._buf, (_struc)._num, (_struc)._alloc, \
                 _add, _err, _act_err)

#define EXTEND_ALLOC_SP(_struc, _buf, _num, _alloc,_add,_err,_act_err) \
   EXTEND_ALLOC((_struc)->_buf, (_struc)->_num, (_struc)->_alloc, \
                 _add, _err, _act_err)

#define EXTEND_ALLOC_M(_buf, _num, _alloc, _add,_err, _act_err, _type) \
   do { if ((_num) >= (_alloc)) { \
     int _new_alloc = (_alloc) + (_add); \
     _type *_new_buf = (_type *)REALLOC((_buf), _new_alloc * sizeof(*(_buf))); \
     ASSERT(L_PTR(_new_buf), _act_err; return (_err);); \
     (_alloc) = _new_alloc; \
     (_buf)   = _new_buf; \
   } } while(0)

#define EXTEND_ALLOC_S_M(_struc, _buf, _num, _alloc,_add,_err,_act_err, _type) \
   EXTEND_ALLOC_M((_struc)._buf, (_struc)._num, (_struc)._alloc, \
                 _add, _err, _act_err, _type)

#define EXTEND_ALLOC_SP_M(_struc, _buf, _num, _alloc,_add,_err,_act_err, _type) \
   EXTEND_ALLOC_M((_struc)->_buf, (_struc)->_num, (_struc)->_alloc, \
                 _add, _err, _act_err, _type)

/* MIF 19 July 1998 - Macro definitions for common vector package (dynamic array)
   NOTE : you cannot use GEN_VECTOR_ALLOC if there is any special clearence requied before return from the function
*/

#define DEFINE_GEN_VECTOR(_data_type) \
   struct {		\
      _data_type *data;	\
      int num, alloc;	\
   }
/* Methods (no *.c file */
#define GEN_VECTOR_INIT(_vec)  MEMSETP(_vec)
#define GEN_VECTOR_FREE(_vec)  do { COND_FREE((_vec)->data); GEN_VECTOR_INIT (_vec); } while (0)
#define GEN_VECTOR_ALLOC(_vec,_add) EXTEND_ALLOC_SP(_vec, data, num + _add, alloc, _add, E_GEN_MEM_INTERNAL, ;);
#define GEN_VECTOR_IS_INDEX(_vec,_i)  IS_BOUNDED(_i, (_vec)->num)

typedef enum {
    GEN_MEM_FAILURE_NONE = 0,
    GEN_MEM_FAILURE_ALLOC,
    GEN_MEM_FAILURE_SENTINEL
} gen_mem_failure_enum;

typedef struct {
   void* (*malloc_cb) (int size, int mode, const char *file, int line);
   void* (*calloc_cb) (int nelem, int size, int mode, const char *file, int line);
   void* (*realloc_cb) (void *p, int size, int mode, const char *file, int line);
   void  (*free_cb) (void *p, int mode, const char *file, int line);
   int   (*link_add_cb) (void *user_p);
   int   (*link_get_cb) (void *user_p, int *link_count_p);
} memory_manager_struc;

#endif
