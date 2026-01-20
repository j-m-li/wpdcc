/******************************************************************************
 * @file            process.c
 *****************************************************************************/
#include    <ctype.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <time.h>

#include    "as.h"
#include    "cstr.h"
#include    "expr.h"
#include    "frag.h"
#include    "hashtab.h"
#include    "kwd.h"
#include    "lex.h"
#include    "lib.h"
#include    "listing.h"
#include    "ll.h"
#include    "macro.h"
#include    "process.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"
#include    "vector.h"

static char *preprocess_line (char *src, int in_macro) {

    struct cstring cstr;
    char *line;
    
    char *caret = src, *start;
    char *sname, ch;
    
    struct hashtab_name *key;
    struct macro *m;
    
    cstr_new (&cstr);
    
    while (!is_end_of_line[(int) *caret]) {
    
        start = caret;
        
        if (*caret == ' ' || *caret == '\t') {
        
            while (*caret == ' ' || *caret == '\t') {
            
                cstr_ccat (&cstr, ' ');
                caret++;
            
            }
            
            continue;
        
        }
        
        if (*caret == '"' || *caret == '\'') {
        
            ch = *caret++;
            
            while (!is_end_of_line[(int) *caret]) {
            
                if (*caret == '\\') {
                
                    caret++;
                    
                    if (!is_end_of_line[(int) *caret]) {
                        caret++;
                    }
                    
                    continue;
                
                }
                
                if (*caret == ch) { break; }
                caret++;
            
            }
            
            if (*caret != ch) {
            
                char *temp = xmalloc ((caret - start) + 2);
                sprintf (temp, "%.*s%c", (int) (caret - start), start, ch);
                
                report_line_at (get_filename (), get_line_number (), REPORT_WARNING, src, start, "missing terminating %c character", ch);
                cstr_cat (&cstr, temp, strlen (temp));
                
                continue;
            
            }
            
            caret++;
            
            cstr_cat (&cstr, start, caret - start);
            continue;
        
        }
        
        if (is_name_beginner ((int) *caret)) {
        
            sname = symname (&caret);
            
            if ((key = find_macro (sname))) {
            
                if ((m = get_macro (key))) {
                
                    char *pm;
                    int spaces = 0;
                    
                    if (*caret == ' ' || *caret == '\t') {
                    
                        cstr_ccat (&cstr, ' ');
                        
                        while (*caret == ' ' || *caret == '\t') {
                        
                            spaces++;
                            caret++;
                        
                        }
                    
                    }
                    
                    if ((pm = process_macro (start, &caret, m))) {
                    
                        char *temp = preprocess_line (pm, 1);
                        cstr_cat (&cstr, temp, strlen (temp));
                        
                        if (!is_end_of_line[(int) *pm]) {
                        
                            while (spaces--) {
                                cstr_ccat (&cstr, ' ');
                            }
                        
                        }
                    
                    }
                
                }
                
                continue;
            
            }
            
            if (xstrcasecmp (sname, "DGROUP") == 0 && *caret == ':') {
            
                caret++;
                continue;
            
            }
            
            cstr_cat (&cstr, start, caret - start);
            continue;
        
        }
        
        if (*caret == '@') {
        
            char *arg, *temp = (caret + 1);
            
            if ((arg = symname (&temp))) {
            
                if (xstrcasecmp (arg, "DataSize") == 0) {
                
                    caret = temp;
                    free (arg);
                    
                    cstr_ccat (&cstr, state->data_size | 0x30);
                    continue;
                
                }
                
                if (xstrcasecmp (arg, "Model") == 0) {
                
                    caret = temp;
                    free (arg);
                    
                    cstr_ccat (&cstr, (state->model > 0 ? state->model | 0x30 : '1'));
                    continue;
                
                }
                
                free (arg);
            
            }
        
        }
        
        if (isdigit ((int) *caret)) {
        
            if (caret[0] == '0' && tolower ((int) caret[1]) == 'x') {
                caret += 2;
            }
            
            while (isdigit ((int) *caret)) {
                caret++;
            }
            
            cstr_cat (&cstr, start, caret - start);
            continue;
        
        }
        
        if (ispunct ((int) *caret)) {
        
            cstr_ccat (&cstr, *caret);
            
            caret++;
            continue;
        
        }
        
        report_line_at (get_filename (), get_line_number (), REPORT_INTERNAL_ERROR, src, caret, "Do we see this???");
        caret++;
    
    }
    
    if (!in_macro && state->ofp) {
        cstr_ccat (&cstr, '\n');
    }
    
    cstr_ccat (&cstr, '\0');
    
    line = xstrdup (cstr.data);;
    cstr_free (&cstr);
    
    return line;

}


