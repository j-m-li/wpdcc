/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
All source code is Public Domain.

## Obtain the source code

    git clone https://git.candlhat.org/xmake.git

## Building

    BSD:
    
        Make sure you have gcc and gmake installed then run gmake -f Makefile.unix.
        Alternatively, you can run make -f Makefile.bsd.
    
    Linux:
    
        Make sure you have gcc and make installed then run make -f Makefile.unix.
    
    macOS:
    
        Make sure you have xcode command line tools installed then run make -f Makefile.unix.
    
    Windows:
    
        Make sure you have mingw installed and the location within your PATH variable then run mingw32-make.exe -f Makefile.w32.
*/
/******************************************************************************
 * @file            command.h
 *****************************************************************************/
#ifndef     _COMMAND_H
#define     _COMMAND_H

struct commands {

    char *text;
    unsigned long len;

};

#endif      /* _COMMAND_H */
/******************************************************************************
 * @file            cstr.h
 *****************************************************************************/
#ifndef     _CSTR_H
#define     _CSTR_H

typedef struct CString {

    long size;
    void *data;
    
    long size_allocated;

} CString;

void cstr_ccat (CString *cstr, int ch);
void cstr_cat (CString *cstr, const char *str, long len);

void cstr_new (CString *cstr);
void cstr_free (CString *cstr);

#endif      /* _CSTR_H */
/******************************************************************************
 * @file            dep.h
 *****************************************************************************/
#ifndef     _DEP_H
#define     _DEP_H

struct dep {

    char *name;
    struct dep *next;

};

#endif      /* _DEP_H */
/******************************************************************************
 * @file            hashtab.h
 *****************************************************************************/
#ifndef     _HASHTAB_H
#define     _HASHTAB_H

#include    <stddef.h>

#define hashtab_get haget
#define hashtab_put haput
#define hashtab_remove haremove


struct hashtab_name {

    const char *chars;
    unsigned int bytes, hash;

};

struct hashtab_entry {

    struct hashtab_name *key;
    void *value;

};

struct hashtab {

    struct hashtab_entry *entries;
    unsigned long capacity, count, used;

};

struct hashtab_name *hashtab_alloc_name (const char *str);
struct hashtab_name *hashtab_get_key (struct hashtab *table, const char *name);

int hashtab_put (struct hashtab *table, struct hashtab_name *key, void *value);

void *hashtab_get (struct hashtab *table, struct hashtab_name *key);
void hashtab_remove (struct hashtab *table, struct hashtab_name *key);

#endif      /* _HASHTAB_H */
/******************************************************************************
 * @file            lib.h
 *****************************************************************************/
#ifndef     _LIB_H
#define     _LIB_H

#if     defined (_WIN32)
# define    PATHSEP                     ";"
#else
# define    PATHSEP                     ":"
#endif

struct option {

    const char *rule;
    int (*callback) (const char *);

};

char *skip_whitespace (char *p);

char *xstrdup (const char *__s);
char *xstrndup (const char *__s, unsigned long __len);

int parse_args (char **__args, int __cnt);
void dynarray_add (void *__ptab, unsigned long *__nb_ptr, void *__data);

void *xmalloc (unsigned long __sz);
void *xrealloc (void *__ptr, unsigned long __sz);


char *get_current_directory (void);

int change_directory (const char *__path);
int directory_exists (const char *__path);

#endif      /* _LIB_H */
/******************************************************************************
 * @file            read.h
 *****************************************************************************/
#ifndef     _READ_H
#define     _READ_H

void read_memory_makefile (const char *filename, unsigned long line_no, char *memory);
int read_makefile (const char *filename);

struct nameseq {

    char *name;
    struct nameseq *next;

};

void record_files (const char *filename, unsigned long line_no, struct nameseq *filenames, char *cmds, size_t cmds_idx, char *depstr);
void *parse_nameseq (char *line, size_t size);

#endif      /* _READ_H */
/******************************************************************************
 * @file            report.h
 *****************************************************************************/
#ifndef     _REPORT_H
#define     _REPORT_H

enum report_type {

    REPORT_WARNING,
    REPORT_ERROR,
    REPORT_FATAL_ERROR,
    REPORT_INTERNAL_ERROR

};

#if     defined (_WIN32)
# define    COLOR_ERROR                 12
# define    COLOR_WARNING               13
# define    COLOR_INTERNAL_ERROR        19
#else
# define    COLOR_ERROR                 91
# define    COLOR_INTERNAL_ERROR        94
# define    COLOR_WARNING               95
#endif

unsigned long get_error_count (void);
void report_at (const char *filename, unsigned long lineno, enum report_type type, const char *fmt, ...);

#endif      /* _REPORT_H */
/******************************************************************************
 * @file            variable.h
 *****************************************************************************/
#ifndef     _VARIABLE_H
#define     _VARIABLE_H

enum variable_flavor {

    VAR_FLAVOR_RECURSIVELY_EXPANDED,
    VAR_FLAVOR_SIMPLY_EXPANDED,
    VAR_FLAVOR_IMMEDIATELY_EXPANDED

};

enum variable_origin {

    VAR_ORIGIN_AUTOMATIC,
    VAR_ORIGIN_COMMAND_LINE,
    VAR_ORIGIN_FILE

};

struct variable {

    char *name, *value;
    
    enum variable_flavor flavor;
    enum variable_origin origin;

};

struct variable *variable_add (char *name, char *value, enum variable_origin origin);
struct variable *variable_change (char *name, char *value, enum variable_origin origin);
struct variable *variable_find (char *name);

void parse_var_line (const char *filename, unsigned long line_no, char *line, enum variable_origin origin);
char *variable_expand_line (const char *filename, unsigned long line_no, char *line);

void variables_init (void);

#endif      /* _VARIABLE_H */
/******************************************************************************
 * @file            rule.h
 *****************************************************************************/
#ifndef     _RULE_H
#define     _RULE_H


struct rule {

    char *name;
    
    struct dep *deps;
    struct commands *cmds;
    
    const char *filename;
    unsigned long line_no;

};

struct suffix_rule {

    char *first, *second;
    struct commands *cmds;
    
    const char *filename;
    unsigned long line_no;
    
    struct suffix_rule *next;

};

extern struct suffix_rule *suffix_rules;
struct rule *rule_find (const char *name);

void rule_add (const char *filename, unsigned long line_no, char *name, struct dep *deps, struct commands *cmds);
void rule_add_suffix (const char *filename, unsigned long line_no, char *name, struct commands *cmds);
void rules_init (void);

#endif      /* _RULE_H */
/******************************************************************************
 * @file            xmake.h
 *****************************************************************************/
#ifndef     _XMAKE_H
#define     _XMAKE_H

struct xmake_state {

    char **makefiles;
    unsigned long nb_makefiles;
    
    char **goals;
    unsigned long nb_goals;
    
    char **include_paths;
    unsigned long nb_include_paths;
    
    char **directories;
    unsigned long nb_directories;
    
    char *path;
    int quiet, no_print;
    
    int dry_run, env_override, keep_going;
    int ignore_errors;

};

#if     defined (__PDOS386__)
# ifndef        __PDOS__
#  define   __PDOS__
# endif
#endif

extern struct xmake_state *state;
extern const char *program_name;

int rule_search_and_build (char *name);

#endif      /* _XMAKE_H */
/******************************************************************************
 * @file            cstr.c
 *****************************************************************************/
#include    <stdlib.h>
#include    <string.h>

extern void *xrealloc (void *__ptr, unsigned long __size);

static void cstr_realloc (CString *cstr, long new_size) {

    long size = cstr->size_allocated;
    
    if (size < 8) {
        size = 8;
    }
    
    while (size < new_size) {
        size *= 2;
    }
    
    cstr->data = xrealloc (cstr->data, size);
    cstr->size_allocated = size;

}

void cstr_ccat (CString *cstr, int ch) {

    long size = cstr->size + 1;
    
    if (size > cstr->size_allocated) {
        cstr_realloc (cstr, size);
    }
    
    ((unsigned char *) cstr->data)[size - 1] = ch;
    cstr->size = size;

}

void cstr_cat (CString *cstr, const char *str, long len) {

    long size;
    
    if (len <= 0) {
        len = strlen (str) + 1 + len;
    }
    
    size = cstr->size + len;
    
    if (size > cstr->size_allocated) {
        cstr_realloc (cstr, size);
    }
    
    memmove (((unsigned char *) cstr->data) + cstr->size, str, len);
    cstr->size = size;

}

void cstr_new (CString *cstr) {
    memset (cstr, 0, sizeof (CString));
}

void cstr_free (CString *cstr) {

    free (cstr->data);
    cstr_new (cstr);

}
/******************************************************************************
 * @file            hashtab.c
 *****************************************************************************/
#include    <stddef.h>
#include    <stdlib.h>
#include    <string.h>

static struct hashtab_entry *find_entry (struct hashtab_entry *entries, unsigned long capacity, struct hashtab_name *key);

static int adjust_capacity (struct hashtab *table, unsigned long new_capacity) {

    struct hashtab_entry *new_entries, *old_entries;
    unsigned long i, new_count, old_capacity;
    
    if ((new_entries = xmalloc (sizeof (*new_entries) * new_capacity)) == NULL) {
        return -2;
    }
    
    for (i = 0; i < new_capacity; ++i) {
    
        struct hashtab_entry *entry = &new_entries[i];
        
        entry->key = NULL;
        entry->value = NULL;
    
    }
    
    old_entries = table->entries;
    old_capacity = table->capacity;
    
    new_count = 0;
    
    for (i = 0; i < old_capacity; ++i) {
    
        struct hashtab_entry *entry = &old_entries[i], *dest;
        
        if (entry->key == NULL) {
            continue;
        }
        
        dest = find_entry (new_entries, new_capacity, entry->key);
        
        dest->key = entry->key;
        dest->value = entry->value;
        
        ++new_count;
    
    }
    
    free (old_entries);
    
    table->capacity = new_capacity;
    table->count = new_count;
    table->entries = new_entries;
    table->used = new_count;
    
    return 0;

}

static struct hashtab_entry *find_entry (struct hashtab_entry *entries, unsigned long capacity, struct hashtab_name *key) {

    struct hashtab_entry *tombstone = NULL;
    unsigned long index;
    
    for (index = key->hash % capacity; ; index = (index + 1) % capacity) {
    
        struct hashtab_entry *entry = &entries[index];
        
        if (entry->key == NULL) {
        
            if (entry->value == NULL) {
            
                if (tombstone == NULL) {
                    return entry;
                }
                
                return tombstone;
            
            } else if (tombstone == NULL) {
                tombstone = entry;
            }
        
        } else if (entry->key->bytes == key->bytes) {
        
            if (memcmp (entry->key->chars, key->chars, key->bytes) == 0 && entry->key->hash == key->hash) {
                return entry;
            }
        
        }
    
    }

}

static unsigned int hash_string (const void *p, unsigned int length) {

    unsigned char *str = (unsigned char *) p;
    unsigned int i;
    
    unsigned int result = 0;
    
    for (i = 0; i < length; i++) {
        result = (((unsigned short) str[i]) << 4) + (result >> 9) + result + (result >> 3) + (((unsigned short) str[i]) << 2) - (result << 12);
    }
    
    return result;

}

struct hashtab_name *hashtab_alloc_name (const char *str) {

    unsigned int bytes = strlen (str), hash = hash_string (str, bytes);
    struct hashtab_name *name;
    
    if ((name = xmalloc (sizeof (*name))) == NULL) {
        return NULL;
    }
    
    name->bytes = bytes;
    name->chars = str;
    name->hash = hash;
    
    return name;

}

struct hashtab_name *hashtab_get_key (struct hashtab *table, const char *name) {

    struct hashtab_name *key;
    struct hashtab_entry *entry;
    
    if (table == NULL || table->count == 0 || !(key = hashtab_alloc_name (name))) {
        return 0;
    }
    
    entry = find_entry (table->entries, table->capacity, key);
    free (key);
    
    return entry->key;

}

void *hashtab_get (struct hashtab *table, struct hashtab_name *key) {

    struct hashtab_entry *entry;
    
    if (table == NULL || table->count == 0) {
        return NULL;
    }
    
    entry = find_entry (table->entries, table->capacity, key);
    
    if (entry->key == NULL) {
        return NULL;
    }
    
    return entry->value;

}

int hashtab_put (struct hashtab *table, struct hashtab_name *key, void *value) {

    const long MIN_CAPACITY = 15;
    
    struct hashtab_entry *entry;
    int ret = 0;
    
    if (table->used >= table->capacity / 2) {
    
        long capacity = table->capacity * 2 - 1;
        
        if (capacity < MIN_CAPACITY) {
            capacity = MIN_CAPACITY;
        }
        
        if ((ret = adjust_capacity (table, capacity))) {
            return ret;
        }
    
    }
    
    entry = find_entry (table->entries, table->capacity, key);
    
    if (entry->key == NULL) {
    
        ++table->count;
        
        if (entry->value == NULL) {
            ++table->used;
        }
    
    }
    
    entry->key = key;
    entry->value = value;
    
    return 0;

}

void hashtab_remove (struct hashtab *table, struct hashtab_name *key) {

    struct hashtab_entry *entry;
    
    if ((entry = find_entry (table->entries, table->capacity, key)) != NULL) {
    
        entry->key = NULL;
        entry->value = NULL;
        
        --table->count;
    
    }

}
/******************************************************************************
 * @file            lib.c
 *****************************************************************************/
#include    <assert.h>
#include    <ctype.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>


size_t len = 0;

