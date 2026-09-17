/*
** 2006 June 7
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the SQLite interface for use by
** shared libraries that want to be imported as extensions into
** an SQLite instance.  Shared libraries that intend to be loaded
** as extensions by SQLite should #include this file instead of 
** sqlite3.h.
*/
#ifndef SQLITE3EXT_H
#define SQLITE3EXT_H
#include "sqlite3.h"

/*
** The following structure holds pointers to all of the SQLite API
** routines.
**
** WARNING:  In order to maintain backwards compatibility, add new
** interfaces to the end of this structure only.  If you insert new
** interfaces in the middle of this structure, then older different
** versions of SQLite will not be able to load each other's shared
** libraries!
*/
struct mc_sqlite3_api_routines {
  void * (*aggregate_context)(mc_sqlite3_context*,int nBytes);
  int  (*aggregate_count)(mc_sqlite3_context*);
  int  (*bind_blob)(mc_sqlite3_stmt*,int,const void*,int n,void(*)(void*));
  int  (*bind_double)(mc_sqlite3_stmt*,int,double);
  int  (*bind_int)(mc_sqlite3_stmt*,int,int);
  int  (*bind_int64)(mc_sqlite3_stmt*,int,sqlite_int64);
  int  (*bind_null)(mc_sqlite3_stmt*,int);
  int  (*bind_parameter_count)(mc_sqlite3_stmt*);
  int  (*bind_parameter_index)(mc_sqlite3_stmt*,const char*zName);
  const char * (*bind_parameter_name)(mc_sqlite3_stmt*,int);
  int  (*bind_text)(mc_sqlite3_stmt*,int,const char*,int n,void(*)(void*));
  int  (*bind_text16)(mc_sqlite3_stmt*,int,const void*,int,void(*)(void*));
  int  (*bind_value)(mc_sqlite3_stmt*,int,const mc_sqlite3_value*);
  int  (*busy_handler)(mc_sqlite3*,int(*)(void*,int),void*);
  int  (*busy_timeout)(mc_sqlite3*,int ms);
  int  (*changes)(mc_sqlite3*);
  int  (*close)(mc_sqlite3*);
  int  (*collation_needed)(mc_sqlite3*,void*,void(*)(void*,mc_sqlite3*,
                           int eTextRep,const char*));
  int  (*collation_needed16)(mc_sqlite3*,void*,void(*)(void*,mc_sqlite3*,
                             int eTextRep,const void*));
  const void * (*column_blob)(mc_sqlite3_stmt*,int iCol);
  int  (*column_bytes)(mc_sqlite3_stmt*,int iCol);
  int  (*column_bytes16)(mc_sqlite3_stmt*,int iCol);
  int  (*column_count)(mc_sqlite3_stmt*pStmt);
  const char * (*column_database_name)(mc_sqlite3_stmt*,int);
  const void * (*column_database_name16)(mc_sqlite3_stmt*,int);
  const char * (*column_decltype)(mc_sqlite3_stmt*,int i);
  const void * (*column_decltype16)(mc_sqlite3_stmt*,int);
  double  (*column_double)(mc_sqlite3_stmt*,int iCol);
  int  (*column_int)(mc_sqlite3_stmt*,int iCol);
  sqlite_int64  (*column_int64)(mc_sqlite3_stmt*,int iCol);
  const char * (*column_name)(mc_sqlite3_stmt*,int);
  const void * (*column_name16)(mc_sqlite3_stmt*,int);
  const char * (*column_origin_name)(mc_sqlite3_stmt*,int);
  const void * (*column_origin_name16)(mc_sqlite3_stmt*,int);
  const char * (*column_table_name)(mc_sqlite3_stmt*,int);
  const void * (*column_table_name16)(mc_sqlite3_stmt*,int);
  const unsigned char * (*column_text)(mc_sqlite3_stmt*,int iCol);
  const void * (*column_text16)(mc_sqlite3_stmt*,int iCol);
  int  (*column_type)(mc_sqlite3_stmt*,int iCol);
  mc_sqlite3_value* (*column_value)(mc_sqlite3_stmt*,int iCol);
  void * (*commit_hook)(mc_sqlite3*,int(*)(void*),void*);
  int  (*complete)(const char*sql);
  int  (*complete16)(const void*sql);
  int  (*create_collation)(mc_sqlite3*,const char*,int,void*,
                           int(*)(void*,int,const void*,int,const void*));
  int  (*create_collation16)(mc_sqlite3*,const void*,int,void*,
                             int(*)(void*,int,const void*,int,const void*));
  int  (*create_function)(mc_sqlite3*,const char*,int,int,void*,
                          void (*xFunc)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                          void (*xStep)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                          void (*xFinal)(mc_sqlite3_context*));
  int  (*create_function16)(mc_sqlite3*,const void*,int,int,void*,
                            void (*xFunc)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void (*xStep)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void (*xFinal)(mc_sqlite3_context*));
  int (*create_module)(mc_sqlite3*,const char*,const mc_sqlite3_module*,void*);
  int  (*data_count)(mc_sqlite3_stmt*pStmt);
  mc_sqlite3 * (*db_handle)(mc_sqlite3_stmt*);
  int (*declare_vtab)(mc_sqlite3*,const char*);
  int  (*enable_shared_cache)(int);
  int  (*errcode)(mc_sqlite3*db);
  const char * (*errmsg)(mc_sqlite3*);
  const void * (*errmsg16)(mc_sqlite3*);
  int  (*exec)(mc_sqlite3*,const char*,mc_sqlite3_callback,void*,char**);
  int  (*expired)(mc_sqlite3_stmt*);
  int  (*finalize)(mc_sqlite3_stmt*pStmt);
  void  (*free)(void*);
  void  (*free_table)(char**result);
  int  (*get_autocommit)(mc_sqlite3*);
  void * (*get_auxdata)(mc_sqlite3_context*,int);
  int  (*get_table)(mc_sqlite3*,const char*,char***,int*,int*,char**);
  int  (*global_recover)(void);
  void  (*interruptx)(mc_sqlite3*);
  sqlite_int64  (*last_insert_rowid)(mc_sqlite3*);
  const char * (*libversion)(void);
  int  (*libversion_number)(void);
  void *(*malloc)(int);
  char * (*mprintf)(const char*,...);
  int  (*open)(const char*,mc_sqlite3**);
  int  (*open16)(const void*,mc_sqlite3**);
  int  (*prepare)(mc_sqlite3*,const char*,int,mc_sqlite3_stmt**,const char**);
  int  (*prepare16)(mc_sqlite3*,const void*,int,mc_sqlite3_stmt**,const void**);
  void * (*profile)(mc_sqlite3*,void(*)(void*,const char*,sqlite_uint64),void*);
  void  (*progress_handler)(mc_sqlite3*,int,int(*)(void*),void*);
  void *(*realloc)(void*,int);
  int  (*reset)(mc_sqlite3_stmt*pStmt);
  void  (*result_blob)(mc_sqlite3_context*,const void*,int,void(*)(void*));
  void  (*result_double)(mc_sqlite3_context*,double);
  void  (*result_error)(mc_sqlite3_context*,const char*,int);
  void  (*result_error16)(mc_sqlite3_context*,const void*,int);
  void  (*result_int)(mc_sqlite3_context*,int);
  void  (*result_int64)(mc_sqlite3_context*,sqlite_int64);
  void  (*result_null)(mc_sqlite3_context*);
  void  (*result_text)(mc_sqlite3_context*,const char*,int,void(*)(void*));
  void  (*result_text16)(mc_sqlite3_context*,const void*,int,void(*)(void*));
  void  (*result_text16be)(mc_sqlite3_context*,const void*,int,void(*)(void*));
  void  (*result_text16le)(mc_sqlite3_context*,const void*,int,void(*)(void*));
  void  (*result_value)(mc_sqlite3_context*,mc_sqlite3_value*);
  void * (*rollback_hook)(mc_sqlite3*,void(*)(void*),void*);
  int  (*set_authorizer)(mc_sqlite3*,int(*)(void*,int,const char*,const char*,
                         const char*,const char*),void*);
  void  (*set_auxdata)(mc_sqlite3_context*,int,void*,void (*)(void*));
  char * (*xsnprintf)(int,char*,const char*,...);
  int  (*step)(mc_sqlite3_stmt*);
  int  (*table_column_metadata)(mc_sqlite3*,const char*,const char*,const char*,
                                char const**,char const**,int*,int*,int*);
  void  (*thread_cleanup)(void);
  int  (*total_changes)(mc_sqlite3*);
  void * (*trace)(mc_sqlite3*,void(*xTrace)(void*,const char*),void*);
  int  (*transfer_bindings)(mc_sqlite3_stmt*,mc_sqlite3_stmt*);
  void * (*update_hook)(mc_sqlite3*,void(*)(void*,int ,char const*,char const*,
                                         sqlite_int64),void*);
  void * (*user_data)(mc_sqlite3_context*);
  const void * (*value_blob)(mc_sqlite3_value*);
  int  (*value_bytes)(mc_sqlite3_value*);
  int  (*value_bytes16)(mc_sqlite3_value*);
  double  (*value_double)(mc_sqlite3_value*);
  int  (*value_int)(mc_sqlite3_value*);
  sqlite_int64  (*value_int64)(mc_sqlite3_value*);
  int  (*value_numeric_type)(mc_sqlite3_value*);
  const unsigned char * (*value_text)(mc_sqlite3_value*);
  const void * (*value_text16)(mc_sqlite3_value*);
  const void * (*value_text16be)(mc_sqlite3_value*);
  const void * (*value_text16le)(mc_sqlite3_value*);
  int  (*value_type)(mc_sqlite3_value*);
  char *(*vmprintf)(const char*,va_list);
  /* Added ??? */
  int (*overload_function)(mc_sqlite3*, const char *zFuncName, int nArg);
  /* Added by 3.3.13 */
  int (*prepare_v2)(mc_sqlite3*,const char*,int,mc_sqlite3_stmt**,const char**);
  int (*prepare16_v2)(mc_sqlite3*,const void*,int,mc_sqlite3_stmt**,const void**);
  int (*clear_bindings)(mc_sqlite3_stmt*);
  /* Added by 3.4.1 */
  int (*create_module_v2)(mc_sqlite3*,const char*,const mc_sqlite3_module*,void*,
                          void (*xDestroy)(void *));
  /* Added by 3.5.0 */
  int (*bind_zeroblob)(mc_sqlite3_stmt*,int,int);
  int (*blob_bytes)(mc_sqlite3_blob*);
  int (*blob_close)(mc_sqlite3_blob*);
  int (*blob_open)(mc_sqlite3*,const char*,const char*,const char*,mc_sqlite3_int64,
                   int,mc_sqlite3_blob**);
  int (*blob_read)(mc_sqlite3_blob*,void*,int,int);
  int (*blob_write)(mc_sqlite3_blob*,const void*,int,int);
  int (*create_collation_v2)(mc_sqlite3*,const char*,int,void*,
                             int(*)(void*,int,const void*,int,const void*),
                             void(*)(void*));
  int (*file_control)(mc_sqlite3*,const char*,int,void*);
  mc_sqlite3_int64 (*memory_highwater)(int);
  mc_sqlite3_int64 (*memory_used)(void);
  mc_sqlite3_mutex *(*mutex_alloc)(int);
  void (*mutex_enter)(mc_sqlite3_mutex*);
  void (*mutex_free)(mc_sqlite3_mutex*);
  void (*mutex_leave)(mc_sqlite3_mutex*);
  int (*mutex_try)(mc_sqlite3_mutex*);
  int (*open_v2)(const char*,mc_sqlite3**,int,const char*);
  int (*release_memory)(int);
  void (*result_error_nomem)(mc_sqlite3_context*);
  void (*result_error_toobig)(mc_sqlite3_context*);
  int (*sleep)(int);
  void (*soft_heap_limit)(int);
  mc_sqlite3_vfs *(*vfs_find)(const char*);
  int (*vfs_register)(mc_sqlite3_vfs*,int);
  int (*vfs_unregister)(mc_sqlite3_vfs*);
  int (*xthreadsafe)(void);
  void (*result_zeroblob)(mc_sqlite3_context*,int);
  void (*result_error_code)(mc_sqlite3_context*,int);
  int (*test_control)(int, ...);
  void (*randomness)(int,void*);
  mc_sqlite3 *(*context_db_handle)(mc_sqlite3_context*);
  int (*extended_result_codes)(mc_sqlite3*,int);
  int (*limit)(mc_sqlite3*,int,int);
  mc_sqlite3_stmt *(*next_stmt)(mc_sqlite3*,mc_sqlite3_stmt*);
  const char *(*sql)(mc_sqlite3_stmt*);
  int (*status)(int,int*,int*,int);
  int (*backup_finish)(mc_sqlite3_backup*);
  mc_sqlite3_backup *(*backup_init)(mc_sqlite3*,const char*,mc_sqlite3*,const char*);
  int (*backup_pagecount)(mc_sqlite3_backup*);
  int (*backup_remaining)(mc_sqlite3_backup*);
  int (*backup_step)(mc_sqlite3_backup*,int);
  const char *(*compileoption_get)(int);
  int (*compileoption_used)(const char*);
  int (*create_function_v2)(mc_sqlite3*,const char*,int,int,void*,
                            void (*xFunc)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void (*xStep)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void (*xFinal)(mc_sqlite3_context*),
                            void(*xDestroy)(void*));
  int (*db_config)(mc_sqlite3*,int,...);
  mc_sqlite3_mutex *(*db_mutex)(mc_sqlite3*);
  int (*db_status)(mc_sqlite3*,int,int*,int*,int);
  int (*extended_errcode)(mc_sqlite3*);
  void (*log)(int,const char*,...);
  mc_sqlite3_int64 (*soft_heap_limit64)(mc_sqlite3_int64);
  const char *(*sourceid)(void);
  int (*stmt_status)(mc_sqlite3_stmt*,int,int);
  int (*strnicmp)(const char*,const char*,int);
  int (*unlock_notify)(mc_sqlite3*,void(*)(void**,int),void*);
  int (*wal_autocheckpoint)(mc_sqlite3*,int);
  int (*wal_checkpoint)(mc_sqlite3*,const char*);
  void *(*wal_hook)(mc_sqlite3*,int(*)(void*,mc_sqlite3*,const char*,int),void*);
  int (*blob_reopen)(mc_sqlite3_blob*,mc_sqlite3_int64);
  int (*vtab_config)(mc_sqlite3*,int op,...);
  int (*vtab_on_conflict)(mc_sqlite3*);
  /* Version 3.7.16 and later */
  int (*close_v2)(mc_sqlite3*);
  const char *(*db_filename)(mc_sqlite3*,const char*);
  int (*db_readonly)(mc_sqlite3*,const char*);
  int (*db_release_memory)(mc_sqlite3*);
  const char *(*errstr)(int);
  int (*stmt_busy)(mc_sqlite3_stmt*);
  int (*stmt_readonly)(mc_sqlite3_stmt*);
  int (*stricmp)(const char*,const char*);
  int (*uri_boolean)(const char*,const char*,int);
  mc_sqlite3_int64 (*uri_int64)(const char*,const char*,mc_sqlite3_int64);
  const char *(*uri_parameter)(const char*,const char*);
  char *(*xvsnprintf)(int,char*,const char*,va_list);
  int (*wal_checkpoint_v2)(mc_sqlite3*,const char*,int,int*,int*);
  /* Version 3.8.7 and later */
  int (*auto_extension)(void(*)(void));
  int (*bind_blob64)(mc_sqlite3_stmt*,int,const void*,mc_sqlite3_uint64,
                     void(*)(void*));
  int (*bind_text64)(mc_sqlite3_stmt*,int,const char*,mc_sqlite3_uint64,
                      void(*)(void*),unsigned char);
  int (*cancel_auto_extension)(void(*)(void));
  int (*load_extension)(mc_sqlite3*,const char*,const char*,char**);
  void *(*malloc64)(mc_sqlite3_uint64);
  mc_sqlite3_uint64 (*msize)(void*);
  void *(*realloc64)(void*,mc_sqlite3_uint64);
  void (*reset_auto_extension)(void);
  void (*result_blob64)(mc_sqlite3_context*,const void*,mc_sqlite3_uint64,
                        void(*)(void*));
  void (*result_text64)(mc_sqlite3_context*,const char*,mc_sqlite3_uint64,
                         void(*)(void*), unsigned char);
  int (*strglob)(const char*,const char*);
  /* Version 3.8.11 and later */
  mc_sqlite3_value *(*value_dup)(const mc_sqlite3_value*);
  void (*value_free)(mc_sqlite3_value*);
  int (*result_zeroblob64)(mc_sqlite3_context*,mc_sqlite3_uint64);
  int (*bind_zeroblob64)(mc_sqlite3_stmt*, int, mc_sqlite3_uint64);
  /* Version 3.9.0 and later */
  unsigned int (*value_subtype)(mc_sqlite3_value*);
  void (*result_subtype)(mc_sqlite3_context*,unsigned int);
  /* Version 3.10.0 and later */
  int (*status64)(int,mc_sqlite3_int64*,mc_sqlite3_int64*,int);
  int (*strlike)(const char*,const char*,unsigned int);
  int (*db_cacheflush)(mc_sqlite3*);
  /* Version 3.12.0 and later */
  int (*system_errno)(mc_sqlite3*);
  /* Version 3.14.0 and later */
  int (*trace_v2)(mc_sqlite3*,unsigned,int(*)(unsigned,void*,void*,void*),void*);
  char *(*expanded_sql)(mc_sqlite3_stmt*);
  /* Version 3.18.0 and later */
  void (*set_last_insert_rowid)(mc_sqlite3*,mc_sqlite3_int64);
  /* Version 3.20.0 and later */
  int (*prepare_v3)(mc_sqlite3*,const char*,int,unsigned int,
                    mc_sqlite3_stmt**,const char**);
  int (*prepare16_v3)(mc_sqlite3*,const void*,int,unsigned int,
                      mc_sqlite3_stmt**,const void**);
  int (*bind_pointer)(mc_sqlite3_stmt*,int,void*,const char*,void(*)(void*));
  void (*result_pointer)(mc_sqlite3_context*,void*,const char*,void(*)(void*));
  void *(*value_pointer)(mc_sqlite3_value*,const char*);
  int (*vtab_nochange)(mc_sqlite3_context*);
  int (*value_nochange)(mc_sqlite3_value*);
  const char *(*vtab_collation)(mc_sqlite3_index_info*,int);
  /* Version 3.24.0 and later */
  int (*keyword_count)(void);
  int (*keyword_name)(int,const char**,int*);
  int (*keyword_check)(const char*,int);
  mc_sqlite3_str *(*str_new)(mc_sqlite3*);
  char *(*str_finish)(mc_sqlite3_str*);
  void (*str_appendf)(mc_sqlite3_str*, const char *zFormat, ...);
  void (*str_vappendf)(mc_sqlite3_str*, const char *zFormat, va_list);
  void (*str_append)(mc_sqlite3_str*, const char *zIn, int N);
  void (*str_appendall)(mc_sqlite3_str*, const char *zIn);
  void (*str_appendchar)(mc_sqlite3_str*, int N, char C);
  void (*str_reset)(mc_sqlite3_str*);
  int (*str_errcode)(mc_sqlite3_str*);
  int (*str_length)(mc_sqlite3_str*);
  char *(*str_value)(mc_sqlite3_str*);
  /* Version 3.25.0 and later */
  int (*create_window_function)(mc_sqlite3*,const char*,int,int,void*,
                            void (*xStep)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void (*xFinal)(mc_sqlite3_context*),
                            void (*xValue)(mc_sqlite3_context*),
                            void (*xInv)(mc_sqlite3_context*,int,mc_sqlite3_value**),
                            void(*xDestroy)(void*));
  /* Version 3.26.0 and later */
  const char *(*normalized_sql)(mc_sqlite3_stmt*);
  /* Version 3.28.0 and later */
  int (*stmt_isexplain)(mc_sqlite3_stmt*);
  int (*value_frombind)(mc_sqlite3_value*);
  /* Version 3.30.0 and later */
  int (*drop_modules)(mc_sqlite3*,const char**);
  /* Version 3.31.0 and later */
  mc_sqlite3_int64 (*hard_heap_limit64)(mc_sqlite3_int64);
  const char *(*uri_key)(const char*,int);
  const char *(*filename_database)(const char*);
  const char *(*filename_journal)(const char*);
  const char *(*filename_wal)(const char*);
  /* Version 3.32.0 and later */
  const char *(*create_filename)(const char*,const char*,const char*,
                           int,const char**);
  void (*free_filename)(const char*);
  mc_sqlite3_file *(*database_file_object)(const char*);
  /* Version 3.34.0 and later */
  int (*txn_state)(mc_sqlite3*,const char*);
  /* Version 3.36.1 and later */
  mc_sqlite3_int64 (*changes64)(mc_sqlite3*);
  mc_sqlite3_int64 (*total_changes64)(mc_sqlite3*);
  /* Version 3.37.0 and later */
  int (*autovacuum_pages)(mc_sqlite3*,
     unsigned int(*)(void*,const char*,unsigned int,unsigned int,unsigned int),
     void*, void(*)(void*));
  /* Version 3.38.0 and later */
  int (*error_offset)(mc_sqlite3*);
  int (*vtab_rhs_value)(mc_sqlite3_index_info*,int,mc_sqlite3_value**);
  int (*vtab_distinct)(mc_sqlite3_index_info*);
  int (*vtab_in)(mc_sqlite3_index_info*,int,int);
  int (*vtab_in_first)(mc_sqlite3_value*,mc_sqlite3_value**);
  int (*vtab_in_next)(mc_sqlite3_value*,mc_sqlite3_value**);
  /* Version 3.39.0 and later */
  int (*deserialize)(mc_sqlite3*,const char*,unsigned char*,
                     mc_sqlite3_int64,mc_sqlite3_int64,unsigned);
  unsigned char *(*serialize)(mc_sqlite3*,const char *,mc_sqlite3_int64*,
                              unsigned int);
  const char *(*db_name)(mc_sqlite3*,int);
  /* Version 3.40.0 and later */
  int (*value_encoding)(mc_sqlite3_value*);
  /* Version 3.41.0 and later */
  int (*is_interrupted)(mc_sqlite3*);
  /* Version 3.43.0 and later */
  int (*stmt_explain)(mc_sqlite3_stmt*,int);
  /* Version 3.44.0 and later */
  void *(*get_clientdata)(mc_sqlite3*,const char*);
  int (*set_clientdata)(mc_sqlite3*, const char*, void*, void(*)(void*));
  /* Version 3.50.0 and later */
  int (*setlk_timeout)(mc_sqlite3*,int,int);
  /* Version 3.51.0 and later */
  int (*set_errmsg)(mc_sqlite3*,int,const char*);
  int (*db_status64)(mc_sqlite3*,int,mc_sqlite3_int64*,mc_sqlite3_int64*,int);
  /* Version 3.52.0 and later */
  void (*str_truncate)(mc_sqlite3_str*,int);
  void (*str_free)(mc_sqlite3_str*);
  int (*carray_bind)(mc_sqlite3_stmt*,int,void*,int,int,void(*)(void*));
  int (*carray_bind_v2)(mc_sqlite3_stmt*,int,void*,int,int,void(*)(void*),void*);
};

