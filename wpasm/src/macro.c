/******************************************************************************
 * @file            macro.c
 *****************************************************************************/
#include    <ctype.h>
#include    <stdlib.h>
#include    <string.h>

#include    "cstr.h"
#include    "hashtab.h"
#include    "lex.h"
#include    "lib.h"
#include    "macro.h"
#include    "report.h"
#include    "vector.h"

static struct hashtab hashtab_macros = { 0 };

struct hashtab_name *find_macro (char *sname) {

    struct hashtab_name *key;
    
    if ((key = hashtab_get_key (&hashtab_macros, sname))) {
        return key;
    }
    
    return 0;

}

struct macro *get_macro (struct hashtab_name *key) {
    return hashtab_get (&hashtab_macros, key);
}

void add_macro (char *start, char **pp, int report_line) {

    char *sname, *caret = *pp, *arg;
    unsigned int len;
    
    struct hashtab_name *key;
    struct macro *m;
    
    const char *filename;
    unsigned long line_number;
    get_filename_and_line_number (&filename, &line_number);
    
    if (is_end_of_line[(int) **pp]) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_ERROR, start, caret - 1, "no macro name give in %%define directive");
        } else {
            report_at (filename, line_number, REPORT_ERROR, "no macro name give in %%define directive");
        }
        
        return;
    
    }
    
    if (!(sname = symname (pp))) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_ERROR, start, caret, "macro names must be identifiers");
        } else {
            report_at (filename, line_number, REPORT_ERROR, "macro names must be identifiers");
        }
        
        return;
    
    }
    
    if (strcmp (sname, "defined") == 0) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_ERROR, start, caret, "\"%s\" cannout be used as a macro name", sname);
        } else {
            report_at (filename, line_number, REPORT_ERROR, "\"%s\" cannout be used as a macro name", sname);
        }
        
        return;
    
    }
    
    if (**pp != '(' && !isspace ((int) **pp)) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "whitespace is required after macro name");
        } else {
            report_at (filename, line_number, REPORT_ERROR, "whitespace is required after macro name");
        }
    
        return;
    
    }
    
    if ((key = find_macro (sname))) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_WARNING, start, caret, "\"%s\" redefined", sname);
        } else {
            report_at (filename, line_number, REPORT_WARNING, "\"%s\" redefined", sname);
        }
        
        if ((m = hashtab_get (&hashtab_macros, key))) {
        
            while ((arg = vec_pop (&m->args))) {
                free (arg);
            }
            
            free (m);
        
        }
        
        hashtab_remove (&hashtab_macros, key);
    
    } else {
    
        if (!(key = hashtab_alloc_name (sname))) {
        
            free (sname);
            return;
        
        }
    
    }
    
    m = xmalloc (sizeof (*m));
    m->nargs = -1;
    
    m->type = MACRO_USER;
    m->name = sname;
    
    if (**pp == '(') {
    
        m->nargs = 0;
        (*pp)++;
        
        while (!is_end_of_line[(int) **pp]) {
        
            *pp = skip_whitespace (*pp);
            
            if (**pp == ')') {
                break;
            }
            
            if (m->is_variadic) {
            
                if (report_line) {
                    report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "expected ')' after '...'");
                } else {
                    report_at (filename, line_number, REPORT_ERROR, "expected ')' after '...'");
                }
                
                while ((arg = vec_pop (&m->args))) {
                    free (arg);
                }
                
                free (m);
                return;
            
            }
            
            arg = *pp;
            
            while (!is_end_of_line[(int) *arg] && !isspace ((int) *arg)) {
            
                if (*arg == ',' || *arg == ')') {
                    break;
                }
                
                arg++;
            
            }
            
            if (arg - *pp == 3) {
            
                if (memcmp (*pp, "...", 3) == 0) {
                
                    m->is_variadic = 1;
                    
                    *pp = arg;
                    continue;
                
                }
            
            }
            
            if ((sname = symname (pp))) {
            
                vec_push (&m->args, sname);
                m->nargs++;
                
                *pp = skip_whitespace (*pp);
                
                if (**pp != ',' && **pp != ')') {
                
                    if (report_line) {
                        report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "expected ',' or, ')' after parameter");
                    } else {
                        report_at (filename, line_number, REPORT_ERROR, "expected ',' or, ')' after parameter");
                    }
                    
                    goto err;
                
                }
                
                if (**pp == ')') {
                    break;
                }
                
                (*pp)++;
                continue;
            
            }
            
            if (report_line) {
                report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "expected parameter name");
            } else {
                report_at (filename, line_number, REPORT_ERROR, "expected parameter name");
            }
            
            goto err;
        
        }
        
        if (**pp != ')') {
        
            if (report_line) {
                report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "expected ')' before end of line");
            } else {
                report_at (filename, line_number, REPORT_ERROR, "expected ')' before end of line");
            }
            
            goto err;
        
        }
        
        (*pp)++;
    
    }
    
    *pp = skip_whitespace (*pp);
    
    m->value = xstrdup (*pp);
    len = strlen (m->value);
    
    if (is_end_of_line[(int) m->value[len - 1]]) {
        m->value[len - 1] = '\0';
    }
    
    hashtab_put (&hashtab_macros, key, m);
    
    if (!m->is_variadic) {
    
        char *haystack = m->value, *needle = "__VA_ARGS__";
        char *p;
        
        while ((p = strstr (haystack, needle))) {
        
            haystack = (p + strlen (needle));
            
            if (report_line) {
                report_line_at (filename, line_number, REPORT_WARNING, m->value, p, "%s can only appear in the expansion of a variadic macro", needle);
            } else {
                report_at (filename, line_number, REPORT_WARNING, "%s can only appear in the expansion of a variadic macro", needle);
            }
        
        }
    
    }
    
    return;
    
