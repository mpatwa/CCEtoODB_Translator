/*
 Prototypes for file gen_mem
 */ 

#undef EXTERN
#if (defined __GEN_MEM_C__) && (!defined EXT_PROTOTYPE)
#define EXTERN
#include "gen_mem.ps"
#else
#define EXTERN extern
#endif

#undef EXTERN_DLL
#ifdef USEDLL
#if (defined CORETRILOGY_DLL) || (defined SAME_DLL)
#define EXTERN_DLL __declspec(dllexport)
#else
#define EXTERN_DLL __declspec(dllimport)
#endif
#else
#define EXTERN_DLL EXTERN
#endif

/*    PUBLIC DECLARATIONS    */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
EXTERN_DLL int gen__mem_set_external_manager (memory_manager_struc *ext_mem_manager)
;
EXTERN_DLL void *gen__mem_malloc (int size, int mode, const char *file, int line)
;
EXTERN_DLL void *gen__mem_calloc (int nelem, int size, int mode, const char *file, int line)
;
EXTERN_DLL void *gen__mem_realloc (void *p, int size, int mode, const char *file, int line)
;
EXTERN_DLL void gen__mem_free (void *p, int mode, const char *file, int line)
;
EXTERN_DLL int gen__mem_link_add (void *user_p)
;
EXTERN_DLL int gen__mem_link_get (void *user_p, int *link_count_p)
;
EXTERN void gen__mem_set_failure_code (boolean set)
;
EXTERN int gen__mem_get_failure_code (gen_mem_failure_enum *code)
;
EXTERN_DLL int gen__mem_move (void *s1, void *s2, int n)
;
EXTERN_DLL void gen__mem_mark(boolean mark)
;
EXTERN_DLL int gen__mem_log (void)
;
EXTERN int gen__mem_log_serial(int low_serial, int high_serial)
;
EXTERN_DLL int gen__mem_report (const char *file_name)
;
EXTERN_DLL void gen__mem_print_info(void)
;
EXTERN_DLL void mem_log_summary(char *buf)
;
EXTERN_DLL void gen__mem_print_info2(gen_mem_info_struc *mem_info)
;
EXTERN_DLL int gen__mem_get_info(gen_mem_info_struc *mem_info_p)
;
EXTERN int gen__mem_free_malloc_buf (/*gen_mem_info_struc *mem_info_p*/)
;
EXTERN void gen__mem_memorymap(void)
;
EXTERN_DLL int gen__mem_validate(void)
;
EXTERN_DLL void gen__mem_set_lfh_flag(int lfh_disable)
;
EXTERN int gen__mem_is_heap_ok ()
;
EXTERN void gen__mem_set_heap_size(char *init_heap_size_str)
;
EXTERN_DLL int gen__mem_heap_compact()
;
EXTERN_DLL void *gen__mem_memcpy (void *dest, const void *src, int size)
;
EXTERN_DLL void *gen__mem_memmove (void *dest, const void *src, int size)
;
#undef EXTERN_DLL

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