struct pp_pseudo_op_entry {

    const char *name;
    void (*handler) (char *start, char **pp);

};

static struct vector vec_include_paths = { 0 };

struct cond {

    char *directive;
    int ignore_line;
    
    char *filename;
    unsigned long line_number;
    
    int has_else;

};

static struct vector vec_ifstack = { 0 };
static int ignore_line = 0, iflevel = 0;

static void handler_if (char *start, char **pp) {

    struct cond *cond;
    struct expr expr_s;
    
    char *tokenized_line, *tokenized_start;
    unsigned long pos = *pp - start;
    
    tokenized_start = (tokenized_line = preprocess_line (start, 0));
    tokenized_line += pos;
    
    expression_evaluate_and_read_into (tokenized_start, &tokenized_line, &expr_s);
    free (tokenized_start);
    
    if (expr_s.type != EXPR_TYPE_CONSTANT) {
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "non-constant expression in \"%if\" statement");
    }
    
    if (!ignore_line) {
    
        cond = xmalloc (sizeof (*cond));
        
        cond->ignore_line = ignore_line;
        cond->directive = xstrdup ("if");
        
        cond->filename = xstrdup (get_filename ());
        cond->line_number = get_line_number ();
        
        vec_push (&vec_ifstack, cond);
        ignore_line = (expr_s.add_number == 0);
    
    } else {
        iflevel++;
    }

}

static void handler_ifdef (char *start, char **pp) {

    struct cond *cond;
    char *sname, *caret;
    
    if (!ignore_line) {
    
        cond = xmalloc (sizeof (*cond));
        
        cond->ignore_line = ignore_line;
        cond->directive = xstrdup ("ifdef");
        
        cond->filename = xstrdup (get_filename ());
        cond->line_number = get_line_number ();
        
        vec_push (&vec_ifstack, cond);
        *pp = skip_whitespace (*pp);
        
        if (is_name_beginner ((int) **pp)) {
        
            caret = (*pp);
            
            while (is_name_part ((int) **pp)) {
                (*pp)++;
            }
            
            sname = xstrndup (caret, *pp - caret);
            ignore_line = (find_macro (sname) == NULL);
            
            free (sname);
        
        }
        
        *pp = skip_whitespace (*pp);
        
        if (!is_end_of_line[(int) **pp]) {
            report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, *pp, "extra tokens at end of %%ifdef directive");
        }
    
    } else {
        iflevel++;
    }

}

static void handler_ifndef (char *start, char **pp) {

    struct cond *cond;
    char *sname, *caret;
    
    if (!ignore_line) {
    
        cond = xmalloc (sizeof (*cond));
        
        cond->ignore_line = ignore_line;
        cond->directive = xstrdup ("ifndef");
        
        cond->filename = xstrdup (get_filename ());
        cond->line_number = get_line_number ();
        
        vec_push (&vec_ifstack, cond);
        *pp = skip_whitespace (*pp);
        
        if (is_name_beginner ((int) **pp)) {
        
            caret = (*pp);
            
            while (is_name_part ((int) **pp)) {
                (*pp)++;
            }
            
            sname = xstrndup (caret, *pp - caret);
            ignore_line = (find_macro (sname) != NULL);
            
            free (sname);
        
        }
        
        *pp = skip_whitespace (*pp);
        
        if (!is_end_of_line[(int) **pp]) {
            report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, *pp, "extra tokens at end of %%ifndef directive");
        }
    
    } else {
        iflevel++;
    }

}

