/*
 Prototypes for file gen_fs
 */ 

#undef EXTERN
#if (defined __GEN_FS_C__) && (!defined EXT_PROTOTYPE)
#define EXTERN
#include "gen_fs.ps"
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
EXTERN_DLL int gen__fs_set_vars (LibArg *args, int num_args)
;
EXTERN_DLL int gen__fs_file_size (const char *path)
;
EXTERN_DLL int gen__fs_file_type (const char *path)
;
EXTERN_DLL int gen__fs_file_read (char *path, char **text_p, int *size_p)
;
EXTERN int gen__fs_file_write (char *path, char *text, int size)
;
EXTERN int gen__fs_fs_size(char *path, int *used, int *empty)
;
EXTERN_DLL int gen__fs_copy_dir (char *path1, char *path2)
;
EXTERN int gen__fs_copy_wild (char *path1, char *path2, char *wild, int *fnum)
;
EXTERN_DLL int gen__fs_copy_file (char *path1, char *path2)
;
EXTERN_DLL int gen__fs_comp_files (char *path1, char *path2)
;
EXTERN int gen__fs_merge_dirs (char *dir_path1, char *dir_path2)
;
EXTERN_DLL int gen__fs_deldir (char *path, int mode)
;
EXTERN_DLL int gen__fs_deldir_silent (char *path, int mode)
;
EXTERN_DLL int gen__fs_rename (char *path1, char *path2)
;
EXTERN int gen__fs_dir_size (char *path, int *ret_size, int *ret_checksum)
;
EXTERN_DLL int gen__fs_dir_time (char *path, int mode, int *ret_oldt, int *ret_newt)
;
EXTERN_DLL int gen__fs_create_dir (const char *path,
			ushort in_mode,
			boolean leaf_file,
			int *ret_fd)
;
EXTERN_DLL int gen__fs_file_checksum (char *path, int *ret_checksum)
;
EXTERN int gen__fs_buffer_checksum (void *p, int n)
;
EXTERN_DLL int gen__fs_gzip_file (const char *path1, const char *path2,
		       boolean *zipped_p)
;
EXTERN_DLL int gen__fs_compress_file (char *path1, char *path2,
			   boolean *compress_p)
;
EXTERN_DLL int gen__fs_uncompress_file(char *path1, char *path2)
;
EXTERN_DLL int gen__fs_gunzip_file(const char *path1, char *path2)
;
EXTERN_DLL int gen__fs_expand_file(const char *path1, char *path2)
;
EXTERN_DLL int gen__fs_tar (char *from_path, char *to_path,
		 tar_mode_enum tar_mode,
		 tar_opt_enum  tar_opt)
;
EXTERN_DLL FILE *gen__fs_fopen(const char *path, const char *mode, char *file, int line)
;
EXTERN_DLL int gen__fs_fclose(FILE *fp, char *file, int line)
;
EXTERN void gen__fs_fclose_all(void)
;
EXTERN_DLL int gen__fs_update_entry(FILE *fp,
			 char *dir,
			 char *file)
;
EXTERN_DLL int gen__fs_get_entry(FILE *fp,
		      char *dir,
		      char *file,
		      char *mode)
;
EXTERN int gen__fs_find (char *path, char *name, char *npath)
;
EXTERN_DLL int gen__fs_dir (const char *path, char *name, char ***list, int *num_list, int type)
;
EXTERN_DLL int gen__fs_tempname (const char *dir_path, char *prefix, char *tempname_p)
;
EXTERN int gen__fs_tempname_match (char *file, boolean *is_temp_p)
;
EXTERN_DLL int gen__fs_temp_delete (const char *dir_path)
;
EXTERN int gen__fs_temp_delete_force (const char *dir_path, boolean force)
;
EXTERN int gen__fs_temp_delete_silent (const char *dir_path)
;
EXTERN_DLL int gen__fs_parse_dir
   (char *path,
    gen_fs_parse_dir_struc **parse_dir_p, int *parse_dir_len_p)
;
EXTERN_DLL int gen__fs_parse_dir_ml
   (char *path, int limit,
    gen_fs_parse_dir_struc **parse_dir_p, int *parse_dir_len_p)
;
EXTERN_DLL int gen__fs_parse_dir_get_path
   (gen_fs_parse_dir_struc *parse_dir, int parse_dir_len, int nf,
    char **path_p)
;
EXTERN int gen__fs_is_ascii_buf (char     *buf,
			  int	    buf_size,
		          boolean  *is_ascii)
;
EXTERN int gen__fs_is_ascii_file (char     *path,
		           boolean  *is_ascii)
;
EXTERN int gen__fs_binary_dump (char *path, int start_char, int end_char,
			 int line_width, char **fbuf_p,
			 int *max_line_length_p)
;
EXTERN int gen__fs_same (char *path1, char *path2, boolean *same_p)
;
EXTERN_DLL int gen__fs_val_filename (char *filename)
;
EXTERN_DLL char *gen__fs_getcwd (void)
;
EXTERN_DLL int gen__fs_read_line (FILE *fp, char **line_p, int *len_p)
;
EXTERN_DLL boolean gen__fs_access_lwr_modify(char *path, int amode)
;
EXTERN_DLL int gen__fs_encode64(char *path, char **encoded_file_p)
;
EXTERN int gen__fs_decode64(char *path, char **decoded_file_p,
		     int *out_len_p)
;
EXTERN int gen__fs_set_init_path (void)
;
EXTERN int gen__fs_get_init_path (char *path)
;
EXTERN_DLL int gen__fs_get_dir_datesum(char* path, int* datesum_p)
;
EXTERN_DLL int gen__fs_read_protected_file (int     file_type,
				 int     decomp_buffer,
				 int     deformat_buffer,
				 char    *file_name,
				 char    **inp_str,
				 int     *inp_str_len,
				 boolean *is_protected)
;
EXTERN_DLL int gen__fs_write_protected_file (int  file_type,
				 int  compress_buffer,
				 int  format_buffer,
				 char *out_str,
				 int  out_str_len,
				 char *file_name,
				 int  compress_file)
;
#undef EXTERN_DLL

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