err:
    
    while ((arg = vec_pop (&m->args))) {
        free (arg);
    }
    
    free (m);
    return;

}

void remove_macro (char *start, char **pp, int report_line) {

    char *sname, *caret = *pp;
    
    struct hashtab_name *key;
    struct macro *mp;
    
    const char *filename;
    unsigned long line_number;
    get_filename_and_line_number (&filename, &line_number);
    
    if (!(sname = symname (pp))) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_ERROR, start, caret, "macro names must be identifiers");
        } else {
            report_at (filename, line_number, REPORT_ERROR, "macro names must be identifiers");
        }
        
        return;
    
    }
    
    if ((key = find_macro (sname))) {
    
        if ((mp = hashtab_get (&hashtab_macros, key))) {
            free (mp);
        }
        
        hashtab_remove (&hashtab_macros, key);
    
    }
    
    *pp = skip_whitespace (*pp);
    
    if (!is_end_of_line[(int) **pp]) {
    
        if (report_line) {
            report_line_at (filename, line_number, REPORT_WARNING, start, *pp, "extra tokens at end of %%undef directive");
        } else {
            report_at (filename, line_number, REPORT_WARNING, "extra tokens at end of %%undef directive");
        }
    
    }

}

static struct vector *get_macro_args (char *start, char *macro_name, char **pp) {

    static struct vector args_list = { 0 };
    char *arg, saved_ch, ch;
    
    const char *filename;
    unsigned long line_number;
    get_filename_and_line_number (&filename, &line_number);
    
    memset (&args_list, 0, sizeof (args_list));
    