static void handler_elif (char *start, char **pp) {

    struct cond *cond;
    struct expr expr_s;
    
    char *tokenized_line, *tokenized_start;
    unsigned long pos = *pp - start;
    
    tokenized_start = (tokenized_line = preprocess_line (start, 0));
    tokenized_line += pos;
    
    expression_evaluate_and_read_into (tokenized_start, &tokenized_line, &expr_s);
    free (tokenized_start);
    
    if (!iflevel) {
    
        if (vec_ifstack.length == 0) {
        
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%%elif without %%if");
            return;
        
        } else {
        
            cond = vec_ifstack.data[vec_ifstack.length - 1];
            
            if (cond->has_else > 0) {
            
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%%elif after %%else");
                return;
            
            }
        
        }
        
        ignore_line = (ignore_line && (expr_s.add_number == 0));
    
    }

}

static void handler_else (char *start, char **pp) {

    struct cond *cond;
    
    if (!iflevel) {
    
        if (vec_ifstack.length == 0) {
        
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%%else without %%if");
            return;
        
        } else {
        
            cond = vec_ifstack.data[vec_ifstack.length - 1];
            
            if (cond->has_else > 0) {
            
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%%else after %%else");
                return;
            
            }
            
            cond->has_else++;
        
        }
        
        *pp = skip_whitespace (*pp);
        
        if (!is_end_of_line[(int) **pp]) {
            report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, *pp, "extra tokens at end of %%else directive");
        }
        
        ignore_line = !ignore_line;
    
    }

}

static void handler_endif (char *start, char **pp) {

    struct cond *cond;
    
    if (!iflevel) {
    
        if ((cond = vec_pop (&vec_ifstack))) {
        
            ignore_line = cond->ignore_line;
            
            free (cond->filename);
            free (cond->directive);
            
            free (cond);
        
        } else {
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%%endif without %%if");
        }
        
        *pp = skip_whitespace (*pp);
        
        if (!is_end_of_line[(int) **pp]) {
            report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, *pp, "extra tokens at end of %%endif directive");
        }
    
    } else {
        iflevel--;
    }

}

static struct pp_pseudo_op_entry cond_pseudo_op_table[] = {

    {   "if",           &handler_if,            },
    {   "ifdef",        &handler_ifdef          },
    {   "ifndef",       &handler_ifndef         },
    {   "elif",         &handler_elif           },
    {   "else",         &handler_else           },
    {   "endif",        &handler_endif          },
    
    {   0,              0                       }

};

static struct hashtab hashtab_cond_pseudo_ops = { 0 };
static int includes = 0;

static void install_pp_pseudo_op_table (struct pp_pseudo_op_entry *table, struct hashtab *hashatb_table) {

    struct pp_pseudo_op_entry *entry;
    struct hashtab_name *key;
    
    for (entry = table; entry->name; entry++) {
    
        if (hashtab_get_key (hashatb_table, entry->name)) {
        
            report_at (program_name, 0, REPORT_ERROR, "duplicate entry '%s'", entry->name);
            continue;
        
        }
        
        if (!(key = hashtab_alloc_name (entry->name))) {
        
            report_at (program_name, 0, REPORT_ERROR, "failed to allocate memory for '%s'", entry->name);
            continue;
        
        }
        
        hashtab_put (hashatb_table, key, entry);
    
    }

}

static struct pp_pseudo_op_entry *find_cond_directive (char *name) {

    struct hashtab_name *key;
    struct pp_pseudo_op_entry *entry;
    
