/* This is a public domain general purpose hash table package
   originally written by Peter Moore @ UCB.

   The hash table data structures were redesigned and the package was
   rewritten by Vladimir Makarov <vmakarov@redhat.com>.  */

#ifndef RUBY_ST2_H
#define RUBY_ST2_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#include <stddef.h>
#include <stdint.h>
#include "ruby/config.h"

RUBY_SYMBOL_EXPORT_BEGIN

#if SIZEOF_LONG == SIZEOF_VOIDP
typedef unsigned long st2_data_t;
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
typedef unsigned LONG_LONG st2_data_t;
#else
# error ---->> st.c requires sizeof(void*) == sizeof(long) or sizeof(LONG_LONG) to be compiled. <<----
#endif
#define ST_DATA_T_DEFINED

#ifndef CHAR_BIT
# ifdef HAVE_LIMITS_H
#  include <limits.h>
# else
#  define CHAR_BIT 8
# endif
#endif
#ifndef _
# define _(args) args
#endif
#ifndef ANYARGS
# ifdef __cplusplus
#   define ANYARGS ...
# else
#   define ANYARGS
# endif
#endif

typedef struct st2_table st2_table;

typedef st2_data_t st2_index_t;

/* Maximal value of unsigned integer type st2_index_t.  */
#define MAX_ST2_INDEX_VAL (~(st2_index_t) 0)

typedef int st_compare_func(st2_data_t, st2_data_t);
typedef st2_index_t st_hash_func(st2_data_t);

typedef char st_check_for_sizeof_st2_index_t[SIZEOF_VOIDP == (int)sizeof(st2_index_t) ? 1 : -1];
#define SIZEOF_ST_INDEX_T SIZEOF_VOIDP

struct st2_hash_type {
    int (*compare)(st2_data_t, st2_data_t); /* st_compare_func* */
    st2_index_t (*hash)(st2_data_t);        /* st_hash_func* */
};

typedef struct st_functions {
	void *(*nonempty_memcpy)(void *dest, const void *src, size_t t, size_t n);
} st_functions_t;

#define ST_INDEX_BITS (SIZEOF_ST_INDEX_T * CHAR_BIT)

#if defined(HAVE_BUILTIN___BUILTIN_CHOOSE_EXPR) && defined(HAVE_BUILTIN___BUILTIN_TYPES_COMPATIBLE_P)
# define ST2_DATA_COMPATIBLE_P(type) \
   __builtin_choose_expr(__builtin_types_compatible_p(type, st2_data_t), 1, 0)
#else
# define ST2_DATA_COMPATIBLE_P(type) 0
#endif

typedef struct st2_table_entry st2_table_entry;

struct st2_table_entry; /* defined in st2.c */

struct st2_table {
    /* Cached features of the table -- see st.c for more details.  */
    unsigned char entry_power, bin_power, size_ind;
    /* How many times the table was rebuilt.  */
    unsigned int rebuilds_num;
    const struct st2_hash_type *type;
    /* Number of entries currently in the table.  */
    st2_index_t num_entries;
    /* Array of bins used for access by keys.  */
    st2_index_t *bins;
    /* Start and bound index of entries in array entries.
       entries_starts and entries_bound are in interval
       [0,allocated_entries].  */
    st2_index_t entries_start, entries_bound;
    /* Array of size 2^entry_power.  */
    st2_table_entry *entries;
    st_functions_t *functions;
};

#define st2_is_member(table,key) rb_st2_lookup((table),(key),(st2_data_t *)0)

enum st2_retval {ST2_CONTINUE, ST2_STOP, ST2_DELETE, ST2_CHECK, ST2_REPLACE};