static int print_help (const char *arg) {

    (void) arg;
    
    if (program_name) {
    
        fprintf (stderr, "Usage: %s [options] [target] ...\n\n", program_name);
        fprintf (stderr, "Options:\n\n");
        
        fprintf (stderr, "    -B                    Ignored.\n");
        fprintf (stderr, "\n");
        
        fprintf (stderr, "    -C DIRECTORY          Change to DIRECTORY before doing anything.\n");
        fprintf (stderr, "    -I DIRECTORY          Search DIRECTORY for included makefiles.\n");
        fprintf (stderr, "\n");
        
        fprintf (stderr, "    -e                    Environment variables override makefiles.\n");
        fprintf (stderr, "    -f FILE               Read FILE as a makefile.\n");
        fprintf (stderr, "    -s                    Don't echo recipes.\n");
        fprintf (stderr, "\n");
        
        fprintf (stderr, "    --no-print-directory  Don't print the current directory.\n");
        fprintf (stderr, "    --help                Print this help information.\n");
        
        fprintf (stderr, "\n");
    
    }
    
    exit (EXIT_SUCCESS);
    return 0;       /* Needed by wcl */

}

static int print_version (const char *arg) {

    (void) arg;
    
    fprintf (stderr, "%s: version 0.0.1\n", program_name);
    return 1;

}

static int set_quiet (const char *arg) {

    (void) arg;
    
    state->quiet = 1;
    return 0;

}

static int set_ignore_errors (const char *arg) {

    (void) arg;
    
    state->ignore_errors = 1;
    return 0;

}

static int set_dry_run (const char *arg) {

    (void) arg;
    
    state->dry_run = 1;
    return 0;

}

static int set_no_print (const char *arg) {

    (void) arg;
    
    state->no_print = 1;
    return 0;

}

static int set_env_override (const char *arg) {

    (void) arg;
    
    state->env_override = 1;
    return 0;

}

static int set_keep_going (const char *arg) {

    (void) arg;
    
    state->keep_going = 1;
    return 0;

}

static int add_include_path (const char *path) {

    const char *in = path;
    const char *p;
    
    do {
    
        int c;
        
        CString str;
        cstr_new (&str);
        
        for (p = in; c = *p, c != '\0' && c != PATHSEP[0]; p++) {
        
            if (c == '\\') { c = '/'; }
            cstr_ccat (&str, c);
        
        }
        
        if (str.size) {
        
            if (((char *) str.data)[str.size - 1] != '/') {
                cstr_ccat (&str, '/');
            }
            
            cstr_ccat (&str, '\0');
            dynarray_add (&state->include_paths, &state->nb_include_paths, xstrdup (str.data));
        
        }
        
        cstr_free (&str);
        in = (p + 1);
    
    } while (*p != '\0');
    
    return 0;

}

static int ignored (const char *arg) {

    (void) arg;
    return 0;

}

static int add_directory (const char *path) {

    char *arg = xstrdup (path);
    
    while (arg[strlen (arg) - 1] == '/' || arg[strlen (arg) - 1] == '\\') {
        arg[strlen (arg) - 1] = '\0';
    }
    
    dynarray_add (&state->directories, &state->nb_directories, arg);
    return 0;

}

static int add_makefile (const char *name) {

    dynarray_add (&state->makefiles, &state->nb_makefiles, xstrdup (name));
    return 0;

}

static int non_option (const char *arg) {

    if (strchr (arg, '=')) {
    
        char *temp = xstrdup (arg);
        
        parse_var_line ("<command-line>", 1, temp, VAR_ORIGIN_COMMAND_LINE);
        free (temp);
    
    } else {
        dynarray_add (&state->goals, &state->nb_goals, xstrdup (arg));
    }
    
    return 0;

}

static struct option opts[] = {

    {   "--help",                       &print_help             },
    
    {   "--version",                    &print_version          },
    
    {   "--silent",                     &set_quiet              },
    {   "--quiet",                      &set_quiet              },
    {   "-s",                           &set_quiet              },
    
    {   "--no-print-directory",         &set_no_print           },
    
    {   "--no-builtin-rules",           &ignored                },
    {   "-r",                           &ignored                },
    
    {   "--include_dir=",               &add_include_path       },
    {   "-I:",                          &add_include_path       },
    
    {   "--always-make",                &ignored                },
    {   "-B",                           &ignored                },
    
    {   "--directory=",                 &add_directory          },
    {   "-C:",                          &add_directory          },
    
    {   "--makefile=",                  &add_makefile           },
    {   "--file=",                      &add_makefile           },
    {   "-f:",                          &add_makefile           },
    
    {   "--ignore-errors",              &set_ignore_errors      },
    {   "-i",                           &set_ignore_errors      },
    
    {   "--dry_run",                    &set_dry_run            },
    {   "-n",                           &set_dry_run            },
    
    {   "--environment-overrides",      &set_env_override       },
    {   "-e",                           &set_env_override       },
    
    {   "--keep-going",                 &set_keep_going         },
    {   "-k",                           &set_keep_going         },
    
    {   0,                              &non_option             }

};

static int match_rule (const char *rule, const char *arg) {

    const char *ptr;
    
    assert (rule);
    assert (arg);
    
    while (*rule == *arg && *rule && *arg) {
    
        rule++;
        arg++;
    
    }
    
    switch (*rule) {
    
        case '\0':
            return *arg == '\0';
        
        case '[':
        
            ptr = ++rule;
            
            while (*ptr != ']') {
                ptr++;
            }
            
            return !strncmp (rule, arg, ptr - rule) ? match_rule (ptr + 1, arg + (ptr - rule)) : match_rule (ptr + 1, arg);
        
        case '{':
        
            do {
            
                ptr = ++rule;
                
                while (*ptr != '}' && *ptr != '|') {
                    ptr++;
                }
                
                if (strncmp (rule, arg, ptr - rule) == 0) {
                
                    arg = arg + (ptr - rule);
                    
                    do {
                    
                        rule = ptr;
                        ptr++;
                    
                    } while (*rule != '}');
                    
                    return match_rule (ptr, arg);
                
                }
                
                rule = ptr;
            
            } while (*rule == '|');
            
            break;
    
    }
    
    return 0;

}

static int match_arg (struct option opt, int argc, char **argv, int *ret) {

    size_t rule_len, arg_len;
    assert (opt.callback);
    
    *ret = 0;
    
    rule_len = strlen (opt.rule);
    arg_len = strlen (argv[0]);
    
    switch (opt.rule[rule_len - 1]) {
    
        case ':':
        
            rule_len--;
            
            if (strncmp (opt.rule, argv[0], rule_len) == 0) {
            
                if (arg_len == rule_len) {
                
                    if (argc < 2) {
                    
                        report_at (program_name, 0, REPORT_ERROR, "missing argument to '%s'", opt.rule);
                        
                        *ret = 1;
                        return 0;
                    
                    }
                    
                    *ret = opt.callback (argv[1]);
                    return 2;
                
                } else {
                
                    assert (arg_len > rule_len);
                    
                    *ret = opt.callback (argv[0] + rule_len);
                    return 1;
                
                }
            
            }
            
            break;
        
        case '<':
        
            rule_len--;
            /* fall through */
        
        case '=':
        
            if (strncmp (opt.rule, argv[0], rule_len) == 0) {
            
                if (arg_len == rule_len) {
                
                    report_at (program_name, 0, REPORT_ERROR, "missing argument to '%s'", argv[0]);
                    
                    *ret = 1;
                    return 0;
                
                }
                
                *ret = opt.callback (argv[0] + rule_len);
                return 1;
            
            }
            
            break;
        
        default:
        
            if (match_rule (opt.rule, argv[0])) {
            
                *ret = opt.callback (argv[0]);
                return 1;
            
            }
    
    }
    
    return 0;

}


char *skip_whitespace (char *p) {

    while (isspace ((int) *p)) {
        p++;
    }
    
    return p;

}

char *xstrdup (const char *__s) {

    char *p = xmalloc (strlen (__s) + 1);
    
    strcpy (p, __s);
    return p;

}

char *xstrndup (const char *__s, unsigned long __len) {

    char *p = xmalloc (__len + 1);
    
    memcpy (p, __s, __len);
    return p;

}

int parse_args (char **__args, int __cnt) {

    struct option *opt, *last;
    int i, c, ret;
    
    for (last = opts; last->rule; last++) {
        ;
    }
    
    for (i = 1; i < __cnt; ) {
    
        if (*(__args[i]) == '-') {
        
            for (opt = opts, c = 0; opt->rule && !c; opt++) {
            
                c = match_arg (*opt, __cnt - i, __args + i, &ret);
                
                if (ret != 0) {
                    return ret;
                }
            
            }
            
            if (c) {
                i += c;
            } else {
            
                report_at (program_name, 0, REPORT_ERROR, "unrecognized option '%s'", __args[i]);
                return 1;
            
            }
        
        } else {
        
            if ((ret = last->callback (__args[i])) != 0) {
                return ret;
            }
            
            i++;
        
        }
    
    }
    
    if (!state->nb_makefiles) {
        dynarray_add (&state->makefiles, &state->nb_makefiles, xstrdup ("Makefile"));
    }
    
    return 0;

}

void dynarray_add (void *__ptab, unsigned long *__nb_ptr, void *__data) {

    long nb, nb_alloc;
    void **pp;
    
    nb = *__nb_ptr;
    pp = *(void ***) __ptab;
    
    if ((nb & (nb - 1)) == 0) {
    
        if (!nb) {
            nb_alloc = 1;
        } else {
            nb_alloc = nb * 2;
        }
        
        pp = xrealloc (pp, nb_alloc * sizeof (void *));
        *(void ***) __ptab = pp;
    
    }
    
    pp[nb++] = __data;
    *__nb_ptr = nb;

}

void *xmalloc (unsigned long __sz) {

    void *ptr = malloc (__sz);
    
    if (!ptr && __sz) {
    
        report_at (program_name, 0, REPORT_ERROR, "memory full (malloc)");
        exit (EXIT_FAILURE);
    
    }
    
    memset (ptr, 0, __sz);
    return ptr;

}

void *xrealloc (void *__ptr, unsigned long __sz) {

    void *ptr = realloc (__ptr, __sz);
    
    if (!ptr && __sz) {
    
        report_at (program_name, 0, REPORT_ERROR, "memory full (realloc)");
        exit (EXIT_FAILURE);
    
    }
    
    return ptr;

}


#if     defined(_WIN32)
# include   <windows.h>

char *get_current_directory (void) {

    static TCHAR tszBuffer[4096];
    size_t i;
    
    DWORD dwRet;
    
    if ((dwRet = GetCurrentDirectory (sizeof (tszBuffer), tszBuffer)) == 0) {
    
        report_at (program_name, 0, REPORT_FATAL_ERROR, "failed to get current directory");
        exit (EXIT_FAILURE);
    
    }
    
    for (i = 0; i < strlen (tszBuffer); i++) {
    
        if (tszBuffer[i] == '\\') {
            tszBuffer[i] = '/';
        }
    
    }
    
    return tszBuffer;

}

int change_directory (const char *__path) {
    return (SetCurrentDirectory (__path) != 0);
}