    if ((key = hashtab_get_key (&hashtab_cond_pseudo_ops, name))) {
    
        if ((entry = hashtab_get (&hashtab_cond_pseudo_ops, key))) {
            return entry;
        }
    
    }
    
    return 0;

}


static struct hashtab hashtab_pseudo_ops = { 0 };

static void handler_define (char *start, char **pp) {
    add_macro (start, pp, 1);
}

static void handler_error (char *start, char **pp) {

    unsigned long len = strlen (*pp);
    char *temp, *type = "error";
    
    if ((*pp)[len - 1] == '\n') {
        (*pp)[len - 1] = '\0';
    }
    
    temp = xmalloc (1 + strlen (type) + 1 + strlen (*pp) + 1);
    sprintf (temp, "%%%s %s", type, *pp);
    
    report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, skip_whitespace (start + 1), "%s", temp);
    free (temp);

}

static void handler_include (char *start, char **pp) {

    const char *orig_fn = get_filename ();
    unsigned long orig_ln = get_line_number ();
    
    char *caret, *sname, ch;
    int i;
    
    char *inc_path, *tmp;
    FILE *fp;
    
    if (**pp != '"' && **pp != '<') {
    
        report_line_at (orig_fn, orig_ln, REPORT_ERROR, start, *pp, "%%include expects \"FILENAME\" or <FILENAME>");
        return;
    
    }
    
    ch = (**pp == '"' ? '"' : '>');
    caret = (*pp)++;
    
    while (!is_end_of_line[(int) **pp]) {
    
        if (**pp == ch) { break; }
        (*pp)++;
    
    }
    
    if (**pp != ch) {
    
        report_line_at (orig_fn, orig_ln, REPORT_ERROR, start, caret, "%%include expects \"FILENAME\" or <FILENAME>");
        return;
    
    } else {
        (*pp)++;
    }
    
    sname = xstrndup (caret + 1, *pp - caret - 2);
    
    if (ch == '"' && (fp = fopen (sname, "r"))) {
    
        fclose (fp);
        
        includes++;
        process_file (sname);
        
        includes--;
        goto end;
    
    }
    
    for (i = vec_include_paths.length - 1; i >= 0; i--) {
    
        inc_path = vec_include_paths.data[i];
        
        tmp = xmalloc (strlen (inc_path) + strlen (sname) + 1);
        sprintf (tmp, "%s%s", inc_path, sname);
        
        if ((fp = fopen (tmp, "r"))) {
        
            fclose (fp);
            includes++;
            
            process_file (tmp);
            includes--;
            
            free (tmp);
            goto end;
        
        }
        
        free (tmp);
    
    }
    
    report_line_at (orig_fn, orig_ln, REPORT_ERROR, start, caret, "failed to open '%s' for reading", sname);
    
end:
    
    set_filename_and_line_number (orig_fn, orig_ln);
    free (sname);

}

static void handler_undef (char *start, char **pp) {
    remove_macro (start, pp, 1);
}

static void handler_warning (char *start, char **pp) {

    unsigned long len = strlen (*pp);
    char *temp, *type = "warning";
    
    if ((*pp)[len - 1] == '\n') {
        (*pp)[len - 1] = '\0';
    }
    
    temp = xmalloc (1 + strlen (type) + 1 + strlen (*pp) + 1);
    sprintf (temp, "%%%s %s", type, *pp);
    
    report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, skip_whitespace (start + 1), "%s", temp);
    free (temp);

}

static struct pp_pseudo_op_entry pseudo_op_table[] = {

    {   "define",       &handler_define         },
    {   "error",        &handler_error          },
    {   "include",      &handler_include        },
    {   "undef",        &handler_undef          },
    {   "warning",      &handler_warning        },
    
    {   0,              0                       }

};

struct pp_pseudo_op_entry *find_directive (char *name) {