size_t rb_st2_table_size(const struct st2_table *tbl);
st2_table *rb_st2_init_table(const struct st2_hash_type *, st_functions_t *);
st2_table *rb_st2_init_table_with_size(const struct st2_hash_type *, st_functions_t *, st2_index_t);
st2_table *rb_st2_init_numtable(st_functions_t *);
st2_table *rb_st2_init_numtable_with_size(st_functions_t *, st2_index_t);
st2_table *rb_st2_init_strtable(st_functions_t *);
st2_table *rb_st2_init_strtable_with_size(st_functions_t *, st2_index_t);
st2_table *rb_st2_init_strcasetable(st_functions_t *);
st2_table *rb_st2_init_strcasetable_with_size(st_functions_t *, st2_index_t);
int rb_st2_delete(st2_table *, st2_data_t *, st2_data_t *); /* returns 0:notfound 1:deleted */
int rb_st2_delete_safe(st2_table *, st2_data_t *, st2_data_t *, st2_data_t);
int rb_st2_shift(st2_table *, st2_data_t *, st2_data_t *); /* returns 0:notfound 1:deleted */
int rb_st2_insert(st2_table *, st2_data_t, st2_data_t);
int rb_st2_insert2(st2_table *, st2_data_t, st2_data_t, st2_data_t (*)(st2_data_t));
int rb_st2_lookup(st2_table *, st2_data_t, st2_data_t *);
int rb_st2_get_key(st2_table *, st2_data_t, st2_data_t *);
typedef int st2_update_callback_func(st2_data_t *key, st2_data_t *value, st2_data_t arg, int existing);
/* *key may be altered, but must equal to the old key, i.e., the
 * results of hash() are same and compare() returns 0, otherwise the
 * behavior is undefined */
int rb_st2_update(st2_table *table, st2_data_t key, st2_update_callback_func *func, st2_data_t arg);
typedef int st2_foreach_callback_func(st2_data_t, st2_data_t, st2_data_t);
typedef int st2_foreach_check_callback_func(st2_data_t, st2_data_t, st2_data_t, int);
int rb_st2_foreach_with_replace(st2_table *tab, st2_foreach_check_callback_func *func, st2_update_callback_func *replace, st2_data_t arg);
int rb_st2_foreach(st2_table *, st2_foreach_callback_func *, st2_data_t);
int rb_st2_foreach_check(st2_table *, st2_foreach_check_callback_func *, st2_data_t, st2_data_t);
st2_index_t rb_st2_keys(st2_table *table, st2_data_t *keys, st2_index_t size);
st2_index_t rb_st2_keys_check(st2_table *table, st2_data_t *keys, st2_index_t size, st2_data_t never);
st2_index_t rb_st2_values(st2_table *table, st2_data_t *values, st2_index_t size);
st2_index_t rb_st2_values_check(st2_table *table, st2_data_t *values, st2_index_t size, st2_data_t never);
void rb_st2_add_direct(st2_table *, st2_data_t, st2_data_t);
void rb_st2_free_table(st2_table *);
void rb_st2_cleanup_safe(st2_table *, st2_data_t);
void rb_st2_clear(st2_table *);
st2_table *rb_st2_copy(st2_table *);
CONSTFUNC(int rb_st2_numcmp(st2_data_t, st2_data_t));
CONSTFUNC(st2_index_t rb_st2_numhash(st2_data_t));
PUREFUNC(int rb_st2_locale_insensitive_strcasecmp(const char *s1, const char *s2));
PUREFUNC(int rb_st2_locale_insensitive_strncasecmp(const char *s1, const char *s2, size_t n));
PUREFUNC(size_t rb_st2_memsize(const st2_table *));
PUREFUNC(st2_index_t rb_st2_hash(const void *ptr, size_t len, st2_index_t h));
CONSTFUNC(st2_index_t rb_st2_hash_uint32(st2_index_t h, uint32_t i));
CONSTFUNC(st2_index_t rb_st2_hash_uint(st2_index_t h, st2_index_t i));
CONSTFUNC(st2_index_t rb_st2_hash_end(st2_index_t h));
CONSTFUNC(st2_index_t rb_st2_hash_start(st2_index_t h));

RUBY_SYMBOL_EXPORT_END

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* RUBY_ST2_H */