int directory_exists (const char *__path) {

    DWORD dwAttrib = GetFileAttributes (__path);
    return ((dwAttrib != -1LU) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}
#elif   defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
# include   <dirent.h>
# include   <unistd.h>

char *get_current_directory (void) {

    static char cwd[4096] = { 0 };
    memset (cwd, 0, sizeof (cwd));
    
    if (getcwd (cwd, sizeof (cwd))) {
        return cwd;
    }
    
    report_at (program_name, 0, REPORT_FATAL_ERROR, "failed to get current directory");
    exit (EXIT_FAILURE);

}

int change_directory (const char *__path) {
    return (chdir (__path) == 0);
}

int directory_exists (const char *__path) {

    DIR *dir;
    
    if ((dir = opendir (__path))) {
    
        closedir (dir);
        return 1;
    
    }
    
    return 0;

}
#elif   defined (__WATCOMC__)
# include       <i86.h>

static char cwd[68] = { 0 }, *old_path = 0;

char *get_current_directory (void) {

    char path[65] = { 0 };
    
    struct SREGS sr = { 0 };
    union REGS r = { 0 };
    
    int drive_no = 0;
    
    r.w.ax = 0x1900;
    int86x (0x21, &r, &r, &sr);
    
    drive_no = r.h.al;
    memset (cwd, 0, 67);
    
    cwd[0] = drive_no + 'A';
    cwd[1] = ':';
    cwd[2] = '\\';
    
    r.w.ax = 0x4700;
    r.h.dl = 0;
    
    r.w.si = FP_OFF (path);
    sr.ds = FP_SEG (path);
    
    int86x (0x21, &r, &r, &sr);
    
    if (*path) {
        sprintf (cwd + 3, "%s\\", path);
    }
    
    return cwd;

}

int change_directory (const char *path) {

    struct SREGS sr = { 0 };
    union REGS r = { 0 };
    
    char *temp;
    unsigned long len;
    
    if (old_path) {
    
        if ((len = strlen (old_path)) > 3) {
        
            while (old_path[len - 1] == '\\') {
                old_path[--len] = '\0';
            }
        
        }
        
        r.w.ax = 0x3b00;
        r.w.dx = FP_OFF (old_path);
        
        sr.ds = FP_SEG (old_path);
        int86x (0x21, &r, &r, &sr);
        
        free (old_path);
    
    }
    
    if (strlen (path) >= 2 && isalpha ((int) path[0]) && path[1] == ':') {
    
        r.h.dl = path[0] - 'A';
        
        r.w.ax = 0x0e00;
        int86 (0x21, &r, &r);
    
    }
    
    old_path = xstrdup (get_current_directory ());
    
    if ((len = strlen (temp = xstrdup (path))) > 3) {
    
        while (temp[len - 1] == '\\') {
            temp[--len] = '\0';
        }
    
    }
    
    r.w.ax = 0x3b00;
    r.w.dx = FP_OFF (temp);
    
    sr.ds = FP_SEG (temp);
    int86x (0x21, &r, &r, &sr);
    
    free (temp);
    
    if (r.w.cflag & 1) {
        return 0;
    }
    
    return 1;

}

int directory_exists (const char *path) {

    struct SREGS sr = { 0 };
    union REGS r = { 0 };
    
    char *temp = xstrdup (path);
    unsigned len = strlen (temp);
    
    if (len > 3) {
    
        while (temp[len - 1] == '\\') {
            temp[--len] = '\0';
        }
    
    }
    
    r.w.ax = 0x4300;
    r.w.dx = FP_OFF (temp);
    
    sr.ds = FP_SEG (temp);
    int86x (0x21, &r, &r, &sr);
    
    free (temp);
    return (!(r.w.cflag & 1) && (r.w.cx & 0x10));

}
#endif
#define linebuf linebuf1
/******************************************************************************
 * @file            read.c
 *****************************************************************************/
#include    <ctype.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>


extern struct variable *default_goal_var;

struct linebuf {

    char *start, *memory;
    unsigned long size;
    
    const char *filename;
    unsigned long line_no;
    
    FILE *f;

};

static char *memory_fgets (char *str, int num, char **source_p) {

    char *source = *source_p;
    char *p = strchr (source, '\n');
    
    if (source[0] == '\0') {
        return 0;
    }
    
    if (p && p - source < num - 1) {
    
        memcpy (str, source, p - source + 1);
        str[p - source + 1] = '\0';
        
        *source_p += p - source + 1;
        return str;
    
    }
    
    if ((int) strlen (source) > num - 1) {
    
        memcpy (str, source, num - 1);
        str[num - 1] = '\0';
        
        *source_p += num - 1;;
        return str;
    
    }
    
    strcpy (str, source);
    
    *source_p += strlen (source);
    return str;

}

static long read_line (struct linebuf *lbuf) {

    long lines_read = 0;
    
    char *end = lbuf->start + lbuf->size;
    char *p = lbuf->start;
    
    while (lbuf->f ? fgets (p, end - p, lbuf->f) : memory_fgets (p, end - p, &(lbuf->memory))) {
    
        size_t offset;
        
        p += strlen (p);
        offset = p - lbuf->start;
        
        lines_read++;
        
        if (offset >= 2) {
        
            if (p[-1] == '\n' && p[-2] != '\\') {
            
                p[-1] = '\0';
                break;
            
            }
        
        } else if (offset >= 1) {
        
            if (p[-1] == '\n') {
            
                p[-1] = '\0';
                break;
            
            }
        
        }
        
        if (end - p >= 80) {
            continue;
        }
        
        lbuf->size *= 2;
        lbuf->start = xrealloc (lbuf->start, lbuf->size);
        
        p = lbuf->start + offset;
        end = lbuf->start + lbuf->size;
    
    }
    
    return lines_read;

}

static void remove_backslash_newlines (char *line) {

    char *in = line, *out = line;
    char *p;
    
    if (!(p = strchr (in, '\n'))) {
        return;
    }
    
    do {
    
        size_t out_line_len = p - in - 1;
        
        if (out != in) {
            memmove (out, in, out_line_len);
        }
        
        out += out_line_len;
        in = p + 1;
        
        while (isspace ((int) *in)) {
            in++;
        }
        
        *(out++) = ' ';
    
    } while ((p = strchr (in, '\n')));
    
    memmove (out, in, strlen (in) + 1);

}

static void remove_comment (char *line) {

    char *p;
    
    if ((p = strchr (line, '#'))) {
        *p = '\0';
    }

}

struct if_stack {

    struct if_stack *prev;
    
    int ignoring;
    int prev_ignoring;
    int has_else;

};

static struct if_stack *cur_if_stack = 0;

static void add_if_stack (void) {

    struct if_stack *if_stack = xmalloc (sizeof *if_stack);
    
    if (cur_if_stack) {
        if_stack->prev_ignoring = cur_if_stack->ignoring;
    }
    
    if_stack->prev = cur_if_stack;
    cur_if_stack = if_stack;

}

static int include_makefile (const char *src_filename, unsigned long line_no, const char *filename, int bsd) {

    struct if_stack *saved_if_stack = cur_if_stack;
    char *path, *new_name;
    
    unsigned long i;
    cur_if_stack = 0;
    
    if (bsd) {
    
        char ch = *filename++, *end;
        
        if (ch != '<' && ch != '"') {
        
            fprintf (stderr, "%s: %s:%lu: *** .include filename must be delimited by '\"' or '<'. Stop.\n", program_name, src_filename, line_no);
            exit (EXIT_FAILURE);
        
        }
        
        if (ch == '<') {
        
            if (!(end = strrchr (filename, '>'))) {
            
                fprintf (stderr, "%s: %s:%lu: *** unclosed .include filename. '>' expected. Stop.\n", program_name, src_filename, line_no);
                exit (EXIT_FAILURE);
            
            }
        
        } else {
        
            if (!(end = strrchr (filename, '"'))) {
            
                fprintf (stderr, "%s: %s:%lu: *** unclosed .include filename. '\"' expected. Stop.\n", program_name, src_filename, line_no);
                exit (EXIT_FAILURE);
            
            }
        
        }
        
        *end = '\0';
    
    }
    
    if (!read_makefile (filename)) {
    
        cur_if_stack = saved_if_stack;
        return 0;
    
    }
    
    for (i = 0; i < state->nb_include_paths; i++) {
    
        path = state->include_paths[i];
        
        new_name = xmalloc (strlen (path) + strlen (filename) + 1);
        sprintf (new_name, "%s%s", path, filename);
        
        if (!read_makefile (new_name)) {
        
            cur_if_stack = saved_if_stack;
            
            free (new_name);
            return 0;
        
        }
        
        free (new_name);
    
    }
    
    fprintf (stderr, "%s: %s:%lu: *** failed to include '%s'. Stop.\n", program_name, src_filename, line_no, filename);
    exit (EXIT_FAILURE);
    
    return 1;       /* Needed for wcl */

}

static int read_lbuf (struct linebuf *lbuf, int set_default) {

    struct nameseq* filenames = 0;
    int ret;
    
    char *cmds, *clean = 0, *depstr = 0, *q;
    size_t clean_size = 0, cmds_idx = 0, cmds_sz = 256;
    
    char *colon, *semicolonp, *commentp;
    long lines_read;
    
    unsigned long line_no = 1;
    cmds = xmalloc (cmds_sz);
    
#define     record_waiting_files()                                              \
    do {                                                                        \
        if (filenames) {                                                        \
            record_files (lbuf->filename, line_no - 1, filenames, cmds, cmds_idx, depstr);                   \
            filenames = 0;                                                      \
        }                                                                       \
        cmds_idx = 0;                                                           \
    } while (0)
    
    while ((lines_read = read_line (lbuf))) {
    
        char *line = lbuf->start;
        int after_else = 0;
        
        char *p, *src, *dst, *comma, *arg1 = 0, *arg2 = 0;
        char ch;
        
        line_no = lbuf->line_no;
        lbuf->line_no += lines_read;
        
        if (*line == '\0') {
            continue;
        }
        
        if (strlen (line) + 1 > clean_size) {
        
            clean_size = strlen (line) + 1;
            
            free (clean);
            clean = xmalloc (clean_size);
        
        }
        
        strcpy (clean, line);
        
        remove_backslash_newlines (clean);
        remove_comment (clean);
        
        p = skip_whitespace (clean);
        
        if (*p == '\0') {
            continue;
        }
        
        /**
         * In command lines ifeq, etc. with space/tab before them
         * should be treated as real commands, not directives.
         */
        if (filenames && p != clean) {
            goto is_command;
        }
        
        if (strncmp (p, ".else", 5) == 0 || strncmp (p, "else", 4) == 0) {
        
            if (*p == '.') {
                p = skip_whitespace (p + 5);
            } else {
                p = skip_whitespace (p + 4);
            }
            
            if (!cur_if_stack) {
                fprintf (stderr, "%s: %s:%lu: extraneous 'else'\n", program_name, lbuf->filename, line_no);
            }
            
            if (strncmp (p, "ifeq", 4) == 0 || strncmp (p, "ifneq", 5) == 0 || strncmp (p, "ifdef", 5) == 0 || strncmp (p, "ifndef", 6) == 0) {
                after_else = 1;
            } else {
            
                if (*p) {
                    fprintf (stderr, "%s: %s:%lu: extraneous text after 'else' directive\n", program_name, lbuf->filename, line_no);
                }
                
                if (cur_if_stack->has_else) {
                
                    fprintf (stderr, "%s: %s:%lu: *** only one 'else' per conditional. Stop.\n", program_name, lbuf->filename, line_no);
                    exit (EXIT_FAILURE);
                
                }
                
                cur_if_stack->ignoring = (cur_if_stack->prev_ignoring || !cur_if_stack->ignoring);
                cur_if_stack->has_else = 1;
                
                continue;
            
            }
        
        }
        
        if (strncmp (p, "ifeq", 4) == 0 || strncmp (p, "ifneq", 5) == 0) {
        
            int ifneq;
            
            if (strncmp (p, "ifeq", 4) == 0) {
            
                ifneq = 0;
                p += 4;
            
            } else {
            
                ifneq = 1;
                p += 5;
            
            }
            
            if (*p == '(' || *skip_whitespace (p) == '(') {
            
                q = skip_whitespace (skip_whitespace (p) + 1);
                
                if ((comma = strrchr (p, ','))) {
                
                    if (!isspace ((int) *p)) {
                    
                        fprintf (stderr, "%s: %s:%lu: *** missing separator (%s must be followed by whitespace). Stop.\n", program_name, lbuf->filename, line_no, ifneq ? "ifneq" : "ifeq");
                        exit (EXIT_FAILURE);
                    
                    }
                    
                    arg1 = p = q;
                    
                    if (!(q = strrchr (p, ')')) || q < comma) {
                        goto ifeq_invalid_syntax;
                    }
                    
                    *comma = '\0';
                    *q = '\0';
                    
                    for (q = comma; q > p && isspace ((int) *(q - 1));) {
                        *--q = '\0';
                    }
                    
                    for (p = comma + 1; isspace ((int) *p);) {
                        p++;
                    }
                    
                    arg2 = p;
                    
                    while (*p != '\0') {
                        p++;
                    }
                
                } else {
                
                    fprintf (stderr, "%s: %s:%lu: currently only ifeq (arg,arg) is supported\n", program_name, lbuf->filename, line_no);
                    exit (EXIT_FAILURE);
                
                }
            
            } else {
            
                if (!*skip_whitespace (p)) {
                    goto ifeq_invalid_syntax;
                }
                
                if (!isspace ((int) *p)) {
                
                    fprintf (stderr, "%s: %s:%lu: *** missing separator (%s must be followed by whitespace). Stop.\n", program_name, lbuf->filename, line_no, ifneq ? "ifneq" : "ifeq");
                    exit (EXIT_FAILURE);
                
                }
                
                p = skip_whitespace (p);
                
                if (*p == '"' || *p == '\'') {
                
                    ch = *p++;
                    arg1 = p;
                    
                    while (*p != ch) {
                    
                        if (*p == '\0') {
                            goto ifeq_invalid_syntax;
                        }
                        
                        p++;
                    
                    }
                    
                    *p = '\0';
                    p = skip_whitespace (p + 1);
                    
                    if (*p != '"' && *p != '\'') {
                        goto ifeq_invalid_syntax;
                    }
                    
                    ch = *p++;
                    arg2 = p;
                    
                    while (*p != ch) {
                    
                        if (*p == '\0') {
                            goto ifeq_invalid_syntax;
                        }
                        
                        p++;
                    
                    }
                    
                    *p++ = '\0';
                
                }
            
            }
            
            p = skip_whitespace (p);
            
            if (*p || !arg1 || !arg2) {
                fprintf (stderr, "%s: %s:%lu: extraneous text after '%s' directive\n", program_name, lbuf->filename, line_no, ifneq ? "ifneq" : "ifeq");
            }
            
            p = variable_expand_line (lbuf->filename, line_no, xstrdup (arg1));
            q = variable_expand_line (lbuf->filename, line_no, xstrdup (arg2));
            
            if (after_else) {
                cur_if_stack->prev_ignoring |= !cur_if_stack->ignoring;
            } else {
                add_if_stack ();
            }
            
            if (cur_if_stack->prev_ignoring) {
                cur_if_stack->ignoring = 1;
            } else {
            
                int is_equal = (strcmp (p, q) == 0);
                cur_if_stack ->ignoring = is_equal ^ (!ifneq);
            
            }
            
            free (p);
            free (q);
            
            continue;
            
        ifeq_invalid_syntax:
            
            fprintf (stderr, "%s: %s:%lu: *** invalid syntax in conditional. Stop.", program_name, lbuf->filename, line_no);
            exit (EXIT_FAILURE);
        
        }
        
        if (strncmp (p, ".ifdef", 6) == 0 || strncmp (p, "ifdef", 5) == 0 || strncmp (p, ".ifndef", 7) == 0 || strncmp (p, "ifndef", 6) == 0) {
        
            struct variable *var;
            int ifndef;
            
            if (*p == '.') {
                p++;
            }
            
            if (strncmp (p, "ifdef", 5) == 0) {
            
                ifndef = 0;
                p += 5;
            
            } else {
            
                ifndef = 1;
                p += 6;
            
            }
            
            if (!isspace ((int) *p)) {
            
                fprintf (stderr, "%s: %s:%lu: %s must be followed by whitespace. Stop.", program_name, lbuf->filename, line_no, ifndef ? "ifndef" : "ifdef");
                exit (EXIT_FAILURE);
            
            }
            
            p = line = variable_expand_line (lbuf->filename, line_no, xstrdup (skip_whitespace (p)));
            
            for (; isspace ((int) *p);) {
                p++;
            }
            
            for (q = p + strlen (p); q > p && isspace ((int) *(q - 1));) {
                *--q = '\0';
            }
            
            var = variable_find (p);
            
            if (after_else) {
                cur_if_stack->prev_ignoring |= !cur_if_stack->ignoring;
            } else {
                add_if_stack ();
            }
            
            if (cur_if_stack->prev_ignoring) {
                cur_if_stack->ignoring = 1;
            } else {
            
                int is_defined = 0;
                
                if (var) {
                    is_defined = 1;
                }
                
                cur_if_stack->ignoring = is_defined ^ (!ifndef);
            
            }
            
            continue;
        
        }
        
        if (strncmp (p, ".endif", 6) == 0 || strncmp (p, "endif", 5) == 0) {
        
            if (*p == '.') {
                p = skip_whitespace (p + 6);
            } else {
                p = skip_whitespace (p + 5);
            }
            
            if (!cur_if_stack) {
                fprintf (stderr, "%s: %s:%lu: extraneous 'endif'\n", program_name, lbuf->filename, line_no);
            } else {
            
                struct if_stack *prev = cur_if_stack->prev;
                
                free (cur_if_stack);
                cur_if_stack = prev;
            
            }
            
            if (*p) {
                fprintf (stderr, "%s: %s:%lu: extraneous text after 'endif' directive\n", program_name, lbuf->filename, line_no);
            }
            
            continue;
        
        }
        
    is_command:
        
        if (cur_if_stack && cur_if_stack->ignoring) {
            continue;
        }
        
        if (line[0] == ' ' || line[0] == '\t') {
        
            if (filenames) {
            
                while (line[0] == ' ' || line[0] == '\t') {
                    line++;
                }
                
                if (cmds_idx + strlen (line) + 1 > cmds_sz) {
                
                    cmds_sz = (cmds_idx + strlen (line) + 1) * 2;
                    cmds = xrealloc (cmds, cmds_sz);
                
                }
                
                src = line;
                dst = &cmds[cmds_idx];
                
                for (; *src; src++, dst++) {
                
                    if (src[0] == '\n' && src[-1] == '\\' && src[1] == '\t') {
                    
                        *dst = *src;
                        
                        src++;
                        continue;
                    
                    }
                    
                    *dst = *src;
                
                }
                
                *dst = '\0';
                
                cmds_idx += dst - &cmds[cmds_idx] + 1;
                cmds[cmds_idx - 1] = '\n';
                
                continue;
            
            }
        
        }
        
        if (strncmp (p, ".include", 8) == 0 || (strncmp (p, "include", 7) == 0 && (isspace ((int) p[7]) || p[7] == '\0'))) {
        
            int bsd = 0;
            
            if (*p == '.') {
            
                bsd = 1;
                p++;
            
            }
            
            p = skip_whitespace (p + 7);
            
            for (q = p + strlen (p); q > p && isspace ((int) q[-1]); q--) {
                ;
            }
            
            *q = '\0';
            p = line = variable_expand_line (lbuf->filename, line_no, xstrdup (p));
            
            while (1) {
            
                char saved_ch;
                
                for (; isspace ((int) *p); p++) {
                    ;
                }
                
                for (q = p; *q && !isspace ((int) *q); q++) {
                    ;
                }
                
                if (q == p) {
                    break;
                }
                
                saved_ch = *q;
                *q = '\0';
                
                if ((ret = include_makefile (lbuf->filename, line_no, p, bsd))) {
                    return ret;
                }
                
                *q = saved_ch;
                
                p = q;
            
            }
            
            free (line);
            continue;
        
        }
        
        if (*p == '$') {
        
            variable_expand_line (lbuf->filename, line_no, p);
            continue;
        
        } else if (strchr (p, '=')) {
        
            record_waiting_files ();
            
            parse_var_line (lbuf->filename, line_no, p, VAR_ORIGIN_FILE);
            continue;
        
        }
        
        record_waiting_files ();
        
        semicolonp = strchr (line, ';');
        commentp = strchr (line, '#');
        
        if (commentp && semicolonp && (commentp < semicolonp)) {
        
            *commentp = '\0';
            semicolonp = 0;
        
        } else if (semicolonp) {
            *(semicolonp++) = '\0';
        }
        
        remove_backslash_newlines (line);
        line = variable_expand_line (lbuf->filename, line_no, xstrdup (line));
        
        if (!(colon = strchr (line, ':'))) {
        
            if (*(p = skip_whitespace (line))) {
                fprintf (stderr, "%s: %s:%lu: missing ':' in rule line!\n", program_name, lbuf->filename, line_no);
            }
            
            free (line);
            continue;
        
        }
        
        *colon = '\0';
        
        filenames = parse_nameseq (line, sizeof (*filenames));
        depstr = xstrdup (colon + 1);
        
        free (line);
        
        if (semicolonp) {
        
            if (cmds_idx + strlen (semicolonp) + 1 > cmds_sz) {
            
                cmds_sz = (cmds_idx + strlen (semicolonp) + 1) * 2;
                cmds = xrealloc (cmds, cmds_sz);
            
            }
            
            memcpy (&(cmds[cmds_idx]), semicolonp, strlen (semicolonp) + 1);
            cmds_idx += strlen (semicolonp) + 1;
        
        }
        
        if (set_default && default_goal_var->value[0] == '\0') {
        
            struct nameseq *ns;
            
            for (ns = filenames; ns; ns = ns->next) {
            
                if ((ns->name[0] == '.') && strchr (ns->name, '\\') == 0 && strchr (ns->name, '/') == 0) {
                    continue;
                }
                
                free (default_goal_var->value);
                
                default_goal_var->value = xstrdup (ns->name);
                break;
            
            }
        
        }
    
    }
    
    record_waiting_files ();
    
    if (*(skip_whitespace (lbuf->start))) {
        line_no++;
    }
    
    free (clean);
    free (cmds);
    
    if (cur_if_stack) {
    
        struct if_stack *prev;
        
        for (; cur_if_stack; cur_if_stack = prev) {
        
            prev = cur_if_stack->prev;
            free (cur_if_stack);
        
        }
        
        fprintf (stderr, "%s: %s:%lu: *** missing 'endif'. Stop.\n", program_name, lbuf->filename, line_no);
        exit (EXIT_FAILURE);
    
    }
    
    return 0;

}

int read_makefile (const char *filename) {

    struct linebuf lbuf;
    struct variable *makefile_list;
    
    char *new_value;
    int ret;
    
    if (strcmp (filename, "-") == 0) {
    
        filename = "<stdin>";
        lbuf.f = stdin;
    
    } else if (!(lbuf.f = fopen (filename, "r"))) {
        return 1;
    }
    
    lbuf.size = 256;
    
    lbuf.filename = filename;
    lbuf.line_no = 1;
    
    lbuf.start = xmalloc (lbuf.size);
    
    if ((makefile_list = variable_find ("MAKEFILE_LIST"))) {
    
        unsigned long old_len = strlen (makefile_list->value);
        
        new_value = xmalloc (old_len + 1 + strlen (filename) + 1);
        sprintf (new_value, "%s %s", makefile_list->value, filename);
        
        variable_change ("MAKEFILE_LIST", new_value, VAR_ORIGIN_FILE);
    
    } else {
    
        new_value = xmalloc (strlen (filename) + 1);
        sprintf (new_value, "%s", filename);
        
        variable_change ("MAKEFILE_LIST", new_value, VAR_ORIGIN_FILE);
    
    }
    
    ret = read_lbuf (&lbuf, 1);
    free (lbuf.start);
    
    if (lbuf.f != stdin) {
        fclose (lbuf.f);
    }
    
    return ret;

}

void read_memory_makefile (const char *filename, unsigned long line_no, char *memory) {

    struct if_stack *saved_if_stack;
    struct linebuf lbuf;
    
    lbuf.size = 256;
    lbuf.f = 0;
    
    lbuf.start = xmalloc (lbuf.size);
    lbuf.memory = memory;
    
    lbuf.filename = filename;
    lbuf.line_no = line_no;
    
    saved_if_stack = cur_if_stack;
    cur_if_stack = 0;
    
    read_lbuf (&lbuf, 1);
    free (lbuf.start);
    
    cur_if_stack = saved_if_stack;

}

void *parse_nameseq (char *line, size_t size) {

    struct nameseq *start = 0;
    struct nameseq **pp = &start;
    
    char *p, *temp;
    temp = xmalloc (strlen (line) + 1);
    
#define     add_nameseq(_name)                              \
    do {                                                    \
        *pp = xmalloc (size);                               \
        (*pp)->name = xstrdup (_name);                      \
        pp = &((*pp)->next);                                \
    } while (0)
    
    p = line;
    
    while (1) {
    
        char *p2;
        
        while (isspace ((int) *p)) {
            p++;
        }
        
        if (*p == '\0') {
            break;
        }
        
        p2 = p;
        
        while (*p2 && !isspace ((int) *p2)) {
            p2++;
        }
        
        memcpy (temp, p, p2 - p);
        temp[p2 - p] = '\0';
        
        add_nameseq (temp);
        p = p2;
    
    }
    
#undef      add_nameseq
    
    free (temp);
    return start;

}

void record_files (const char *filename, unsigned long line_no, struct nameseq *filenames, char *cmds, size_t cmds_idx, char *depstr) {

    struct commands *cmds_p;
    
    struct dep *deps;
    struct nameseq *ns, *old_ns;
    
    if (cmds_idx > 0) {
    
        cmds_p = xmalloc (sizeof (*cmds_p));
        
        cmds_p->text = xstrndup (cmds, cmds_idx);
        cmds_p->len = cmds_idx;
    
    } else {
        cmds_p = 0;
    }
    
    if (depstr) {
        deps = parse_nameseq (depstr, sizeof (*deps));
    } else {
        deps = 0;
    }
    
    for (ns = filenames, old_ns = 0; ns; old_ns = ns, ns = ns->next, free (old_ns->name), free (old_ns)) {
    
        if (ns->name[0] == '.' && !strchr (ns->name, '\\') && !strchr (ns->name, '/')) {
            rule_add_suffix (filename, line_no - 1, ns->name, cmds_p);
        } else {
            rule_add (filename, line_no - 1, ns->name, deps, cmds_p);
        }
    
    }
    
    free (depstr);

}
#undef linebuf 
/******************************************************************************
 * @file            report.c
 *****************************************************************************/
#include    <stdarg.h>
#include    <stdio.h>
#include    <string.h>

unsigned long errors = 0;

#ifndef     __PDOS__
#if     defined (_WIN32)
# include   <windows.h>
static int OriginalConsoleColor = -1;
#endif

static void reset_console_color (void) {

#if     defined (_WIN32)

    HANDLE hStdError = GetStdHandle (STD_ERROR_HANDLE);
    
    if (OriginalConsoleColor == -1) { return; }
    
    SetConsoleTextAttribute (hStdError, OriginalConsoleColor);
    OriginalConsoleColor = -1;

#else

    fprintf (stderr, "\033[0m");

#endif

}

static void set_console_color (int color) {

#if     defined (_WIN32)

    HANDLE hStdError = GetStdHandle (STD_ERROR_HANDLE);
    WORD wColor;
    
    if (OriginalConsoleColor == -1) {
    
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        
        if (!GetConsoleScreenBufferInfo (hStdError, &csbi)) {
            return;
        }
        
        OriginalConsoleColor = csbi.wAttributes;
    
    }
    
    wColor = (OriginalConsoleColor & 0xF0) + (color & 0xF);
    SetConsoleTextAttribute (hStdError, wColor);

#else

    fprintf (stderr, "\033[%dm", color);

#endif

}
#endif

static void output_message (const char *filename, unsigned long lineno, enum report_type type, const char *fmt, va_list ap) {

    if (filename) {
    
        if (lineno == 0) {
            fprintf (stderr, "%s: ", filename);
        } else {
            fprintf (stderr, "%s:", filename);
        }
    
    }
    
    if (lineno > 0) {
        fprintf (stderr, "%lu: ", lineno);
    }
    
    if (type == REPORT_ERROR || type == REPORT_FATAL_ERROR) {
    
#ifndef     __PDOS__
        set_console_color (COLOR_ERROR);
#endif
        
        if (type == REPORT_ERROR) {
            fprintf (stderr, "error:");
        } else {
            fprintf (stderr, "fatal error:");
        }
    
    } else if (type == REPORT_INTERNAL_ERROR) {
    
#ifndef     __PDOS__
        set_console_color (COLOR_INTERNAL_ERROR);
#endif
        
        fprintf (stderr, "internal error:");
    
    } else if (type == REPORT_WARNING) {
    
#ifndef     __PDOS__
        set_console_color (COLOR_WARNING);
#endif
        
        fprintf (stderr, "warning:");
    
    }
    
#ifndef     __PDOS__
    reset_console_color ();
#endif
    
    fprintf (stderr, " ");
    vfprintf (stderr, fmt, ap);
    fprintf (stderr, "\n");
    
    if (type != REPORT_WARNING) {
        ++errors;
    }

}

unsigned long get_error_count (void) {
    return errors;
}

void report_at (const char *filename, unsigned long lineno, enum report_type type, const char *fmt, ...) {

    va_list ap;
    
    va_start (ap, fmt);
    output_message (filename, lineno, type, fmt, ap);
    va_end (ap);

}
/******************************************************************************
 * @file            rule.c
 *****************************************************************************/
#include    <stdlib.h>
#include    <string.h>


static struct hashtab hashtab_rules = { 0 };
struct suffix_rule *suffix_rules = 0;

struct rule *rule_find (const char *name) {

    struct hashtab_name *key;
    
    char *hash_tagged_name = 0;
    char *dot = 0, *ptr = 0;
    
    if (!(key = hashtab_get_key (&hashtab_rules, name))) {
    
        hash_tagged_name = xstrdup (name);
        
        /* Find the begining of the filename. */
        if (!(ptr = strrchr (hash_tagged_name, '/'))) {
            ptr = strrchr (hash_tagged_name, '\\');
        }
        
        if (!ptr) {
            ptr = hash_tagged_name;
        } else {
            ptr++;
        }
        
        /* Replace filename by '%'. */
        if (*ptr) {
        
            dot = strrchr (ptr, '.');
            *ptr++ = '%';
            
            if (dot) {
                memmove (ptr, dot, strlen (dot) + 1);
            } else {
                *ptr = '\0';
            }
        
        }
        
        key = hashtab_get_key (&hashtab_rules, hash_tagged_name);
        free (hash_tagged_name);
        
        if (!key) {
            return 0;
        }
    
    }
    
    return hashtab_get (&hashtab_rules, key);

}

void rule_add (const char *filename, unsigned long line_no, char *name, struct dep *deps, struct commands *cmds) {

    struct hashtab_name *key;
    struct rule *r;
    
    if (!(key = hashtab_alloc_name (xstrdup (name)))) {
        return;
    }
    
    r = xmalloc (sizeof (*r));
    
    r->name = xstrdup (name);
    r->deps = deps;
    r->cmds = cmds;
    
    r->filename = xstrdup (filename);
    r->line_no = line_no;
    
    hashtab_put (&hashtab_rules, key, r);

}

void rule_add_suffix (const char *filename, unsigned long line_no, char *name, struct commands *cmds) {

    struct suffix_rule *s = xmalloc (sizeof (*s));
    char *p;
    
    if ((p = strchr (name + 1, '.'))) {
    
        s->second = xstrdup (p);
        *p = '\0';
    
    }
    
    s->first = xstrdup (name);
    
    s->cmds = cmds;
    s->next = suffix_rules;
    
    s->filename = xstrdup (filename);
    s->line_no = line_no;
    
    suffix_rules = s;

}

void rules_init (void) {}
#define linebuf linebuf2
#define internal_commands internal_commands1
/******************************************************************************
 * @file            variable.c
 *****************************************************************************/
#if     defined (__GNUC__)
# ifndef    _POSIX_C_SOURCE
#  define       _POSIX_C_SOURCE         200112L                                 /* for setenv() */
# endif
#endif

#include    <ctype.h>
#include    <stdlib.h>
#include    <string.h>

#if     defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
# ifndef    __USE_POSIX
#  define       __USE_POSIX
# endif
#elif   defined (_WIN32)
# define        strcasecmp              _stricmp
#endif

#include    <stdio.h>


struct linebuf {

    char *start;
    unsigned long size;
    
    FILE *f;

};

struct builtin_function {

    const char *name;
    char *(*func) (const char *filename, unsigned long line_no, char *input);

};

static char *func_eval (const char *filename, unsigned long line_no, char *input) {

    read_memory_makefile (filename, line_no, input);
    return 0;

}

static char *func_error (const char *filename, unsigned long line_no, char *input) {

    fprintf (stderr, "%s: %s:%lu: *** %s. Stop.\n", program_name, filename, line_no, input);
    exit (EXIT_FAILURE);
    
    return 0;       /* Needed by wcl */

}

static char *func_subst (const char *filename, unsigned long line_no, char *input) {

    char *src, *dst, *new_body, *old_body, *p;
    unsigned long size, pos, l, dst_len, src_len;
    
    (void) filename;
    (void) line_no;
    
    src = input;
    
    if (!(dst = strstr (src, ",")) || !dst[0]) {
        return 0;
    }
    
    *dst++ = 0;
    
    if (!(old_body = strstr (dst, ",")) || !old_body[0]) {
        return 0;
    }
    
    *old_body++ = 0;
    
    size = 16 + strlen (old_body);
    new_body = xmalloc (size);
    
    dst_len = strlen (dst);
    src_len = strlen (src);
    
    pos = 0;
    
    while (*old_body) {
    
        if ((p = strstr (old_body, src))) {
        
            l = p - old_body;
            
            if (l + dst_len + 1 >= size - pos) {
            
                size += l + dst_len + 1 + strlen (old_body);
                new_body = xrealloc (new_body, size);
            
            }
            
            memcpy (new_body + pos, old_body, l);
            pos += l;
            
            memcpy (new_body + pos, dst, dst_len + 1);
            pos += dst_len;
            
            old_body = p + src_len;
        
        } else {
        
            if (strlen (old_body) >= size - pos) {
            
                size += strlen (old_body) + size - pos + 1;
                new_body = xrealloc (new_body, size);
            
            }
            
            memcpy (new_body + pos, old_body, strlen (old_body) + 1);
            break;
        
        }
    
    }
    
    return new_body;

}

#if         defined (_WIN32) || defined (__WIN32__)
# include       <windows.h>
static char *internal_commands[] = {

    "assoc",
    "break",
    "call",
    "cd",
    "chcp",
    "chdir",
    "cls",
    "color",
    "copy",
    "ctty",
    "date",
    "del",
    "dir",
    "echo",
    "echo.",
    "endlocal",
    "erase",
    "exit",
    "for",
    "ftype",
    "goto",
    "if",
    "md",
    "mkdir",
    "move",
    "pause",
    "prompt",
    "rd",
    "rem",
    "ren",
    "rename",
    "rmdir",
    "set",
    "setlocal",
    "shift",
    "time",
    "title",
    "type",
    "ver",
    "verify",
    "vol",
    0

};

static char *func_shell (const char *filename, unsigned long line_no, char *input) {

    DWORD dwExitCode;
    
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    
    HANDLE hStdInPipeRead, hStdInPipeWrite;
    HANDLE hStdOutPipeRead, hStdOutPipeWrite;
    
    char *lpApplicationName = 0, *lpCommandLine = 0;
    char *p, ch;
    
    struct linebuf lbuf;
    input = skip_whitespace (input);
    
    /*for (p = input; !isspace ((int) *p); p++) {
    
        if (*p == '/') {
            *p = '\\';
        }
    
    }*/
    
    if ((p = strchr (input, ' '))) {
    
        lpApplicationName = xstrndup (input, p - input);
        lpCommandLine = xstrdup (skip_whitespace (p));
    
    } else {
        lpApplicationName = xstrdup (skip_whitespace (input));
    }
    
    if (!(p = strchr (lpApplicationName, '\\'))) {
    
        unsigned long count = sizeof (internal_commands) / sizeof (internal_commands[0]), i;
        
        for (i = 0; i < count; i++) {
        
            if (strcasecmp (lpApplicationName, internal_commands[i]) == 0) {
            
                if (lpApplicationName) {
                    free (lpApplicationName);
                }
                
                if (lpCommandLine) {
                    free (lpCommandLine);
                }
                
                lpCommandLine = xmalloc (4 + strlen (input) + 1);
                sprintf (lpCommandLine, "/C %s", input);
                
                lpApplicationName = xstrdup (getenv ("COMSPEC"));
                break;
            
            }
        
        }
    
    }/* else {
    
        if (lpApplicationName) {
            free (lpApplicationName);
        }
        
        lpApplicationName = 0;
    
    }*/
    
    memset (&sa, 0, sizeof (sa));
    memset (&si, 0, sizeof (si));
    memset (&pi, 0, sizeof (pi));
    
    sa.nLength = sizeof (sa);
    sa.bInheritHandle = 1;
    
    if (!CreatePipe (&hStdInPipeRead, &hStdInPipeWrite, &sa, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 0;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    if (!SetHandleInformation (hStdInPipeRead, HANDLE_FLAG_INHERIT, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 0;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    if (!CreatePipe (&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 0;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    if (!SetHandleInformation (hStdOutPipeRead, HANDLE_FLAG_INHERIT, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 0;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    si.cb = sizeof (si);
    
    si.hStdError = hStdOutPipeWrite;
    si.hStdOutput = hStdOutPipeWrite;
    si.hStdInput = hStdInPipeRead;
    
    si.dwFlags |= STARTF_USESTDHANDLES;
    
    if (!CreateProcess (lpApplicationName, lpCommandLine, 0, 0, 1, 0, 0, 0, &si, &pi)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 0;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    CloseHandle (hStdOutPipeWrite);
    CloseHandle (hStdInPipeRead);
    
    /*WaitForSingleObject (pi.hProcess, INFINITE);*/
    lbuf.start = xmalloc (lbuf.size = 256);
    
    {
    
        char *end = lbuf.start + lbuf.size;
        char *p = lbuf.start;
        
        DWORD dwRead;
        
        while (ReadFile (hStdOutPipeRead, p, end - p, &dwRead, 0)) {
        
            size_t offset;
            p += dwRead;
            
            offset = p - lbuf.start;
            
            if (end - p >= 80) {
                continue;
            }
            
            lbuf.size *= 2;
            lbuf.start = xrealloc (lbuf.start, lbuf.size);
            
            p = lbuf.start + offset;
            end = lbuf.start + lbuf.size;
        
        }
    
    }
    
    CloseHandle (hStdOutPipeRead);
    CloseHandle (hStdInPipeWrite);
    
    GetExitCodeProcess (pi.hProcess, &dwExitCode);
    
    CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
    
    if (dwExitCode) {
    
        fprintf (stderr, "%s\n", lbuf.start);
        exit (EXIT_FAILURE);
    
    }
    
    while (isspace ((int) *lbuf.start)) {
        lbuf.start++;
    }
    
    p = lbuf.start;
    
    while ((ch = *p)) {
    
        if (ch == '\n') {
        
            memmove (p, p + 1, strlen (p + 1));
            *(p + strlen (p) - 1) = '\0';
            
            if (p[-1] == '\r') {
                p[-1] = ' ';
            }
            
            continue;
        
        }
        
        p++;
    
    }
    
    if (isspace ((int) *(p - 1))) {
        *(p - 1) = '\0';
    }
    
    return lbuf.start;

}
#elif   defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
# include       <sys/wait.h>
# include       <errno.h>
# include       <unistd.h>
static char *func_shell (const char *filename, unsigned long line_no, char *input) {

    int pipefd[2], pid, status;
    char *p, ch;
    
    struct linebuf lbuf;
    FILE *cmd_output;
    
    if (pipe (pipefd) < 0) {
    
        perror ("pipe");
        exit (EXIT_FAILURE);
    
    }
    
    if ((pid = fork ()) < 0) {
    
        perror ("fork");
        exit (EXIT_FAILURE);
    
    }
    
    input = skip_whitespace (input);
    
    if (pid == 0) {
    
        char *name = input, *space = 0, *slash;
        
        if ((space = strchr (name, ' '))) {
            name = xstrndup (name, space - name);
        }
        
        dup2 (pipefd[1], STDOUT_FILENO);
        
        close (pipefd[0]);
        close (pipefd[1]);
        
        execlp (name, (slash = strrchr (name, '/')) ? (slash + 1) : name, space ? skip_whitespace (space) : NULL, NULL);
        
        fprintf (stderr, "%s: %s:%lu: %s: %s\n", program_name, filename, line_no, name, strerror (errno));
        exit (EXIT_FAILURE);
    
    }
    
    close (pipefd[1]);
    
    if (!(cmd_output = fdopen (pipefd[0], "r"))) {
    
        perror ("fdopen");
        exit (EXIT_FAILURE);
    
    }
    
    lbuf.start = xmalloc (lbuf.size = 256);
    
    {
    
        char *end = lbuf.start + lbuf.size;
        char *p = lbuf.start;
        
        while (fgets (p, end - p, cmd_output)) {
        
            size_t offset;
            p += strlen (p);
            
            offset = p - lbuf.start;
            
            if (end - p >= 80) {
                continue;
            }
            
            lbuf.size *= 2;
            lbuf.start = xrealloc (lbuf.start, lbuf.size);
            
            p = lbuf.start + offset;
            end = lbuf.start + lbuf.size;
        
        }
    
    }
    
    if (waitpid (pid, &status, 0) == -1) {
    
        perror ("waitpid");
        exit (EXIT_FAILURE);
    
    }
    
    if (status) {
    
        char *p = (lbuf.start = skip_whitespace (lbuf.start));
        if (*p != '\0') { fprintf (stderr, "%s\n", p); }
        
        exit (EXIT_FAILURE);
    
    }
    
    p = lbuf.start;
    
    while ((ch = *p)) {
    
        if (ch == '\n' || ch == '\r') {
            *p = ' ';
        }
        
        p++;
    
    }
    
    if (isspace ((int) *(p - 1))) {
        *(p - 1) = '\0';
    }
    
    return lbuf.start;

}
#endif

static struct builtin_function builtin_functions[] ={

    {   "error",    &func_error     },
    
#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
    {   "shell",    &func_shell     },
#endif
    
    {   "eval",     &func_eval      },
    {   "subst",    &func_subst     },
    
    {   0,          0               }

};

static struct hashtab hashtab_builtin = { 0 };
static struct hashtab hashtab_vars = { 0 };

static struct builtin_function *find_builtin_function (const char *name) {

    struct hashtab_name *key;
    
    if ((key = hashtab_get_key (&hashtab_builtin, name))) {
        return hashtab_get (&hashtab_builtin, key);
    }
    
    return 0;

}

static char *variable_suffix_replace (char *body, const char *from_s, const char *to_s) {

    char *new_body = xstrdup (body);
    char *p;
    
    while ((p = strstr (new_body, from_s))) {
    
        if (strlen (from_s) == strlen (to_s)) {
            memcpy (p, to_s, strlen (to_s));
        } else if (strlen (from_s) > strlen (to_s)) {
        
            size_t rem = strlen (from_s) - strlen (to_s);
            memcpy (p, to_s, strlen (to_s));
            
            while (rem--) {
                p[strlen (to_s) + rem] = ' ';
            }
        
        } else {
        
            size_t rem = strlen (to_s) - strlen (from_s);
            
            new_body = xrealloc (new_body, strlen (new_body) + rem + 1);
            memmove (p + rem, p, strlen (p) + 1);
            memcpy (p, to_s, strlen (to_s));
        
        }
    
    }
    
    return new_body;

}

struct variable *variable_add (char *name, char *value, enum variable_origin origin) {

    struct hashtab_name *key;
    struct variable *var;
    
    if (!(key = hashtab_alloc_name (name))) {
    
        report_at (program_name, 0, REPORT_ERROR, "failed to allocate memory for name '%s'", name);
        exit (EXIT_FAILURE);
    
    }
    
    var = xmalloc (sizeof (*var));
    var->flavor = VAR_FLAVOR_RECURSIVELY_EXPANDED;
    var->name = name;
    var->origin = origin;
    var->value = value;
    
    if (hashtab_put (&hashtab_vars, key, var)) {
    
        report_at (program_name, 0, REPORT_FATAL_ERROR, "failed to insert variable '%s' into hashtab", var->name);
        exit (EXIT_FAILURE);
    
    }
    
    return var;

}

struct variable *variable_change (char *name, char *value, enum variable_origin origin) {

    struct variable *var;
    
    if (!(var = variable_find (name))) {
        return variable_add (xstrdup (name), value, origin);
    }
    
    switch (origin) {
    
        case VAR_ORIGIN_AUTOMATIC:
        case VAR_ORIGIN_COMMAND_LINE:
        
            break;
        
        case VAR_ORIGIN_FILE:
        
            if (var->origin == VAR_ORIGIN_FILE) {
                break;
            }
            
            free (value);
            return 0;
    
    }
    
    free (var->value);
    
    var->value = value;
    var->origin = origin;
    
    return var;

}

struct variable *variable_find (char *name) {

    struct hashtab_name *key;
    
    if (!(key = hashtab_get_key (&hashtab_vars, name))) {
        return 0;
    }
    
    return hashtab_get (&hashtab_vars, key);

}

char *variable_expand_line (const char *filename, unsigned long line_no, char *line) {

    size_t pos = 0;
    
    while (line[pos]) {
    
        if (line[pos] == '$' || (line[pos] == '%' && variable_find ("%"))) {
        
            char *new, *replacement = "";
            char *p_after_variable;
            
            struct variable *var = 0;
            char *alloc_replacement = 0;
            
            struct builtin_function *func;
            char saved_ch;
            
            if (line[pos] == '%') {
            
                var = variable_find ("%");
                p_after_variable = line + pos + 1;
            
            } else if (line[pos + 1] == '$') {
            
                p_after_variable = line + pos + 2;
                pos += 1;
            
            } else if (line[pos + 1] == '(' || line[pos + 1] == '{') {
            
                char *body = line + pos + 2;
                char *q = body, *content;
                
                int paren_inbalance = 1;
                char opening_paren = line[pos + 1];
                
                while (paren_inbalance) {
                
                    if (*q == opening_paren) {
                        paren_inbalance++;
                    } else if (*q == ')' && opening_paren == '(') {
                        paren_inbalance--;
                    } else if (*q == '}' && opening_paren == '{') {
                        paren_inbalance--;
                    } else if (*q == '\0') {
                    
                        fprintf (stderr, "%s: *** unterminated variable reference. Stop.\n", program_name);
                        exit (EXIT_FAILURE);
                    
                    }
                    
                    q++;
                
                }
                
                q--;
                
                p_after_variable = q + 1;
                content = variable_expand_line (filename, line_no, xstrndup (body, q - body));
                
                for (q = content; *q && !isspace ((int) *q);) {
                    q++;
                }
                
                saved_ch = *q;
                *q = '\0';
                
                func = find_builtin_function (content);
                *q = saved_ch;
                
                if (func) {
                
                    for (; isspace ((int) *q);) {
                        q++;
                    }
                    
                    /*if ((alloc_replacement = func->func (filename, line_no, q))) {
                    
                        new = xmalloc (pos + strlen (alloc_replacement) + 1);
                        
                        memcpy (new, line, pos);
                        memcpy (new + pos, alloc_replacement, strlen (alloc_replacement));
                        
                        free (alloc_replacement);
                        return new;
                    
                    }*/
                    
                    alloc_replacement = func->func (filename, line_no, q);
                
                } else if ((q = strchr (content, '='))) {
                
                    char *colon = strchr (content, ':');
                    
                    if (colon && colon != content && colon < q) {
                    
                        char *from, *to, *p;
                        *colon = '\0';
                        
                        var = variable_find (content);
                        
                        for (from = colon + 1; isspace ((int) *from); from++) {
                            ;
                        }
                        
                        for (p = q; p != from && isspace ((int) p[-1]); p--) {
                            ;
                        }
                        
                        *p = '\0';
                        
                        for (to = q + 1; isspace ((int) *to); to++) {
                            ;
                        }
                        
                        for (p = to + strlen (to); p != to && isspace ((int) p[-1]); p--) {
                            ;
                        }
                        
                        *p = '\0';
                        
                        if (*from && var) {
                            alloc_replacement = variable_suffix_replace (var->value, from, to);
                        }
                    
                    }
                
                } else {
                
                    if (state->env_override || strcmp (content, "PATH") == 0) {
                    
                        if ((alloc_replacement = getenv (content))) {
                        
                            alloc_replacement = xstrdup (alloc_replacement);
                            goto got_value;
                        
                        }
                    
                    }
                    
                    var = variable_find (content);
                
                }
                
            got_value:
                
                free (content);
            
            } else if (line[pos + 1]) {
            
                char name[2] = { 0, 0 };
                
                p_after_variable = line + pos + 2;
                name[0] = line[pos + 1];
                
                var = variable_find (name);
            
            } else {
                p_after_variable = line + pos + 1;
            }
            
            if (var) {
                replacement = var->value;
            }
            
            if (alloc_replacement) {
                replacement = alloc_replacement;
            }
            
            new = xmalloc (pos + strlen (replacement) + strlen (p_after_variable) + 1);
            
            memcpy (new, line, pos);
            memcpy (new + pos, replacement, strlen (replacement));
            memcpy (new + pos + strlen (replacement), p_after_variable, strlen (p_after_variable) + 1);
            
            free (line);
            line = new;
            
            if (!alloc_replacement && var && var->flavor == VAR_FLAVOR_SIMPLY_EXPANDED) {
                pos += strlen (replacement);
            }
            
            if (alloc_replacement) {
                free (alloc_replacement);
            }
            
            continue;
            
        
        }
        
        pos++;
    
    }
    
    return line;

}

void parse_var_line (const char *filename, unsigned long line_no, char *line, enum variable_origin origin) {

    enum {
    
        VAR_ASSIGN,
        VAR_CONDITIONAL_ASSIGN,
        VAR_APPEND,
        VAR_SHELL
    
    } opt = VAR_ASSIGN;
    
    enum variable_flavor flavor = VAR_FLAVOR_RECURSIVELY_EXPANDED;
    struct variable *var;
    
    char *var_name, *new_value;
    char *equals_sign, *p;
    
    if (!(equals_sign = strchr (line, '='))) {
    
        fprintf (stderr, "+++ invalid variable definition!\n");
        return;
    
    }
    
    p = equals_sign;
    
    switch (p - line) {
    
        default:
        
            if (p[-1] == ':' && p[-2] == ':' && p[-3] == ':') {
            
                flavor = VAR_FLAVOR_IMMEDIATELY_EXPANDED;
                
                p -= 3;
                break;
            
            }
            
            /* fall through */
        
        case 2:
        
            if (p[-1] == ':' && p[-2] == ':') {
            
                flavor = VAR_FLAVOR_SIMPLY_EXPANDED;
                
                p -= 2;
                break;
            
            }
            
            /* fall through */
        
        case 1:
        
            if (p[-1] == ':') {
            
                flavor = VAR_FLAVOR_SIMPLY_EXPANDED;
                
                p--;
                break;
            
            }
            
            if (p[-1] == '?') {
            
                opt = VAR_CONDITIONAL_ASSIGN;
                
                p--;
                break;
            
            }
            
            if (p[-1] == '+') {
            
                opt = VAR_APPEND;
                
                p--;
                break;
            
            }
            
            if (p[-1] == '!') {
            
                opt = VAR_SHELL;
                
                p--;
                break;
            
            }
            
            break;
        
        case 0:
        
            break;
    
    }
    
    for (; p > line && isspace ((int) p[-1]); p--) {
        ;
    }
    
    var_name = variable_expand_line (filename, line_no, xstrndup (line, p - line));
    
    if (*var_name == '\0') {
    
        fprintf (stderr, "%s: *** empty variable name. Stop.\n", program_name);
        exit (EXIT_FAILURE);
    
    }
    
    var = variable_find (var_name);
    
    if (opt == VAR_CONDITIONAL_ASSIGN && var) {
    
        free (var_name);
        return;
    
    }
    
    for (p = equals_sign; isspace ((int) p[1]); p++) {
        ;
    }
    
    new_value = xstrdup (p + 1);
    
    if (opt == VAR_ASSIGN || opt == VAR_CONDITIONAL_ASSIGN) {
    
        switch (flavor) {
        
            case VAR_FLAVOR_RECURSIVELY_EXPANDED:
            
                break;
            
            case VAR_FLAVOR_SIMPLY_EXPANDED:
            
                new_value = variable_expand_line (filename, line_no, new_value);
                break;
            
            case VAR_FLAVOR_IMMEDIATELY_EXPANDED: {
            
                size_t dollar_count;
                char *temp, *p2;
                
                new_value = variable_expand_line (filename, line_no, new_value);
                
                for (dollar_count = 0, p = new_value; *p; p++) {
                
                    if (*p == '$') {
                        dollar_count++;
                    }
                
                }
                
                temp = xmalloc (strlen (new_value) + 1 + dollar_count);
                
                for (p = new_value, p2 = temp; *p; p++, p2++) {
                
                    *p2 = *p;
                    
                    if (*p == '$') {
                    
                        p2[1] = '$';
                        p2++;
                    
                    }
                    
                    *p2 = '\0';
                    
                    free (new_value);
                    new_value = temp;
                
                }
                
                break;
            
            }
        
        }
    
    } else if (opt == VAR_APPEND) {
    
        struct variable *var;
        
        if ((var = variable_find (var_name))) {
        
            char *temp = xstrdup (new_value);
            free (new_value);
            
            new_value = xmalloc (strlen (var->value) + 1 + strlen (temp) + 1);
            sprintf (new_value, "%s %s", var->value, temp);
            
            free (temp);
        
        }
    
    } else if (opt == VAR_SHELL) {
    
#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)

        char *temp = xstrdup (new_value);
        free (new_value);
        
        new_value = variable_expand_line (filename, line_no, func_shell (filename, line_no, temp));
        free (temp);

#else
        
        fprintf (stderr, "%s: 'MACRO != value' isn't supported due to OS limitations. Stop.\n", program_name);
        exit (EXIT_FAILURE);
        
#endif
    
    }
    
    if (strcmp (var_name, "PATH") == 0) {
    
#if     defined (_WIN32)

        char *temp = xmalloc (strlen (new_value) + 6);
        int i;
        
        sprintf (temp, "PATH=%s", new_value);
        
        for (i = 5; temp[i] != '\0'; i++) {
        
            if (temp[i] == ':' && temp[i + 1] != '\\') {
                temp[i] = ';';
            }
        
        }
        
        _putenv (temp);
        free (temp);

#else
        setenv ("PATH", new_value, 1);
#endif    
    
    }
    
    if ((var = variable_change (var_name, new_value, origin))) {
        var->flavor = flavor;
    }
    
    free (var_name);

}

void variables_init (void) {

    struct builtin_function *builtin;
    struct hashtab_name *key;
    
    for (builtin = builtin_functions; builtin->name; builtin++) {
    
        if (!(key = hashtab_alloc_name (builtin->name))) {
            continue;
        }
        
        hashtab_put (&hashtab_builtin, key, builtin);
    
    }

}
#undef linebuf 
#undef internal_commands
/******************************************************************************
 * @file            xmake.c
 *****************************************************************************/
#include    <ctype.h>
#include    <stdlib.h>
#include    <string.h>

#if     defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
#elif   defined (_WIN32)
# define        strcasecmp              _stricmp
#endif

#include    <stdio.h>


struct variable *default_goal_var = 0;

struct xmake_state *state = 0;
const char *program_name = 0;

#if     defined (__PDOS__)
const char *os_name = "PDOS";
#elif   defined (_WIN32) || defined (__WIN32__)
const char *os_name = "Windows_NT";
#elif   defined (__MSDOS__)
const char *os_name = "MSDOS";
#elif   defined (__APPLE__)
const char *os_name = "Apple";
#elif   defined (__linux__)
const char *os_name = "Linux";
#elif   defined (unix) || defined (__unix) || defined (__unix__)
const char *os_name = "Unix";
#else
const char *os_name = "";
#endif

struct linebuf {

    char *start;
    unsigned long size;
    
    FILE *f;

};

static int doing_inference_rule_commands = 0;

#if         defined (_WIN32) || defined (__WIN32__)
# include       <windows.h>
static char *internal_commands[] = {

    "assoc",
    "break",
    "call",
    "cd",
    "chcp",
    "chdir",
    "cls",
    "color",
    "copy",
    "ctty",
    "date",
    "del",
    "dir",
    "echo",
    "echo.",
    "endlocal",
    "erase",
    "exit",
    "for",
    "ftype",
    "goto",
    "if",
    "md",
    "mkdir",
    "move",
    "pause",
    "prompt",
    "rd",
    "rem",
    "ren",
    "rename",
    "rmdir",
    "set",
    "setlocal",
    "shift",
    "time",
    "title",
    "type",
    "ver",
    "verify",
    "vol",
    0

};

static int pipe_command (const char *filename, unsigned long line_no, char *input, int is_ignoring_errors, const char *name) {

    DWORD dwExitCode = 0;
    
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    
    HANDLE hStdInPipeRead, hStdInPipeWrite;
    HANDLE hStdOutPipeRead, hStdOutPipeWrite;
    
    char *lpApplicationName = 0, *lpCommandLine = 0;
    char *p;
    
    struct linebuf lbuf;
    int internal = 0;
    
    input = skip_whitespace (input);
    
    if ((p = strchr (input, ' '))) {
    
        lpApplicationName = xstrndup (input, p - input);
        lpCommandLine = xstrdup (skip_whitespace (p));
    
    } else {
        lpApplicationName = xstrdup (skip_whitespace (input));
    }
    
    if (!(p = strchr (lpApplicationName, '\\'))) {
    
        unsigned long count = sizeof (internal_commands) / sizeof (internal_commands[0]) - 1;
        unsigned long i;
        
        for (i = 0; i < count; i++) {
        
            if (strcasecmp (lpApplicationName, internal_commands[i]) == 0) {
            
                if (lpApplicationName) {
                    free (lpApplicationName);
                }
                
                if (lpCommandLine) {
                    free (lpCommandLine);
                }
                
                lpCommandLine = xmalloc (4 + strlen (input) + 1);
                sprintf (lpCommandLine, "/C %s", input);
                
                lpApplicationName = xstrdup (getenv ("COMSPEC"));
                internal = 1;
                
                break;
            
            }
        
        }
    
    }
    
    if (!internal) {
    
        if (!lpCommandLine) {
            lpCommandLine = xstrdup ("");
        }
        
        p = xmalloc (strlen (lpApplicationName) + strlen (lpCommandLine) + 8);
        sprintf (p, "/C %s %s", lpApplicationName, lpCommandLine);
        
        free (lpCommandLine);
        lpCommandLine = p;
    
    }
    
    memset (&sa, 0, sizeof (sa));
    memset (&si, 0, sizeof (si));
    memset (&pi, 0, sizeof (pi));
    
    sa.nLength = sizeof (sa);
    sa.bInheritHandle = 1;
    
    if (!CreatePipe (&hStdInPipeRead, &hStdInPipeWrite, &sa, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 1;
        }
        
        exit (EXIT_FAILURE);
    
    }
    /*
    if (!SetHandleInformation (hStdInPipeRead, HANDLE_FLAG_INHERIT, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 1;
        }
        
        exit (EXIT_FAILURE);
    
    }
    */
    if (!CreatePipe (&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 1;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    if (!SetHandleInformation (hStdOutPipeRead, HANDLE_FLAG_INHERIT, 0)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 1;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    si.cb = sizeof (si);
    
    si.hStdError = hStdOutPipeWrite;
    si.hStdOutput = hStdOutPipeWrite;
    si.hStdInput = hStdInPipeRead;
    
    si.dwFlags |= STARTF_USESTDHANDLES;
    
    if (!CreateProcess (getenv ("COMSPEC"), lpCommandLine, 0, 0, 1, 0, 0, 0, &si, &pi)) {
    
        fprintf (stderr, "process_begin: CreateProcess(NULL, %s, ...) failed.\n", input);
        fprintf (stderr, "%s: %s:%lu: pipe: %s\n", program_name, filename, line_no, strerror (GetLastError ()));
        
        if (state->keep_going) {
            return 1;
        }
        
        exit (EXIT_FAILURE);
    
    }
    
    CloseHandle (hStdOutPipeWrite);
    CloseHandle (hStdInPipeRead);
    CloseHandle (hStdInPipeWrite);
    
    /*WaitForSingleObject (pi.hProcess, INFINITE);*/
    lbuf.start = xmalloc (lbuf.size = 256);
    
    {
    
        char *end = lbuf.start + lbuf.size;
        char *p = lbuf.start;
        
        DWORD dwRead;
        
        while (ReadFile (hStdOutPipeRead, p, end - p, &dwRead, 0)) {
        
            size_t offset;
            p += dwRead;
            
            offset = p - lbuf.start;
            
            if (end - p >= 80) {
                continue;
            }
            
            lbuf.size *= 2;
            lbuf.start = xrealloc (lbuf.start, lbuf.size);
            
            p = lbuf.start + offset;
            end = lbuf.start + lbuf.size;
        
        }
    
    }
    
    GetExitCodeProcess (pi.hProcess, &dwExitCode);
    WaitForSingleObject (pi.hProcess, INFINITE);
    
    CloseHandle (hStdOutPipeRead);
    
    CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
    
    if (dwExitCode) {
    
        if (!is_ignoring_errors) {
        
            char *space, *p = input;
            
            if ((space = strchr (p, ' '))) {
                p = xstrndup (p, space - p);
            }
            
            fprintf (stderr, "%s%s: *** [%s:%lu: %s] Error %ld\n", lbuf.start, program_name, filename, line_no, name, dwExitCode);
            
            if (state->keep_going) {
                return dwExitCode;
            }
            
            exit (EXIT_FAILURE);
        
        }
    
    } else {
        printf ("%s", lbuf.start);
    }
    
    return 0;

}
#elif   defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
# include       <sys/wait.h>
# include       <errno.h>
# include       <unistd.h>
static int pipe_command (const char *filename, unsigned long line_no, char *input, int is_ignoring_errors, const char *name) {

    int pipefd[2], pid, status;
    
    struct linebuf lbuf;
    FILE *cmd_output;
    
    if (pipe (pipefd) < 0) {
    
        perror ("pipe");
        exit (EXIT_FAILURE);
    
    }
    
    if ((pid = fork ()) < 0) {
    
        perror ("fork");
        exit (EXIT_FAILURE);
    
    }
    
    input = skip_whitespace (input);
    
    if (pid == 0) {
    
        char **argv;
        int argc = 1;
        
        char *prog = input, *space = 0, *ptr;
        int offset = 0, quote, i;
        
        if ((space = strchr (prog, ' '))) {
        
            ptr = space + 1;
            
            for (i = 0; ptr[i]; i++) {
            
                if (isspace ((int) ptr[i])) {
                    argc++;
                }
            
            }
            
            prog = xstrndup (prog, space - prog);
            argv = xmalloc (sizeof (*argv) * (argc + 2));
            
            for (i = 0; ; i++) {
            
                if (!isspace ((int) ptr[i])) {
                    break;
                }
            
            }
            
            argv[1] = ptr + i;
            argc = 1;
            quote = 0;
            
            while (ptr[i]) {
            
                ptr[i - offset] = ptr[i];
                
                if (quote) {
                
                    if (ptr[i] == '\\' && (ptr[i + 1] == quote || ptr[i + 1] == '\\')) {
                    
                        offset++;
                        i++;
                        
                        ptr[i - offset] = ptr[i];
                    
                    } else if (ptr[i] == quote) {
                    
                        quote = 0;
                        offset++;
                    
                    }
                    
                    i++;
                
                } else if (ptr[i] == '\'' || ptr[i] == '"') {
                
                    if (argv[argc] == ptr + i - offset) {
                        argv[argc]++;
                    }
                    
                    quote = ptr[i];
                    i++;
                
                } else if (isspace ((int) ptr[i])) {
                
                    ptr[i - offset] = '\0';
                    argc++;
                    
                    for (i = i + 1; ; i++) {
                    
                        if (!isspace ((int) ptr[i])) {
                            break;
                        }
                    
                    }
                    
                    argv[argc] = ptr + i - offset;
                
                } else {
                    i++;
                }
            
            }
            
            if (argv[argc] != ptr + i - offset) {
            
                ptr[i - offset] = '\0';
                argc++;
            
            }
        
        } else {
            argv = xmalloc (sizeof (*argv) * (argc + 1));
        }
        argv[0] = prog;
        argv[argc] = 0;
        
        if (strcmp (prog, "ls") == 0 && argc == 1) {
        
            argv = xrealloc (argv, sizeof (*argv) * 3);
            argc = 2;
            
            argv[1] = "-C";
            argv[2] = 0;
        
        }
        
        dup2 (pipefd[1], STDOUT_FILENO);
        
        close (pipefd[0]);
        close (pipefd[1]);
        
        execvp (prog, argv);
        free (argv);
        
        printf ("%s: %s: %s\n", program_name, prog, strerror (errno));
        exit (EXIT_FAILURE);
    
    }
    
    close (pipefd[1]);
    
    if (!(cmd_output = fdopen (pipefd[0], "r"))) {
    
        perror ("fdopen");
        exit (EXIT_FAILURE);
    
    }
    
    lbuf.start = xmalloc (lbuf.size = 256);
    
    {
    
        char *end = lbuf.start + lbuf.size;
        char *p = lbuf.start;
        
        while (fgets (p, end - p, cmd_output)) {
        
            size_t offset;
            p += strlen (p);
            
            offset = p - lbuf.start;
            
            if (end - p >= 80) {
                continue;
            }
            
            lbuf.size *= 2;
            lbuf.start = xrealloc (lbuf.start, lbuf.size);
            
            p = lbuf.start + offset;
            end = lbuf.start + lbuf.size;
        
        }
    
    }
    
    if (waitpid (pid, &status, 0) == -1) {
    
        perror ("waitpid");
        exit (EXIT_FAILURE);
    
    }
    
    /*{
    
        char *p = lbuf.start, ch;
        
        while ((ch = *p)) {
        
            if (ch == '"') {
            
                memmove (p, p + 1, strlen (p + 1));
                *(p + strlen (p) - 1) = '\0';
                
                continue;
            
            }
            
            if (ch == '\n' || ch == '\r') {
                *p = ' ';
            }
            
            p++;
        
        }
        
        if (isspace ((int) *(p - 1))) {
            *(p - 1) = '\0';
        }
    
    }*/
    
    if (status) {
    
        if (!is_ignoring_errors) {
        
            fprintf (stderr, "%s%s: *** [%s:%lu: %s] Error %d\n", lbuf.start, program_name, filename, line_no, name, status);
            
            if (state->keep_going) {
                return status;
            }
            
            exit (EXIT_FAILURE);
        
        }
    
    } else {
        printf ("%s", lbuf.start);
    }
    
    return 0;

}
#endif

static int rule_run_command (const char *filename, unsigned long line_no, const char *name, char *p, char *q) {

    char *new_cmds, *s;
    int status = 0;
    
    int is_ignoring_errors = state->ignore_errors;
    int is_quiet = state->quiet;
    int should_execute = !state->dry_run;
    
    *q = '\0';
    
    new_cmds = xstrdup (p);
    *q = '\n';
    
    new_cmds = variable_expand_line (filename, line_no, new_cmds);
    s = new_cmds;
    
    while (isspace ((int) *s) || *s == '-' || *s == '@' || *s == '+') {
    
        if (*s == '@') {
            is_quiet = 1;
        }
        
        if (*s == '-') {
            is_ignoring_errors = 1;
        }
        
        if (*s == '+') {
            should_execute = 1;
        }
        
        s++;
    
    }
    
    if (!is_quiet) {
        printf ("%s\n", new_cmds);
    }
    
    if (should_execute) {
    
/*#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
        status = pipe_command (filename, line_no, s, is_ignoring_errors, name);
#else
*/
        if ((status = system (s)) && !is_ignoring_errors) {
        
            char *space, *p = s;
            
            if ((space = strchr (p, ' '))) {
                p = xstrndup (p, space - p);
            }
            
            fprintf (stderr, "%s: %s: No such file or directory\n", program_name, p);
            fprintf (stderr, "%s: *** [%s:%lu: %s] Error %d\n", program_name, filename, line_no, name, status);
            
/*            if (!keep_going) {
                exit (EXIT_FAILURE);
            }
  */      
        }
/*#endif*/
    
    }
    
    free (new_cmds);
    return status;

}

static char *parse_command (char *text) {

    char *cwd = get_current_directory ();
    char *ret, *p;
    
    if (strncmp (text, "./", 2) && strncmp (text, "../", 3)) {
        return text;
    }
    
    while (1) {
    
        if (strncmp (text, "./", 2) && strncmp (text, "../", 3)) {
            break;
        }
        
        if (strncmp (text, "./", 2) == 0) {
            text += 2;
        } else if (strncmp (text, "../", 3) == 0) {
        
            text += 3;
            
            if ((p = strrchr (cwd, '/'))) {
                *p = '\0';
            }
        
        }
    
    }
    
    ret = xmalloc (strlen (cwd) + 1 + strlen (text));
    sprintf (ret, "%s/%s", cwd, text);
    
    return ret;

}

static int run_commands (const char *filename, unsigned long line_no, struct commands *cmds, char *name) {

    char *p = parse_command (cmds->text), *q;
    int error;
    
    while (1) {
    
        q = strchr (p, '\n');
        
        if (q != cmds->text) {
        
            while (q && q[-1] == '\\') {
                q = strchr (q + 1, '\n');
            }
        
        }
        
        if (!q) { break; };
        
        if ((error = rule_run_command (filename, line_no, name, p, q))) {
            return error;
        }
        
        p = q + 1;
    
    }
    
    return 0;

}

static int rule_use (struct rule *r, char *name) {

    struct dep *dep;
    
    char *hash_name = 0, *filename = 0, *dot = 0, *n2 = 0;
    int ret;
    
    char *p, *star_name, *lesser_name;
    
    CString str;
    cstr_new (&str);
    
    for (dep = r->deps; dep; dep = dep->next) {
    
        if ((p = strchr (dep->name, '%'))) {
        
            /* Replace the rule's '%' by the filename without extension. */
            n2 = xmalloc (strlen (dep->name) + strlen (name) + 1);
            strncat (n2, dep->name, p - dep->name);
            
            if (!(filename = strrchr (name, '/'))) {
                filename = strrchr (name, '\\');
            }
            
            if (!filename) {
                filename = name;
            } else {
                filename++;
            }
            
            if ((dot = strrchr (filename, '.'))) {
                strncat (n2, filename, dot - filename);
            } else {
                strcat (n2, filename);
            }
            
            strcat (n2, p + 1);
            ret = rule_search_and_build (n2);
            cstr_cat (&str, n2, strlen (n2));
            
            free (n2);
            
            if (ret) {
                return ret;
            }
        
        } else {
        
            cstr_cat (&str, dep->name, strlen (dep->name));
            
            if ((ret = rule_search_and_build (dep->name))) {
                return ret;
            }
        
        }
        
        if (dep->next) {
            cstr_ccat (&str, ' ');
        }
    
    }
    
    cstr_ccat (&str, '\0');
    
    if (!r->cmds) {
    
        cstr_free (&str);
        return 0;
    
    }
    
    doing_inference_rule_commands = 0;
    
    if (r->deps) {
        lesser_name = xstrdup (r->deps->name);
    } else {
        lesser_name = xstrdup ("");
    }
    
    star_name = xstrdup (name);
    
    if ((p = strrchr (star_name, '.'))) {
        *p = '\0';
    }
    
    if (!(filename = strrchr (name, '/'))) {
        filename = strrchr (name, '\\');
    }
    
    if (!filename) {
        filename = name;
    } else {
        filename++;
    }
    
    hash_name = xstrdup (filename);
    
    if ((dot = strrchr (hash_name, '.'))) {
        *dot = '\0';
    }
    
    variable_change ("@", xstrdup (name), VAR_ORIGIN_AUTOMATIC);
    variable_change ("<", lesser_name, VAR_ORIGIN_AUTOMATIC);
    variable_change ("%", hash_name, VAR_ORIGIN_AUTOMATIC);
    variable_change ("*", star_name, VAR_ORIGIN_AUTOMATIC);
    variable_change ("^", xstrdup (str.data), VAR_ORIGIN_AUTOMATIC);
    
    return run_commands (r->filename, r->line_no, r->cmds, name);

}

static int suffix_rule_use (struct suffix_rule *s, char *name) {

    char *lesser_name, *star_name;
    char *p;
    
    if (!s->cmds) { return 0; }
    doing_inference_rule_commands = 1;
    
    lesser_name = xmalloc (strlen (name) + strlen (s->first) + 1);
    memcpy (lesser_name, name, strlen (name) + 1);
    
    if ((p = strrchr (lesser_name, '.'))) {
        *p = '\0';
    }
    
    strcat (lesser_name, s->first);
    star_name = xstrdup (name);
    
    if ((p = strrchr (star_name, '.'))) {
        *p = '\0';
    }
    
    variable_change ("<", lesser_name, VAR_ORIGIN_AUTOMATIC);
    variable_change ("*", star_name, VAR_ORIGIN_AUTOMATIC);
    
    return run_commands (s->filename, s->line_no, s->cmds, name);

}

static int single_suffix_rule_use (struct suffix_rule *s, char *name) {

    char *lesser_name, *star_name;
    
    if (!s->cmds) { return 0; }
    doing_inference_rule_commands = 1;
    
    lesser_name = xmalloc (strlen (name) + strlen (s->first) + 1);
    strcpy (lesser_name, name);
    strcat (lesser_name, s->first);
    
    star_name = xstrdup (name);
    
    variable_change ("<", lesser_name, VAR_ORIGIN_AUTOMATIC);
    variable_change ("*", star_name, VAR_ORIGIN_AUTOMATIC);
    
    return run_commands (s->filename, s->line_no, s->cmds, name);

}

static int file_exists (char *name) {

    FILE *f;
    
    if (!(f = fopen (name, "r"))) {
        return 0;
    }
    
    fclose (f);
    return 1;

}

static char *find_target (char *target) {

    struct variable *vpath_var;
    char *vpath;
    
    if (file_exists (target)) {
        return target;
    }
    
    if (!(vpath_var = variable_find ("VPATH"))) {
        return 0;
    }
    
    vpath = variable_expand_line ("<command-line>", 1, xstrdup (vpath_var->value));
    
    while (vpath && *vpath) {
    
        char *vpath_part, *new_target;
        char saved_ch;
        
        while (*vpath && (isspace ((int) *vpath) || (*vpath == ';'))) {
            vpath++;
        }
        
        vpath_part = vpath;
        
        while (*vpath && !isspace ((int) *vpath) && *vpath != ';') {
            vpath++;
        }
        
        saved_ch = *vpath;
        *vpath = '\0';
        
        new_target = xmalloc (strlen (vpath_part) + 1 + strlen (target) + 1);
        strcpy (new_target, vpath_part);
        
        if (strchr (vpath_part, '\\')) {
            strcat (new_target, "\\");
        } else {
            strcat (new_target, "/");
        }
        
        strcat (new_target, target);
        *vpath = saved_ch;
        
        if (file_exists (new_target)) {
            return new_target;
        }
        
        free (new_target);
        
        if (!(vpath = strchr (vpath, ';'))) {
            break;
        }
        
        vpath++;
    
    }
    
    return 0;

}

int rule_search_and_build (char *name) {

    struct rule *r;
    struct suffix_rule *s;
    
    char *suffix;
    char *new_name;
    
    if ((r = rule_find (name))) {
        return rule_use (r, name);
    }
    
    if ((suffix = strrchr (name, '.'))) {
    
        char *duplicate_name = xstrdup (name);
        variable_change ("@", xstrdup (duplicate_name), VAR_ORIGIN_AUTOMATIC);
        
        for (s = suffix_rules; s; s = s->next) {
        
            if (s->second && strcmp (suffix, s->second) == 0) {
            
                char *prereq_name, *p;
                char *new_name;
                
                prereq_name = xmalloc (strlen (duplicate_name) + strlen (s->first) + 1);
                sprintf (prereq_name, "%s", duplicate_name);
                
                if ((p = strrchr (prereq_name, '.'))) {
                    *p = '\0';
                }
                
                strcat (prereq_name, s->first);
                
                if (!(new_name = find_target (prereq_name))) {
                
                    free (prereq_name);
                    continue;
                
                }
                
                if (prereq_name == new_name) {
                
                    free (prereq_name);
                    break;
                
                }
                
                free (prereq_name);
                
                if (strlen (s->first) < strlen (s->second)) {
                    new_name = xrealloc (new_name, strlen (new_name) + strlen (s->second) - strlen (s->first) + 1);
                }
                
                if ((p = strrchr (new_name, '.'))) {
                    *p = '\0';
                }
                
                strcat (new_name, s->second);
                
                free (duplicate_name);
                duplicate_name = new_name;
                
                break;
            
            }
        
        }
        
        if (s) {
        
            int ret = suffix_rule_use (s, duplicate_name);
            
            free (duplicate_name);
            return ret;
        
        }
        
        free (duplicate_name);
    
    } else {
    
        variable_change ("@", xstrdup (name), VAR_ORIGIN_AUTOMATIC);
        
        for (s = suffix_rules; s; s = s->next) {
        
            if (!s->second) {
            
                char *prereq_name, *p;
                char *new_name;
                
                prereq_name = xmalloc (strlen (name) + strlen (s->first) + 1);
                strcpy (prereq_name, name);
                strcat (prereq_name, s->first);
                
                if (!(new_name = find_target (prereq_name))) {
                
                    free (prereq_name);
                    continue;
                
                }
                
                if (prereq_name != new_name) {
                    free (prereq_name);
                }
                
                p = new_name + strlen (new_name) - strlen (s->first);
                *p = '\0';
                
                single_suffix_rule_use (s, new_name);
                free (new_name);
                
                return 0;
            
            }
        
        }
    
    }
    
    new_name = find_target (name);
    
    if (new_name != name) {
        free (new_name);
    }
    
    if (!new_name) {
    
        fprintf (stderr, "%s: *** No rule to make target '%s'. Stop.\n", program_name, name);
        return 1;
    
    }
    
    return 0;

}

int main (int argc, char **argv) {

    char *cwd = 0;
    int ret;
    
    unsigned long i;
    
    if (argc && *argv) {
    
        char *p;
        program_name = *argv;
        
        if ((p = strrchr (program_name, '/')) || (p = strrchr (program_name, '\\'))) {
            program_name = (p + 1);
        }
    
    }
    
    variables_init ();
    rules_init ();
    
    default_goal_var = variable_add (xstrdup (".DEFAULT_GOAL"), xstrdup (""), VAR_ORIGIN_FILE);
    
    variable_add (xstrdup ("OS"), xstrdup (os_name), VAR_ORIGIN_FILE);
    variable_add (xstrdup ("MAKE"), xstrdup (argv[0]), VAR_ORIGIN_FILE);
    
#if     defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
#else
    variable_add (xstrdup ("SHELL"), xstrdup ("sh.exe"), VAR_ORIGIN_FILE);
#endif
    
    state = xmalloc (sizeof (*state));
    parse_args (argv, argc);
    
    if (state->nb_directories > 0) {
    
        char *arg;
        unsigned long i;
        
        size_t len = 0;
        
        cwd = xstrdup (get_current_directory ());
        state->path = xmalloc (strlen (cwd) + 2);

#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
        len = sprintf (state->path, "%s/", cwd);
#else
        len = sprintf (state->path, "%s", cwd);
#endif
        
        for (i = 0; i < state->nb_directories; i++) {
        
            arg = state->directories[i];
            
#if     defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
            if (arg[0] != '/') {
#else
            if (strlen (arg) < 2 || !(isalpha ((int) arg[0]) && arg[1] == ':')) {
#endif      
                state->path = xrealloc (state->path, len + strlen (arg) + 2);
                
#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
                len += sprintf (state->path + len, "%s/", arg);
#else
                len += sprintf (state->path + len, "%s\\", arg);
#endif
                
                state->path[len] = '\0';
            
            } else {
            
                unsigned long len = strlen (arg);
                free (state->path);
                
                state->path = xmalloc (len + 2);
                
#if     defined (_WIN32) || defined (__WIN32__) || defined (unix) || defined (__unix) || defined (__unix__) || defined (__APPLE__)
                len = sprintf (state->path, "%s/", arg);
#else
                len = sprintf (state->path, "%s\\", arg);
#endif
            
            }
        
        }
        
        if (!directory_exists (state->path)) {
        
            fprintf (stderr, "%s: *** '%s': No such directory. Stop.\n", program_name, state->path);
            return EXIT_FAILURE;
        
        }
        
        if (!state->no_print) {
            fprintf (stderr, "%s: Entering directory '%s'\n", program_name, state->path);
        }
        
        if (!change_directory (state->path)) {
        
            fprintf (stderr, "%s: *** Failed to enter '%s'. Stop.\n", program_name, state->path);
            return EXIT_FAILURE;
        
        }
    
    }
    
    {
    
        char *path, *cwd = get_current_directory ();
        
        path = xmalloc (strlen ("CURDIR") + 4 + strlen (cwd) + 1);
        sprintf (path, "CURDIR ?= %s", cwd);
        
        parse_var_line ("<command-line>", 1, path, VAR_ORIGIN_FILE);
        free (path);
        
        path = xmalloc (strlen (".CURDIR") + 4 + strlen (cwd) + 1);
        sprintf (path, ".CURDIR ?= %s", cwd);
        
        parse_var_line ("<command-line>", 1, path, VAR_ORIGIN_FILE);
        free (path);
        
        /*variable_add (xstrdup ("CURDIR"), xstrdup (get_current_directory ()), VAR_ORIGIN_FILE);*/
    
    }
    
    if (state->nb_makefiles > 0) {
    
        char *path;
        unsigned long i;
        
        for (i = 0; i < state->nb_makefiles; i++) {
        
            path = state->makefiles[i];
            
            if ((ret = read_makefile (path))) {
            
                fprintf (stderr, "%s: *** Failed to read '%s'. Stop.\n", program_name, path);
                goto out;
            
            }
        
        }
    
    }
    
    if (state->nb_goals == 0) {
    
        if (default_goal_var->value[0] == '\0') {
        
            fprintf (stderr, "%s: *** No targets. Stop.\n", program_name);
            
            ret = EXIT_FAILURE;
            goto out;
        
        }
        
        dynarray_add (&state->goals, &state->nb_goals, xstrdup (default_goal_var->value));
    
    }
    
    for (i = 0; i < state->nb_goals; i++) {
    
        if ((ret = rule_search_and_build (state->goals[i]))) {
        
            if (!state->keep_going) {
                goto out;
            }
        
        }
    
    }
    
    ret = EXIT_SUCCESS;
    
out:
    
    if (state->nb_directories > 0 && !state->no_print) {
    
        fprintf (stderr, "%s: Leaving directory '%s'\n", program_name, state->path);
        if (cwd) { change_directory (cwd); }
    
    }
    
    return ret;

}