    struct hashtab_name *key;
    struct pp_pseudo_op_entry *entry;
    
    if ((key = hashtab_get_key (&hashtab_pseudo_ops, name))) {
    
        if ((entry = hashtab_get (&hashtab_pseudo_ops, key))) {
            return entry;
        }
    
    }
    
    return 0;

}


int preprocess_init (void) {

    struct list *item;
    char *opt, *nopt, *p;
    
    set_filename_and_line_number (xstrdup ("<command-line>"), 1);
    
    if (state->pplist) {
    
        item = state->pplist;
        
        do {
        
            item = item->next;
            
            if (!(opt = item->data)) {
                continue;
            }
            
            if (opt[0] != '-') {
            
                report_at (program_name, 0, REPORT_ERROR, "unrecognised option '%s'", opt);
                continue;
            
            }
            
            switch (opt[1]) {
            
                case 'D':
                
                    nopt = opt + 2;
                    
                    if ((p = strrchr (nopt, '='))) {
                        *p++ = ' ';
                    }
                    
                    add_macro (nopt, &nopt, 0);
                    break;
                
                case 'I':
                
                    vec_push (&vec_include_paths, opt + 2);
                    break;
                
                case 'U':
                
                    nopt = opt + 2;
                    
                    remove_macro (nopt, &nopt, 0);
                    break;
                
                default:
                
                    report_at (program_name, 0, REPORT_ERROR, "unrecognised option '%s'", opt);
                    break;
            
            }
        
        } while (item != state->pplist);
    
    }
    
    install_pp_pseudo_op_table (cond_pseudo_op_table, &hashtab_cond_pseudo_ops);
    install_pp_pseudo_op_table (pseudo_op_table, &hashtab_pseudo_ops);
    
    return get_error_count () > 0;

}


struct section *machine_dependent_simplified_expression_read_into (char *start, char **pp, struct expr *expr);

static struct section *get_known_section_expression (char *start, char **pp, struct expr *expr) {

    struct section *section = machine_dependent_simplified_expression_read_into (start, pp, expr);
    
    if (expr->type == EXPR_TYPE_INVALID || expr->type == EXPR_TYPE_ABSENT) {
    
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "expected address expression");
        
        expr->type = EXPR_TYPE_CONSTANT;
        expr->add_number = 0;
        
        section = absolute_section;
    
    }
    
    if (section == undefined_section) {
    
        if (expr->add_symbol && symbol_get_section (expr->add_symbol) != expr_section) {
            report_at (get_filename (), get_line_number (), REPORT_WARNING, "symbol \"%s\" undefined; zero assumed", symbol_get_name (expr->add_symbol));
        } else {
            report_at (get_filename (), get_line_number (), REPORT_WARNING, "some symbol undefined; zero assumed");
        }
        
        expr->type = EXPR_TYPE_CONSTANT;
        expr->add_number = 0;
        
        section = absolute_section;
    
    }
    
    return section;

}

static void do_org (struct section *section, struct expr *expr, unsigned long fill_value) {

    struct symbol *symbol;
    
    unsigned char *p_in_frag;
    unsigned long offset;
    
    if (section != current_section && section != absolute_section && section != expr_section) {
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "invalid section \"%s\"", section_get_name (section));
    }
    
    symbol = expr->add_symbol;
    offset = expr->add_number;
    
    if (fill_value && current_section == bss_section) {
        report_at (get_filename (), get_line_number (), REPORT_WARNING, "ignoring fill value in section \"%s\"", section_get_name (current_section));
    }
    
    if (expr->type != EXPR_TYPE_CONSTANT && expr->type != EXPR_TYPE_SYMBOL) {
    
        symbol = make_expr_symbol (expr);
        offset = 0;
    
    }
    
    *(p_in_frag = frag_alloc_space (1)) = (unsigned char) fill_value;
    frag_set_as_variant (RELAX_TYPE_ORG, 0, symbol, offset, 0);

}