/*
** This is the function signature used for all extension entry points.  It
** is also defined in the file "loadext.c".
*/
typedef int (*mc_sqlite3_loadext_entry)(
  mc_sqlite3 *db,                       /* Handle to the database. */
  char **pzErrMsg,                   /* Used to set error string on failure. */
  const mc_sqlite3_api_routines *pThunk /* Extension API function pointers. */
);

/*
** The following macros redefine the API routines so that they are
** redirected through the global mc_sqlite3_api structure.
**
** This header file is also used by the loadext.c source file
** (part of the main SQLite library - not an extension) so that
** it can get access to the mc_sqlite3_api_routines structure
** definition.  But the main library does not want to redefine
** the API.  So the redefinition macros are only valid if the
** SQLITE_CORE macros is undefined.
*/
#if !defined(SQLITE_CORE) && !defined(SQLITE_OMIT_LOAD_EXTENSION)
#define mc_sqlite3_aggregate_context      mc_sqlite3_api->aggregate_context
#ifndef SQLITE_OMIT_DEPRECATED
#define mc_sqlite3_aggregate_count        mc_sqlite3_api->aggregate_count
#endif
#define mc_sqlite3_bind_blob              mc_sqlite3_api->bind_blob
#define mc_sqlite3_bind_double            mc_sqlite3_api->bind_double
#define mc_sqlite3_bind_int               mc_sqlite3_api->bind_int
#define mc_sqlite3_bind_int64             mc_sqlite3_api->bind_int64
#define mc_sqlite3_bind_null              mc_sqlite3_api->bind_null
#define mc_sqlite3_bind_parameter_count   mc_sqlite3_api->bind_parameter_count
#define mc_sqlite3_bind_parameter_index   mc_sqlite3_api->bind_parameter_index
#define mc_sqlite3_bind_parameter_name    mc_sqlite3_api->bind_parameter_name
#define mc_sqlite3_bind_text              mc_sqlite3_api->bind_text
#define mc_sqlite3_bind_text16            mc_sqlite3_api->bind_text16
#define mc_sqlite3_bind_value             mc_sqlite3_api->bind_value
#define mc_sqlite3_busy_handler           mc_sqlite3_api->busy_handler
#define mc_sqlite3_busy_timeout           mc_sqlite3_api->busy_timeout
#define mc_sqlite3_changes                mc_sqlite3_api->changes
#define mc_sqlite3_close                  mc_sqlite3_api->close
#define mc_sqlite3_collation_needed       mc_sqlite3_api->collation_needed
#define mc_sqlite3_collation_needed16     mc_sqlite3_api->collation_needed16
#define mc_sqlite3_column_blob            mc_sqlite3_api->column_blob
#define mc_sqlite3_column_bytes           mc_sqlite3_api->column_bytes
#define mc_sqlite3_column_bytes16         mc_sqlite3_api->column_bytes16
#define mc_sqlite3_column_count           mc_sqlite3_api->column_count
#define mc_sqlite3_column_database_name   mc_sqlite3_api->column_database_name
#define mc_sqlite3_column_database_name16 mc_sqlite3_api->column_database_name16
#define mc_sqlite3_column_decltype        mc_sqlite3_api->column_decltype
#define mc_sqlite3_column_decltype16      mc_sqlite3_api->column_decltype16
#define mc_sqlite3_column_double          mc_sqlite3_api->column_double
#define mc_sqlite3_column_int             mc_sqlite3_api->column_int
#define mc_sqlite3_column_int64           mc_sqlite3_api->column_int64
#define mc_sqlite3_column_name            mc_sqlite3_api->column_name
#define mc_sqlite3_column_name16          mc_sqlite3_api->column_name16
#define mc_sqlite3_column_origin_name     mc_sqlite3_api->column_origin_name
#define mc_sqlite3_column_origin_name16   mc_sqlite3_api->column_origin_name16
#define mc_sqlite3_column_table_name      mc_sqlite3_api->column_table_name
#define mc_sqlite3_column_table_name16    mc_sqlite3_api->column_table_name16
#define mc_sqlite3_column_text            mc_sqlite3_api->column_text
#define mc_sqlite3_column_text16          mc_sqlite3_api->column_text16
#define mc_sqlite3_column_type            mc_sqlite3_api->column_type
#define mc_sqlite3_column_value           mc_sqlite3_api->column_value
#define mc_sqlite3_commit_hook            mc_sqlite3_api->commit_hook
#define mc_sqlite3_complete               mc_sqlite3_api->complete
#define mc_sqlite3_complete16             mc_sqlite3_api->complete16
#define mc_sqlite3_create_collation       mc_sqlite3_api->create_collation
#define mc_sqlite3_create_collation16     mc_sqlite3_api->create_collation16
#define mc_sqlite3_create_function        mc_sqlite3_api->create_function
#define mc_sqlite3_create_function16      mc_sqlite3_api->create_function16
#define mc_sqlite3_create_module          mc_sqlite3_api->create_module
#define mc_sqlite3_create_module_v2       mc_sqlite3_api->create_module_v2
#define mc_sqlite3_data_count             mc_sqlite3_api->data_count
#define mc_sqlite3_db_handle              mc_sqlite3_api->db_handle
#define mc_sqlite3_declare_vtab           mc_sqlite3_api->declare_vtab
#define mc_sqlite3_enable_shared_cache    mc_sqlite3_api->enable_shared_cache
#define mc_sqlite3_errcode                mc_sqlite3_api->errcode
#define mc_sqlite3_errmsg                 mc_sqlite3_api->errmsg
#define mc_sqlite3_errmsg16               mc_sqlite3_api->errmsg16
#define mc_sqlite3_exec                   mc_sqlite3_api->exec
#ifndef SQLITE_OMIT_DEPRECATED
#define mc_sqlite3_expired                mc_sqlite3_api->expired
#endif
#define mc_sqlite3_finalize               mc_sqlite3_api->finalize
#define mc_sqlite3_free                   mc_sqlite3_api->free
#define mc_sqlite3_free_table             mc_sqlite3_api->free_table
#define mc_sqlite3_get_autocommit         mc_sqlite3_api->get_autocommit
#define mc_sqlite3_get_auxdata            mc_sqlite3_api->get_auxdata
#define mc_sqlite3_get_table              mc_sqlite3_api->get_table
#ifndef SQLITE_OMIT_DEPRECATED
#define mc_sqlite3_global_recover         mc_sqlite3_api->global_recover
#endif
#define mc_sqlite3_interrupt              mc_sqlite3_api->interruptx
#define mc_sqlite3_last_insert_rowid      mc_sqlite3_api->last_insert_rowid
#define mc_sqlite3_libversion             mc_sqlite3_api->libversion
#define mc_sqlite3_libversion_number      mc_sqlite3_api->libversion_number
#define mc_sqlite3_malloc                 mc_sqlite3_api->malloc
#define mc_sqlite3_mprintf                mc_sqlite3_api->mprintf
#define mc_sqlite3_open                   mc_sqlite3_api->open
#define mc_sqlite3_open16                 mc_sqlite3_api->open16
#define mc_sqlite3_prepare                mc_sqlite3_api->prepare
#define mc_sqlite3_prepare16              mc_sqlite3_api->prepare16
#define mc_sqlite3_prepare_v2             mc_sqlite3_api->prepare_v2
#define mc_sqlite3_prepare16_v2           mc_sqlite3_api->prepare16_v2
#define mc_sqlite3_profile                mc_sqlite3_api->profile
#define mc_sqlite3_progress_handler       mc_sqlite3_api->progress_handler
#define mc_sqlite3_realloc                mc_sqlite3_api->realloc
#define mc_sqlite3_reset                  mc_sqlite3_api->reset
#define mc_sqlite3_result_blob            mc_sqlite3_api->result_blob
#define mc_sqlite3_result_double          mc_sqlite3_api->result_double
#define mc_sqlite3_result_error           mc_sqlite3_api->result_error
#define mc_sqlite3_result_error16         mc_sqlite3_api->result_error16
#define mc_sqlite3_result_int             mc_sqlite3_api->result_int
#define mc_sqlite3_result_int64           mc_sqlite3_api->result_int64
#define mc_sqlite3_result_null            mc_sqlite3_api->result_null
#define mc_sqlite3_result_text            mc_sqlite3_api->result_text
#define mc_sqlite3_result_text16          mc_sqlite3_api->result_text16
#define mc_sqlite3_result_text16be        mc_sqlite3_api->result_text16be
#define mc_sqlite3_result_text16le        mc_sqlite3_api->result_text16le
#define mc_sqlite3_result_value           mc_sqlite3_api->result_value
#define mc_sqlite3_rollback_hook          mc_sqlite3_api->rollback_hook
#define mc_sqlite3_set_authorizer         mc_sqlite3_api->set_authorizer
#define mc_sqlite3_set_auxdata            mc_sqlite3_api->set_auxdata
#define mc_sqlite3_snprintf               mc_sqlite3_api->xsnprintf
#define mc_sqlite3_step                   mc_sqlite3_api->step
#define mc_sqlite3_table_column_metadata  mc_sqlite3_api->table_column_metadata
#define mc_sqlite3_thread_cleanup         mc_sqlite3_api->thread_cleanup
#define mc_sqlite3_total_changes          mc_sqlite3_api->total_changes
#define mc_sqlite3_trace                  mc_sqlite3_api->trace
#ifndef SQLITE_OMIT_DEPRECATED
#define mc_sqlite3_transfer_bindings      mc_sqlite3_api->transfer_bindings
#endif
#define mc_sqlite3_update_hook            mc_sqlite3_api->update_hook
#define mc_sqlite3_user_data              mc_sqlite3_api->user_data
#define mc_sqlite3_value_blob             mc_sqlite3_api->value_blob
#define mc_sqlite3_value_bytes            mc_sqlite3_api->value_bytes
#define mc_sqlite3_value_bytes16          mc_sqlite3_api->value_bytes16
#define mc_sqlite3_value_double           mc_sqlite3_api->value_double
#define mc_sqlite3_value_int              mc_sqlite3_api->value_int
#define mc_sqlite3_value_int64            mc_sqlite3_api->value_int64
#define mc_sqlite3_value_numeric_type     mc_sqlite3_api->value_numeric_type
#define mc_sqlite3_value_text             mc_sqlite3_api->value_text
#define mc_sqlite3_value_text16           mc_sqlite3_api->value_text16
#define mc_sqlite3_value_text16be         mc_sqlite3_api->value_text16be
#define mc_sqlite3_value_text16le         mc_sqlite3_api->value_text16le
#define mc_sqlite3_value_type             mc_sqlite3_api->value_type
#define mc_sqlite3_vmprintf               mc_sqlite3_api->vmprintf
#define mc_sqlite3_vsnprintf              mc_sqlite3_api->xvsnprintf
#define mc_sqlite3_overload_function      mc_sqlite3_api->overload_function
#define mc_sqlite3_prepare_v2             mc_sqlite3_api->prepare_v2
#define mc_sqlite3_prepare16_v2           mc_sqlite3_api->prepare16_v2
#define mc_sqlite3_clear_bindings         mc_sqlite3_api->clear_bindings
#define mc_sqlite3_bind_zeroblob          mc_sqlite3_api->bind_zeroblob
#define mc_sqlite3_blob_bytes             mc_sqlite3_api->blob_bytes
#define mc_sqlite3_blob_close             mc_sqlite3_api->blob_close
#define mc_sqlite3_blob_open              mc_sqlite3_api->blob_open
#define mc_sqlite3_blob_read              mc_sqlite3_api->blob_read
#define mc_sqlite3_blob_write             mc_sqlite3_api->blob_write
#define mc_sqlite3_create_collation_v2    mc_sqlite3_api->create_collation_v2
#define mc_sqlite3_file_control           mc_sqlite3_api->file_control
#define mc_sqlite3_memory_highwater       mc_sqlite3_api->memory_highwater
#define mc_sqlite3_memory_used            mc_sqlite3_api->memory_used
#define mc_sqlite3_mutex_alloc            mc_sqlite3_api->mutex_alloc
#define mc_sqlite3_mutex_enter            mc_sqlite3_api->mutex_enter
#define mc_sqlite3_mutex_free             mc_sqlite3_api->mutex_free
#define mc_sqlite3_mutex_leave            mc_sqlite3_api->mutex_leave
#define mc_sqlite3_mutex_try              mc_sqlite3_api->mutex_try
#define mc_sqlite3_open_v2                mc_sqlite3_api->open_v2
#define mc_sqlite3_release_memory         mc_sqlite3_api->release_memory
#define mc_sqlite3_result_error_nomem     mc_sqlite3_api->result_error_nomem
#define mc_sqlite3_result_error_toobig    mc_sqlite3_api->result_error_toobig
#define mc_sqlite3_sleep                  mc_sqlite3_api->sleep
#define mc_sqlite3_soft_heap_limit        mc_sqlite3_api->soft_heap_limit
#define mc_sqlite3_vfs_find               mc_sqlite3_api->vfs_find
#define mc_sqlite3_vfs_register           mc_sqlite3_api->vfs_register
#define mc_sqlite3_vfs_unregister         mc_sqlite3_api->vfs_unregister
#define mc_sqlite3_threadsafe             mc_sqlite3_api->xthreadsafe
#define mc_sqlite3_result_zeroblob        mc_sqlite3_api->result_zeroblob
#define mc_sqlite3_result_error_code      mc_sqlite3_api->result_error_code
#define mc_sqlite3_test_control           mc_sqlite3_api->test_control
#define mc_sqlite3_randomness             mc_sqlite3_api->randomness
#define mc_sqlite3_context_db_handle      mc_sqlite3_api->context_db_handle
#define mc_sqlite3_extended_result_codes  mc_sqlite3_api->extended_result_codes
#define mc_sqlite3_limit                  mc_sqlite3_api->limit
#define mc_sqlite3_next_stmt              mc_sqlite3_api->next_stmt
#define mc_sqlite3_sql                    mc_sqlite3_api->sql
#define mc_sqlite3_status                 mc_sqlite3_api->status
#define mc_sqlite3_backup_finish          mc_sqlite3_api->backup_finish
#define mc_sqlite3_backup_init            mc_sqlite3_api->backup_init
#define mc_sqlite3_backup_pagecount       mc_sqlite3_api->backup_pagecount
#define mc_sqlite3_backup_remaining       mc_sqlite3_api->backup_remaining
#define mc_sqlite3_backup_step            mc_sqlite3_api->backup_step
#define mc_sqlite3_compileoption_get      mc_sqlite3_api->compileoption_get
#define mc_sqlite3_compileoption_used     mc_sqlite3_api->compileoption_used
#define mc_sqlite3_create_function_v2     mc_sqlite3_api->create_function_v2
#define mc_sqlite3_db_config              mc_sqlite3_api->db_config
#define mc_sqlite3_db_mutex               mc_sqlite3_api->db_mutex
#define mc_sqlite3_db_status              mc_sqlite3_api->db_status
#define mc_sqlite3_extended_errcode       mc_sqlite3_api->extended_errcode
#define mc_sqlite3_log                    mc_sqlite3_api->log
#define mc_sqlite3_soft_heap_limit64      mc_sqlite3_api->soft_heap_limit64
#define mc_sqlite3_sourceid               mc_sqlite3_api->sourceid
#define mc_sqlite3_stmt_status            mc_sqlite3_api->stmt_status
#define mc_sqlite3_strnicmp               mc_sqlite3_api->strnicmp
#define mc_sqlite3_unlock_notify          mc_sqlite3_api->unlock_notify
#define mc_sqlite3_wal_autocheckpoint     mc_sqlite3_api->wal_autocheckpoint
#define mc_sqlite3_wal_checkpoint         mc_sqlite3_api->wal_checkpoint
#define mc_sqlite3_wal_hook               mc_sqlite3_api->wal_hook
#define mc_sqlite3_blob_reopen            mc_sqlite3_api->blob_reopen
#define mc_sqlite3_vtab_config            mc_sqlite3_api->vtab_config
#define mc_sqlite3_vtab_on_conflict       mc_sqlite3_api->vtab_on_conflict
/* Version 3.7.16 and later */
#define mc_sqlite3_close_v2               mc_sqlite3_api->close_v2
#define mc_sqlite3_db_filename            mc_sqlite3_api->db_filename
#define mc_sqlite3_db_readonly            mc_sqlite3_api->db_readonly
#define mc_sqlite3_db_release_memory      mc_sqlite3_api->db_release_memory
#define mc_sqlite3_errstr                 mc_sqlite3_api->errstr
#define mc_sqlite3_stmt_busy              mc_sqlite3_api->stmt_busy
#define mc_sqlite3_stmt_readonly          mc_sqlite3_api->stmt_readonly
#define mc_sqlite3_stricmp                mc_sqlite3_api->stricmp
#define mc_sqlite3_uri_boolean            mc_sqlite3_api->uri_boolean
#define mc_sqlite3_uri_int64              mc_sqlite3_api->uri_int64
#define mc_sqlite3_uri_parameter          mc_sqlite3_api->uri_parameter
#define mc_sqlite3_uri_vsnprintf          mc_sqlite3_api->xvsnprintf
#define mc_sqlite3_wal_checkpoint_v2      mc_sqlite3_api->wal_checkpoint_v2
/* Version 3.8.7 and later */
#define mc_sqlite3_auto_extension         mc_sqlite3_api->auto_extension
#define mc_sqlite3_bind_blob64            mc_sqlite3_api->bind_blob64
#define mc_sqlite3_bind_text64            mc_sqlite3_api->bind_text64
#define mc_sqlite3_cancel_auto_extension  mc_sqlite3_api->cancel_auto_extension
#define mc_sqlite3_load_extension         mc_sqlite3_api->load_extension
#define mc_sqlite3_malloc64               mc_sqlite3_api->malloc64
#define mc_sqlite3_msize                  mc_sqlite3_api->msize
#define mc_sqlite3_realloc64              mc_sqlite3_api->realloc64
#define mc_sqlite3_reset_auto_extension   mc_sqlite3_api->reset_auto_extension
#define mc_sqlite3_result_blob64          mc_sqlite3_api->result_blob64
#define mc_sqlite3_result_text64          mc_sqlite3_api->result_text64
#define mc_sqlite3_strglob                mc_sqlite3_api->strglob
/* Version 3.8.11 and later */
#define mc_sqlite3_value_dup              mc_sqlite3_api->value_dup
#define mc_sqlite3_value_free             mc_sqlite3_api->value_free
#define mc_sqlite3_result_zeroblob64      mc_sqlite3_api->result_zeroblob64
#define mc_sqlite3_bind_zeroblob64        mc_sqlite3_api->bind_zeroblob64
/* Version 3.9.0 and later */
#define mc_sqlite3_value_subtype          mc_sqlite3_api->value_subtype
#define mc_sqlite3_result_subtype         mc_sqlite3_api->result_subtype
/* Version 3.10.0 and later */
#define mc_sqlite3_status64               mc_sqlite3_api->status64
#define mc_sqlite3_strlike                mc_sqlite3_api->strlike
#define mc_sqlite3_db_cacheflush          mc_sqlite3_api->db_cacheflush
/* Version 3.12.0 and later */
#define mc_sqlite3_system_errno           mc_sqlite3_api->system_errno
/* Version 3.14.0 and later */
#define mc_sqlite3_trace_v2               mc_sqlite3_api->trace_v2
#define mc_sqlite3_expanded_sql           mc_sqlite3_api->expanded_sql
/* Version 3.18.0 and later */
#define mc_sqlite3_set_last_insert_rowid  mc_sqlite3_api->set_last_insert_rowid
/* Version 3.20.0 and later */
#define mc_sqlite3_prepare_v3             mc_sqlite3_api->prepare_v3
#define mc_sqlite3_prepare16_v3           mc_sqlite3_api->prepare16_v3
#define mc_sqlite3_bind_pointer           mc_sqlite3_api->bind_pointer
#define mc_sqlite3_result_pointer         mc_sqlite3_api->result_pointer
#define mc_sqlite3_value_pointer          mc_sqlite3_api->value_pointer
/* Version 3.22.0 and later */
#define mc_sqlite3_vtab_nochange          mc_sqlite3_api->vtab_nochange
#define mc_sqlite3_value_nochange         mc_sqlite3_api->value_nochange
#define mc_sqlite3_vtab_collation         mc_sqlite3_api->vtab_collation
/* Version 3.24.0 and later */
#define mc_sqlite3_keyword_count          mc_sqlite3_api->keyword_count
#define mc_sqlite3_keyword_name           mc_sqlite3_api->keyword_name
#define mc_sqlite3_keyword_check          mc_sqlite3_api->keyword_check
#define mc_sqlite3_str_new                mc_sqlite3_api->str_new
#define mc_sqlite3_str_finish             mc_sqlite3_api->str_finish
#define mc_sqlite3_str_appendf            mc_sqlite3_api->str_appendf
#define mc_sqlite3_str_vappendf           mc_sqlite3_api->str_vappendf
#define mc_sqlite3_str_append             mc_sqlite3_api->str_append
#define mc_sqlite3_str_appendall          mc_sqlite3_api->str_appendall
#define mc_sqlite3_str_appendchar         mc_sqlite3_api->str_appendchar
#define mc_sqlite3_str_reset              mc_sqlite3_api->str_reset
#define mc_sqlite3_str_errcode            mc_sqlite3_api->str_errcode
#define mc_sqlite3_str_length             mc_sqlite3_api->str_length
#define mc_sqlite3_str_value              mc_sqlite3_api->str_value
/* Version 3.25.0 and later */
#define mc_sqlite3_create_window_function mc_sqlite3_api->create_window_function
/* Version 3.26.0 and later */
#define mc_sqlite3_normalized_sql         mc_sqlite3_api->normalized_sql
/* Version 3.28.0 and later */
#define mc_sqlite3_stmt_isexplain         mc_sqlite3_api->stmt_isexplain
#define mc_sqlite3_value_frombind         mc_sqlite3_api->value_frombind
/* Version 3.30.0 and later */
#define mc_sqlite3_drop_modules           mc_sqlite3_api->drop_modules
/* Version 3.31.0 and later */
#define mc_sqlite3_hard_heap_limit64      mc_sqlite3_api->hard_heap_limit64
#define mc_sqlite3_uri_key                mc_sqlite3_api->uri_key
#define mc_sqlite3_filename_database      mc_sqlite3_api->filename_database
#define mc_sqlite3_filename_journal       mc_sqlite3_api->filename_journal
#define mc_sqlite3_filename_wal           mc_sqlite3_api->filename_wal
/* Version 3.32.0 and later */
#define mc_sqlite3_create_filename        mc_sqlite3_api->create_filename
#define mc_sqlite3_free_filename          mc_sqlite3_api->free_filename
#define mc_sqlite3_database_file_object   mc_sqlite3_api->database_file_object
/* Version 3.34.0 and later */
#define mc_sqlite3_txn_state              mc_sqlite3_api->txn_state
/* Version 3.36.1 and later */
#define mc_sqlite3_changes64              mc_sqlite3_api->changes64
#define mc_sqlite3_total_changes64        mc_sqlite3_api->total_changes64
/* Version 3.37.0 and later */
#define mc_sqlite3_autovacuum_pages       mc_sqlite3_api->autovacuum_pages
/* Version 3.38.0 and later */
#define mc_sqlite3_error_offset           mc_sqlite3_api->error_offset
#define mc_sqlite3_vtab_rhs_value         mc_sqlite3_api->vtab_rhs_value
#define mc_sqlite3_vtab_distinct          mc_sqlite3_api->vtab_distinct
#define mc_sqlite3_vtab_in                mc_sqlite3_api->vtab_in
#define mc_sqlite3_vtab_in_first          mc_sqlite3_api->vtab_in_first
#define mc_sqlite3_vtab_in_next           mc_sqlite3_api->vtab_in_next
/* Version 3.39.0 and later */
#ifndef SQLITE_OMIT_DESERIALIZE
#define mc_sqlite3_deserialize            mc_sqlite3_api->deserialize
#define mc_sqlite3_serialize              mc_sqlite3_api->serialize
#endif
#define mc_sqlite3_db_name                mc_sqlite3_api->db_name
/* Version 3.40.0 and later */
#define mc_sqlite3_value_encoding         mc_sqlite3_api->value_encoding
/* Version 3.41.0 and later */
#define mc_sqlite3_is_interrupted         mc_sqlite3_api->is_interrupted
/* Version 3.43.0 and later */
#define mc_sqlite3_stmt_explain           mc_sqlite3_api->stmt_explain
/* Version 3.44.0 and later */
#define mc_sqlite3_get_clientdata         mc_sqlite3_api->get_clientdata
#define mc_sqlite3_set_clientdata         mc_sqlite3_api->set_clientdata
/* Version 3.50.0 and later */
#define mc_sqlite3_setlk_timeout          mc_sqlite3_api->setlk_timeout
/* Version 3.51.0 and later */
#define mc_sqlite3_set_errmsg             mc_sqlite3_api->set_errmsg
#define mc_sqlite3_db_status64            mc_sqlite3_api->db_status64
/* Version 3.52.0 and later */
#define mc_sqlite3_str_truncate           mc_sqlite3_api->str_truncate
#define mc_sqlite3_str_free               mc_sqlite3_api->str_free
#define mc_sqlite3_carray_bind            mc_sqlite3_api->carray_bind
#define mc_sqlite3_carray_bind_v2         mc_sqlite3_api->carray_bind_v2
#endif /* !defined(SQLITE_CORE) && !defined(SQLITE_OMIT_LOAD_EXTENSION) */

#if !defined(SQLITE_CORE) && !defined(SQLITE_OMIT_LOAD_EXTENSION)
  /* This case when the file really is being compiled as a loadable 
  ** extension */
# define SQLITE_EXTENSION_INIT1     const mc_sqlite3_api_routines *mc_sqlite3_api=0;
# define SQLITE_EXTENSION_INIT2(v)  mc_sqlite3_api=v;
# define SQLITE_EXTENSION_INIT3     \
    extern const mc_sqlite3_api_routines *mc_sqlite3_api;
#else
  /* This case when the file is being statically linked into the 
  ** application */
# define SQLITE_EXTENSION_INIT1     /*no-op*/
# define SQLITE_EXTENSION_INIT2(v)  (void)v; /* unused parameter */
# define SQLITE_EXTENSION_INIT3     /*no-op*/
#endif

#endif /* SQLITE3EXT_H */
