#ifndef EXTERNAL_PARSE_H
#define EXTERNAL_PARSE_H

#include <stdarg.h>
#include <sys/types.h>
#include "external/value.h"
#include "external/node.h"
#include "st2.h"

#define rb_encoding void
#define OnigCodePoint unsigned int

typedef struct parser_params rb_parser_t;
typedef struct rb_imemo_tmpbuf_struct rb_imemo_tmpbuf_t;

typedef struct rb_parser_config_struct {
	/*
	 * Reference counter.
	 *   This is needed because both parser and ast refer
	 *   same config pointer.
	 *   We can remove this, once decuple parser and ast from Ruby GC.
	 */
	int counter;

	/* For st2 */
	st_functions_t st_functions;

    /* Memory */
    void *(*malloc)(size_t size);
    void *(*calloc)(size_t number, size_t size);
    void *(*realloc)(void *ptr, size_t newsiz);
    void (*free)(void *ptr);
    void *(*alloc_n)(size_t nelems, size_t elemsiz);
    void *(*alloc)(size_t elemsiz);
    void *(*realloc_n)(void *ptr, size_t newelems, size_t newsiz);
    void *(*zalloc)(size_t elemsiz);
    void *(*rb_memmove)(void *dest, const void *src, size_t t, size_t n);
    void *(*nonempty_memcpy)(void *dest, const void *src, size_t t, size_t n);
    void *(*xmalloc_mul_add)(size_t x, size_t y, size_t z);

    /* imemo */
    // TODO: Should it return `rb_strterm_t *'?
    VALUE (*new_strterm)(VALUE v1, VALUE v2, VALUE v3, VALUE v0, int heredoc);
    int (*strterm_is_heredoc)(VALUE strterm);
    VALUE (*tmpbuf_auto_free_pointer)(void);
    void *(*tmpbuf_set_ptr)(VALUE v, void *ptr);
    rb_imemo_tmpbuf_t *(*tmpbuf_parser_heap)(void *buf, rb_imemo_tmpbuf_t *old_heap, size_t cnt);
    rb_ast_t *(*ast_new)(VALUE nb);

    // VALUE rb_suppress_tracing(VALUE (*func)(VALUE), VALUE arg);
    VALUE (*compile_callback)(VALUE (*func)(VALUE), VALUE arg);
    NODE *(*reg_named_capture_assign)(struct parser_params* p, VALUE regexp, const rb_code_location_t *loc);
    int (*script_lines_defined)(void);
    VALUE (*script_lines_get)(void);

    /* Object */
    VALUE (*obj_freeze)(VALUE obj);
    VALUE (*obj_hide)(VALUE obj);
    int (*obj_frozen)(VALUE obj);
    int (*type_p)(VALUE, int);
    void (*obj_freeze_raw)(VALUE obj);

    int (*fixnum_p)(VALUE);
    int (*symbol_p)(VALUE);

    /* Variable */
    VALUE (*attr_get)(VALUE obj, ID id);

    /* Array */
    VALUE (*ary_new)(void);
    VALUE (*ary_push)(VALUE ary, VALUE elem);
    VALUE (*ary_new_from_args)(long n, ...);
    VALUE (*ary_pop)(VALUE ary);
    VALUE (*ary_last)(int argc, const VALUE *argv, VALUE ary);
    VALUE (*ary_unshift)(VALUE ary, VALUE item);
    VALUE (*ary_new2)(long capa); // ary_new_capa
    VALUE (*ary_entry)(VALUE ary, long offset);
    VALUE (*ary_join)(VALUE ary, VALUE sep);
    VALUE (*ary_reverse)(VALUE ary);
    VALUE (*ary_clear)(VALUE ary);
    long (*array_len)(VALUE a);
    VALUE (*array_aref)(VALUE, long);

    /* Symbol */
    VALUE (*sym_intern_ascii_cstr)(const char *ptr);
    ID (*make_temporary_id)(size_t n);
    int (*is_local_id)(ID);
    int (*is_attrset_id)(ID);
    int (*is_global_name_punct)(const int c);
    int (*id_type)(ID id);
    ID (*id_attrset)(ID);
    ID (*intern)(const char *name);
    ID (*intern2)(const char *name, long len);
    ID (*intern3)(const char *name, long len, rb_encoding *enc);
    ID (*intern_str)(VALUE str);
    int (*is_notop_id)(ID);
    int (*enc_symname_type)(const char *name, long len, rb_encoding *enc, unsigned int allowed_attrset);
    VALUE (*str_intern)(VALUE str);
    const char *(*id2name)(ID id);
    VALUE (*id2str)(ID id);
    VALUE (*id2sym)(ID x);
    ID (*sym2id)(VALUE sym);

    /* String */
    VALUE (*str_catf)(VALUE str, const char *format, ...);
    VALUE (*str_cat_cstr)(VALUE str, const char *ptr);
    VALUE (*str_subseq)(VALUE str, long beg, long len);
    VALUE (*str_dup)(VALUE str);
    VALUE (*str_new_frozen)(VALUE orig);
    VALUE (*str_buf_new)(long capa);
    VALUE (*str_buf_cat)(VALUE, const char*, long);
    void (*str_modify)(VALUE str);
    void (*str_set_len)(VALUE str, long len);
    VALUE (*str_cat)(VALUE str, const char *ptr, long len);
    VALUE (*str_resize)(VALUE str, long len);
    VALUE (*str_new)(const char *ptr, long len);
    VALUE (*str_new_cstr)(const char *ptr);
    VALUE (*fstring)(VALUE);
    int (*is_ascii_string)(VALUE str);
    VALUE (*enc_str_new)(const char *ptr, long len, rb_encoding *enc);
    VALUE (*enc_str_buf_cat)(VALUE str, const char *ptr, long len, rb_encoding *enc);
    VALUE (*str_buf_append)(VALUE str, VALUE str2);
    VALUE (*str_vcatf)(VALUE str, const char *fmt, va_list ap);
    char *(*string_value_cstr)(volatile VALUE *ptr);
    VALUE (*rb_sprintf)(const char *format, ...);
    char *(*rstring_ptr)(VALUE str);
    char *(*rstring_end)(VALUE str);
    long (*rstring_len)(VALUE str);
    VALUE (*filesystem_str_new_cstr)(const char *ptr);
    VALUE (*obj_as_string)(VALUE);

    /* Hash */
    VALUE (*hash_clear)(VALUE hash);
    VALUE (*hash_new)(void);
    VALUE (*hash_aset)(VALUE hash, VALUE key, VALUE val);
    VALUE (*hash_lookup)(VALUE hash, VALUE key);
    VALUE (*ident_hash_new)(void);

    /* Fixnum */
    VALUE (*int2fix)(long i);

    /* Bignum */
    void (*bignum_negate)(VALUE b);
    VALUE (*big_norm)(VALUE x);
    VALUE (*int2big)(intptr_t n);
    VALUE (*cstr_to_inum)(const char *str, int base, int badcheck);

    /* Float */
    VALUE (*float_new)(double d);
    double (*float_value)(VALUE v);

    /* Numeric */
    long (*fix2int)(VALUE val);
    int (*num2int)(VALUE val);
    VALUE (*int_positive_pow)(long x, unsigned long y);
    VALUE (*int2num)(int v);
    long (*fix2long)(VALUE val);

    /* Rational */
    VALUE (*rational_new)(VALUE x, VALUE y);
    VALUE (*rational_raw)(VALUE x, VALUE y);
    VALUE (*rational_raw1)(VALUE x);
    void (*rational_set_num)(VALUE r, VALUE n);
    VALUE (*rational_get_num)(VALUE obj);

    /* Complex */
    VALUE (*complex_raw)(VALUE x, VALUE y);
    void (*rcomplex_set_real)(VALUE cmp, VALUE r);
    void (*rcomplex_set_imag)(VALUE cmp, VALUE i);
    VALUE (*rcomplex_get_real)(VALUE obj);
    VALUE (*rcomplex_get_imag)(VALUE obj);

    /* IO */
    int (*stderr_tty_p)(void);
    void (*write_error_str)(VALUE mesg);
    VALUE (*default_rs)(void);
    VALUE (*io_write)(VALUE io, VALUE str);
    VALUE (*io_flush)(VALUE io);
    VALUE (*io_puts)(int argc, const VALUE *argv, VALUE out);
    VALUE (*io_gets_internal)(VALUE io);

    /* IO (Ractor) */
    VALUE (*debug_output_stdout)(void);
    VALUE (*debug_output_stderr)(void);

    /* Encoding */
    int (*is_usascii_enc)(rb_encoding *enc);
    int (*enc_isalnum)(OnigCodePoint c, rb_encoding *enc);
    int (*enc_precise_mbclen)(const char *p, const char *e, rb_encoding *enc);
    int (*mbclen_charfound_p)(int len);
    const char *(*enc_name)(rb_encoding *enc);
    char *(*enc_prev_char)(const char *s, const char *p, const char *e, rb_encoding *enc);
    rb_encoding* (*enc_get)(VALUE obj);
    int (*enc_asciicompat)(rb_encoding *enc);
    rb_encoding *(*utf8_encoding)(void);
    VALUE (*enc_associate)(VALUE obj, rb_encoding *enc);
    rb_encoding *(*ascii8bit_encoding)(void);
    int (*enc_codelen)(int c, rb_encoding *enc);
    int (*enc_mbcput)(unsigned int c, void *buf, rb_encoding *enc);
    int (*char_to_option_kcode)(int c, int *option, int *kcode);
    int (*ascii8bit_encindex)(void);
    int (*enc_find_index)(const char *name);
    rb_encoding *(*enc_from_index)(int idx);
    VALUE (*enc_associate_index)(VALUE obj, int encindex);
    int (*enc_isspace)(OnigCodePoint c, rb_encoding *enc);
    int enc_coderange_7bit;
    int enc_coderange_unknown;
    rb_encoding *(*enc_compatible)(VALUE str1, VALUE str2);
    VALUE (*enc_from_encoding)(rb_encoding *enc);
    int (*encoding_get)(VALUE obj);
    void (*encoding_set)(VALUE obj, int encindex);
    int (*encoding_is_ascii8bit)(VALUE obj);
    rb_encoding *(*usascii_encoding)(void);

    /* Ractor */
    VALUE (*ractor_make_shareable)(VALUE obj);

    /* Compile */
    int (*vm_keep_script_lines)(void);
    // int rb_local_defined(ID id, const rb_iseq_t *iseq);
    int (*local_defined)(ID, const void*);
    // int rb_dvar_defined(ID id, const rb_iseq_t *iseq);
    int (*dvar_defined)(ID, const void*);

    /* Compile (parse.y) */
    int (*literal_cmp)(VALUE val, VALUE lit);
    st2_index_t (*literal_hash)(VALUE a);

    /* Error (Exception) */
    const char *(*builtin_class_name)(VALUE x);
    VALUE (*syntax_error_append)(VALUE, VALUE, int, int, rb_encoding*, const char*, va_list);
    void (*raise)(VALUE exc, const char *fmt, ...);
    VALUE (*syntax_error_new)(void);

    /* Eval */
    VALUE (*errinfo)(void);
    void (*set_errinfo)(VALUE err);
    void (*exc_raise)(VALUE mesg);
    VALUE (*make_exception)(int argc, const VALUE *argv);

    /* GC */
    void (*sized_xfree)(void *x, size_t size);
    void *(*sized_realloc_n)(void *ptr, size_t new_count, size_t element_size, size_t old_count);
    VALUE (*obj_write)(VALUE, VALUE *, VALUE);
    VALUE (*obj_written)(VALUE, VALUE, VALUE);
    void (*gc_register_mark_object)(VALUE object);
    void (*gc_guard)(VALUE);
    void (*gc_mark)(VALUE);
    void (*gc_mark_movable)(VALUE ptr);
    VALUE (*gc_location)(VALUE value);

    /* Re */
    VALUE (*reg_compile)(VALUE str, int options, const char *sourcefile, int sourceline);
    VALUE (*reg_check_preprocess)(VALUE str);
    int (*memcicmp)(const void *x, const void *y, long len);

    /* Error */
    void (*compile_warn)(const char *file, int line, const char *fmt, ...);
    void (*compile_warning)(const char *file, int line, const char *fmt, ...);
    void (*bug)(const char *fmt, ...);
    void (*fatal)(const char *fmt, ...);
    VALUE (*verbose)(void);

    /* VM */
    VALUE (*make_backtrace)(void);

    /* Util */
    unsigned long (*scan_hex)(const char *start, size_t len, size_t *retlen);
    unsigned long (*scan_oct)(const char *start, size_t len, size_t *retlen);
    unsigned long (*scan_digits)(const char *str, ssize_t len, int base, size_t *retlen, int *overflow);
    double (*strtod)(const char *s00, char **se);

    /* ctype */
    int (*isspace)(int c);
    int (*isascii)(int c);
    int (*iscntrl)(int c);
    int (*isalpha)(int c);
    int (*isdigit)(int c);
    int (*isalnum)(int c);
    int (*isxdigit)(int c);
    int (*strcasecmp)(const char *s1, const char *s2);
    int (*strncasecmp)(const char *s1, const char *s2, size_t n);

    /* Misc */
    VALUE (*rbool)(VALUE);
    int (*undef_p)(VALUE);
    int (*rtest)(VALUE obj);
    int (*nil_p)(VALUE obj);
    int (*flonum_p)(VALUE obj);
    VALUE qnil;
    VALUE qtrue;
    VALUE qfalse;
    VALUE qundef;
    VALUE eArgError;
    VALUE mRubyVMFrozenCore;
    int (*long2int)(long);
    int (*special_const_p)(VALUE);
    int (*builtin_type)(VALUE);

    VALUE (*node_case_when_optimizable_literal)(const NODE *const node);

} rb_parser_config_t;

rb_parser_config_t *rb_ruby_parser_config_new(void *(*malloc)(size_t size));
void rb_ruby_parser_free(void *ptr);
void rb_ruby_parser_config_free(rb_parser_config_t *config);

rb_parser_t *rb_ruby_parser_new(rb_parser_config_t *config);
rb_ast_t* rb_ruby_parser_compile_string(rb_parser_t *p, const char *f, VALUE s, int line);

#undef rb_encoding
#undef OnigCodePoint

#endif /* EXTERNAL_PARSE_H */