static void internal_set (char *start, char **pp, struct symbol *symbol) {

    struct expr expr;
    machine_dependent_simplified_expression_read_into (start, pp, &expr);
    
    if (expr.type == EXPR_TYPE_INVALID) {
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "invalid expression");
    } else if (expr.type == EXPR_TYPE_ABSENT) {
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "missing expression");
    }
    
    if (symbol_is_section_symbol (symbol)) {
    
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "attempt to set value of section symbol");
        return;
    
    }
    
    switch (expr.type) {
    
        case EXPR_TYPE_INVALID:
        case EXPR_TYPE_ABSENT:
        
            expr.add_number = 0;
            /* fall through */
        
        case EXPR_TYPE_CONSTANT:
        
            symbol_set_frag (symbol, &zero_address_frag);
            symbol_set_section (symbol, absolute_section);
            symbol_set_value (symbol, expr.add_number);
            
            break;
        
        default:
        
            symbol_set_frag (symbol, &zero_address_frag);
            symbol_set_section (symbol, expr_section);
            symbol_set_value_expression (symbol, &expr);
            
            break;
    
    }

}

static void assign_symbol (char *start, char **pp, char *name) {

    struct symbol *symbol;
    
    if (name[0] == '.' && name[1] == '\0') {
    
        struct section *section;
        struct expr expr;
        
        section = get_known_section_expression (start, pp, &expr);
        do_org (section, &expr, 0);
        
        return;
    
    }
    
    symbol = symbol_find_or_make (name, SYMBOL_SCOPE_LOCAL);
    internal_set (start, pp, symbol);

}

static void handle_org (char *start, char **pp) {

    struct expr expr;
    
    struct section *section = get_known_section_expression (start, pp, &expr);
    unsigned long fill_value = 0;
    
    if (**pp == ',') {
    
        report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "+++handle_org");
        (*pp)++;
    
    }
    
    do_org (section, &expr, fill_value);

}

extern void machine_dependent_assemble_line (char *start, char *line);

extern void machine_dependent_handle_proc (char *start, char **pp, char *name);
extern void machine_dependent_handle_endp (char *start, char *name);

extern void machine_dependent_handle_segment (char *start, char **pp, char *name);
extern void machine_dependent_handle_ends (char *start, char *name);

static char *find_end_of_line (char *line) {

    while (!is_end_of_line[(int) *line]) {
    
        if (line++[0] == '\"') {
        
            while (*line && *line != '\"') {
            
                if (line++[0] == '\\' && *line) {
                    line++;
                }
            
            }
        
        }
    
    }
    
    return line;

}

extern void *machine_dependent_find_templates (char *name, int check_suffix);
extern void *machine_dependent_find_reg_entry (char *name);

