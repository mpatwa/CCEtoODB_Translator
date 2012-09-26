/*
 Prototypes for file gen_log
 */ 

#undef EXTERN
#if (defined __GEN_LOG_C__) && (!defined EXT_PROTOTYPE)
#define EXTERN
#include "gen_log.ps"
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
EXTERN int gen__log_set_vars (LibArg *args, int num_args)
;
EXTERN_DLL int gen__log_set_version
   (int ver_major,
    int ver_minor,
    int ver_local,
    const char *asc_date,
    const char *asc_time,
    const char *asc_patch_id,
    const char *asc_osname)
;
EXTERN_DLL int gen__log_set_login_name(const char *login_name)
;
EXTERN int gen__log_close(char *fname)
;
EXTERN int gen__log_disable (void)
;
EXTERN int gen__log_reset_fd (void)
;
EXTERN char *gen__log_get_path(void)
;
EXTERN int gen__log_open
   (char *log_name, char *proc_name, int proc_pid,
    char *login_name, int shm_key)
;
EXTERN int gen__log_open_with_mode
   (char *log_name, char *proc_name, int proc_pid,
    char *login_name, int shm_key, int mode)
;
EXTERN_DLL void gen__log_set_log_cb(void (* log_cb_p)(log_severity_enum severity, const char *context, char *fmt, va_list args))
;
EXTERN_DLL int gen__log_write_buffer(char *fmt, ...)
;
EXTERN_DLL int gen__log_write_severity(log_severity_enum severity, char *fmt, ...)
;
EXTERN_DLL int gen__log_write_context(log_severity_enum severity, const char *context, char *fmt, ...)
;
EXTERN_DLL int gen__log_write(char *log_msg_type, char *fmt, ...)
;
EXTERN_DLL int gen__log_write_args(log_severity_enum severity, char *fmt, va_list args)
;
EXTERN_DLL void gen__log_disable_stdout(void)
;
EXTERN_DLL void gen__log_enable_stdout(void)
;
EXTERN_DLL int gen__log_write_c(msg_filter_struc *mfilter, char *message, ...)
;
EXTERN_DLL int gen__log_report_c(msg_filter_struc *mfilter)
;
EXTERN void gen__log_format_message(const char *str1, const char *str2, char *hdr_buf, int *len)
;
EXTERN_DLL boolean gen__log_show_progress(int num, int sample, char *objects, int total, char *extra, ...)
;
EXTERN_DLL void gen__log_write_override(char *fmt, ...)
;
EXTERN_DLL boolean gen__log_print_debug(char *source)
;
EXTERN_DLL void status_check(const char *file, int line)
;
#undef EXTERN_DLL

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