    if (**pp == '(') {
    
        (*pp)++;
        
        for (;;) {
        
            *pp = skip_whitespace (*pp);
            
            if (is_end_of_line[(int) **pp] || **pp == ')') {
                break;
            }
            
            arg = *pp;
            ch = *arg;
            
            if (ch == '"' || ch == '\'') {
            
                (*pp)++;
                
                while (!is_end_of_line[(int) **pp]) {
                
                    if (**pp == '\\') {
                    
                        (*pp)++;
                        
                        if (**pp == ch) {
                            (*pp)++;
                        }
                        
                        continue;
                    
                    }
                    
                    if (**pp == ch) { break; }
                    (*pp)++;
                
                }
                
                if (**pp == ch) {
                    (*pp)++;
                } else {
                    report_line_at (filename, line_number, REPORT_WARNING, start, arg, "missing terminating %c character", ch);
                }
            
            } else {
            
                while (!is_end_of_line[(int) **pp] && **pp != ' ' && **pp != '\t') {
                
                    if (**pp == ',' || **pp == ')') {
                        break;
                    }
                    
                    (*pp)++;
                
                }
            
            }
            
            saved_ch = **pp;
            **pp = '\0';
            
            vec_push (&args_list, xstrdup (arg));
            
            **pp = saved_ch;
            
            if (*(*pp = skip_whitespace (*pp)) == ',') {
                (*pp)++;
            }
        
        }
        
        if (**pp != ')') {
        
            report_line_at (filename, line_number, REPORT_ERROR, start, *pp, "unterminated argument list invoking macro \"%s\"", macro_name);
            return 0;
        
        }
        
        (*pp)++;
    
    }
    
    return &args_list;

}

static char *process_value (struct macro *m, struct vector *args_list) {

    char *line = m->value, *arg;
    int i;
    
    struct cstring str;
    cstr_new (&str);
    
    while (!is_end_of_line[(int) *line]) {
    again:
    
        if (line[0] == '#' && line[1] == '#') {
        
            if (str.size && ((char *) str.data)[str.size - 1] == ' ') { str.size--; }
            
            line += 2;
            continue;
        
        }
        
        if (is_name_beginner ((int) *line)) {
        
            arg = symname (&line);
            
            if (strcmp (arg, "__VA_ARGS__") == 0) {
            
                for (i = m->nargs; i < args_list->length; i++) {
                
                    arg = args_list->data[i];
                    cstr_cat (&str, arg, strlen (arg));
                    
                    if (i < args_list->length - 1) {
                    
                        cstr_ccat (&str, ',');
                        cstr_ccat (&str, ' ');
                    
                    }
                
                }
                
                continue;
            
            }
            
            for (i = 0; i < m->nargs; i++) {
            
                if (strcmp (m->args.data[i], arg) == 0) {
                
                    arg = args_list->data[i];
                    cstr_cat (&str, arg, strlen (arg));
                    
                    goto again;
                
                }
            
            }
            
            cstr_cat (&str, arg, strlen (arg));
        
        } else {
            cstr_ccat (&str, *line++);
        }
    
    }
    
    cstr_ccat (&str, '\0');
    return xstrdup (str.data);

}

char *process_macro (char *start, char **pp, struct macro *m) {

    struct vector *args_list;
    char *caret;
    
    const char *filename;
    unsigned long line_number;
    get_filename_and_line_number (&filename, &line_number);
    
    if (m->nargs > 0 || m->is_variadic) {
    
        args_list = 0;
        caret = *pp;
        
        if ((args_list = get_macro_args (start, m->name, pp))) {
        
            if (args_list->length < m->nargs) {
            
                char *tmp = (m->nargs == 1 ? " argument" : " arguments");
                char *tmp2 = (args_list->length == 1 ? "only " : "");
                
                report_line_at (filename, line_number, REPORT_ERROR, start, caret, "macro \"%s\" requires %d%s, but %s%d given", m->name, m->nargs, tmp, tmp2, args_list->length);
            
            } else if (args_list->length > m->nargs) {
            
                if (!m->is_variadic) {
                
                    char *tmp = (args_list->length == 1 ? " argument" : " arguments");
                    char *tmp2 = (m->nargs == 1 ? "just " : "");
                    
                    report_line_at (filename, line_number, REPORT_ERROR, start, caret, "macro \"%s\" passed %d%s, but takes %s%d", m->name, args_list->length, tmp, tmp2, m->nargs);
                
                }
            
            }
        
        }
        
        return process_value (m, args_list);
    
    }
    
    return m->value;

}