static void process_line (char *line, char *line_end) {

    char *start = line, *caret;
    char *arg = 0, saved_ch;
    
    struct pseudo_op_entry *poe;
    int has_colon;
    
    caret = (line = skip_whitespace (line));
    
    if (caret >= line_end) {
        return;
    }
    
    while (line < line_end) {
    
        has_colon = 0;
        
        if (is_name_beginner ((int) *line)) {
        
            caret = line;
            
            if (!(arg = symname (&line))) {
                goto check;
            }
            
            if (xstrcasecmp (arg, "assume") == 0 || xstrcasecmp (arg, "dgroup") == 0) {
            
                ignore_rest_of_line (&line);
                goto check;
            
            }
            
            if (xstrcasecmp (arg, "equ") == 0) {
            
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "equ not preceded by label");
                
                ignore_rest_of_line (&line);
                goto check;
            
            }
            
            if (xstrcasecmp (arg, "org") == 0) {
            
                line = skip_whitespace (line);
                
                handle_org (start, &line);
                goto check;
            
            }
            
            if (xstrcasecmp (arg, "proc") == 0 || xstrcasecmp (arg, "endp") == 0) {
            
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "procedure must have a name");
                
                ignore_rest_of_line (&line);
                goto check;
            
            }
            
            if (xstrcasecmp (arg, "segment") == 0 || xstrcasecmp (arg, "ends") == 0) {
            
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "segment must have a name");
                
                ignore_rest_of_line (&line);
                goto check;
            
            }
            
            line = skip_whitespace (line);
            
            if ((poe = find_poe (arg))) {
            
                poe->handler (start, &line);
                goto check;
            
            }
            
            if (machine_dependent_find_templates (arg, 1)) {
            
                saved_ch = *(line = find_end_of_line (line));
                *line = '\0';
                
                machine_dependent_assemble_line (start, skip_whitespace (start));
                *line = saved_ch;
                
                goto check;
            
            }
            
            if (!machine_dependent_find_reg_entry (arg)) {
            
                char *temp, *directive;
                struct symbol *symbol;
                
                if (*line == ':' || is_end_of_line[(int) *line]) {
                
                    if ((has_colon = (*line == ':'))) {
                    
                        temp = (line = skip_whitespace (line + 1));
                        
                        if ((directive = symname (&line))) {
                        
                            if (xstrcasecmp (directive, "equ") == 0) {
                            
                                assign_symbol (start, &line, arg);
                                
                                free (directive);
                                goto check;
                            
                            }
                            
                            free (directive);
                        
                        }
                        
                        line = temp;
                    
                    }
                    
                    symbol = symbol_label (start, caret, arg);
                    symbol->scope = SYMBOL_SCOPE_LOCAL;
                    
                    if (!has_colon) {
                        report_line_at (get_filename (), get_line_number (), REPORT_WARNING, start, caret, "label alone without colon");
                    }
                    
                    /*free (arg);*/
                    continue;
                
                }
                
                temp = line;
                
                if ((directive = symname (&line))) {
                
                    if ((poe = find_data_poe (directive))) {
                    
                        free (directive);
                        
                        symbol = symbol_label (start, caret, arg);
                        symbol->scope = SYMBOL_SCOPE_LOCAL;
                        
                        poe->handler (start, &line);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "equ") == 0) {
                    
                        assign_symbol (start, &line, arg);
                        
                        free (directive);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "label") == 0) {
                    
                        symbol = symbol_label (start, caret, arg);
                        symbol->scope = SYMBOL_SCOPE_LOCAL;
                        
                        free (directive);
                        
                        ignore_rest_of_line (&line);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "proc") == 0) {
                    
                        machine_dependent_handle_proc (start, &line, arg);
                        
                        free (directive);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "endp") == 0) {
                    
                        machine_dependent_handle_endp (start, arg);
                        
                        free (directive);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "segment") == 0) {
                    
                        machine_dependent_handle_segment (start, &line, arg);
                        
                        free (directive);
                        goto check;
                    
                    }
                    
                    if (xstrcasecmp (directive, "ends") == 0) {
                    
                        machine_dependent_handle_ends (start, arg);
                        
                        free (directive);
                        goto check;
                    
                    }
                    
                    free (directive);
                
                }
                
                line = temp;
            
            }
            
            line = caret;
        
        }
        
    check:
        
        if (arg) { free (arg); }
        arg = 0;
        
        line = skip_whitespace (line);
        
        if (is_end_of_line[(int) *line]) {
            break;
        }
        
        report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, line, "junk '%c' at the end of line", *line);
        ignore_rest_of_line (&line);
    
    }

}

void process_file (const char *ifile) {

    char *start, *arg, *caret;
    FILE *fp;
    
    char *line, *line_end, *real_line;
    unsigned long real_line_len;
    
    unsigned long newlines;
    unsigned long new_line_number;
    
    struct pp_pseudo_op_entry *poe;
    void *load_line_internal_data = NULL;
    
    struct cond *cond;
    int cond_idx, proc_idx, seg_idx;
    
    if (!ifile || strcmp (ifile, "-") == 0) {
    
        set_filename_and_line_number (xstrdup ("<stdin>"), 0);
        fp = stdin;
    
    } else {
    
        set_filename_and_line_number (xstrdup (ifile), 0);
        
        if (!(fp = fopen (ifile, "r"))) {
        
            report_at (program_name, 0, REPORT_FATAL_ERROR, "Failed to open '%s' for reading", ifile);
            return;
        
        }
    
    }
    
    new_line_number = 1;
    load_line_internal_data = load_line_create_internal_data (&new_line_number);
    
    while (!load_line (&line, &line_end, &real_line, &real_line_len, &newlines, fp, &load_line_internal_data)) {
    
        set_line_number (new_line_number);
        new_line_number += newlines + 1;
        
        if (state->lfile) {
        
            update_listing_line (current_frag);
            add_listing_line (real_line, real_line_len, get_filename (), get_line_number ());
        
        }
        
        start = line;
        caret = (line = skip_whitespace (line));
        
        if (!ignore_line && line >= line_end) {
            continue;
        }
        
        if (*line == '%') {
        
            caret = (line = skip_whitespace (line + 1));
            
            if (is_name_beginner ((int) *line)) {
            
                while (is_name_part ((int) *line)) {
                    line++;
                }
                
                arg = xstrndup (caret, line - caret);
                line = skip_whitespace (line);
                
                if ((poe = find_cond_directive (arg))) {
                
                    poe->handler (start, &line);
                    free (arg);
                    
                    continue;
                
                }
                
                if (!ignore_line) {
                
                    if ((poe = find_directive (arg))) {
                    
                        poe->handler (start, &line);
                        free (arg);
                        
                        continue;
                    
                    }
                    
                    report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "unknown preprocessor directive '%%%s'", arg);
                
                }
                
                free (arg);
                continue;
            
            }
            
            if (!is_end_of_line[(int) *caret] && !ignore_line) {
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "unknown preprocessor directive '%%%c'", *caret);
            }
            
            continue;
        
        }
        
        if (is_name_beginner ((int) *line)) {
        
            if ((arg = symname (&line))) {
            
                line = skip_whitespace (line);
                
                if ((poe = find_cond_directive (arg))) {
                
                    poe->handler (start, &line);
                    
                    free (arg);
                    continue;
                
                }
            
            }
            
            line = caret;
        
        }
        
        if (!ignore_line) {
        
            if (line < line_end) {
            
                char *tokenized_line = preprocess_line (line, 0);
                
                process_line (tokenized_line, tokenized_line + strlen (tokenized_line));
                free (tokenized_line);
            
            }
        
        }
    
    }
    
    if (state->lfile) {
        update_listing_line (current_frag);
    }
    
    load_line_destroy_internal_data (load_line_internal_data);
    
    if (!includes) {
    
        for (cond_idx = 0; cond_idx < vec_ifstack.length; cond_idx++) {
        
            cond = vec_ifstack.data[cond_idx];
            report_at (cond->filename, cond->line_number, REPORT_ERROR, "unterminated %%%s statement", cond->directive);
            
            free (cond->filename);
            free (cond->directive);
            
            free (cond);
        
        }
        
        for (proc_idx = 0; proc_idx < state->procs.length; proc_idx++) {
        
            struct proc *proc = (struct proc *) state->procs.data[proc_idx];
            report_at (proc->filename, proc->line_number, REPORT_ERROR, "procedure %s is not closed", proc->name);
        
        }
        
        for (seg_idx = 0; seg_idx < state->segs.length; seg_idx++) {
        
            struct segment *seg = (struct segment *) state->segs.data[seg_idx];
            report_at (seg->filename, seg->line_number, REPORT_ERROR, "segment %s is not closed", seg->name);
        
        }
    
    }
    
    if (fp != stdin) { fclose (fp); }

}
