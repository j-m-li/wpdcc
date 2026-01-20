/******************************************************************************
 * @file            kwd.c
 *****************************************************************************/
#include    <ctype.h>
#include    <limits.h>
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "expr.h"
#include    "fixup.h"
#include    "frag.h"
#include    "hashtab.h"
#include    "kwd.h"
#include    "lex.h"
#include    "lib.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

extern struct section *machine_dependent_simplified_expression_read_into (char *start, char **pp, struct expr *expr);

static struct hashtab hashtab_pseudo_ops = { 0 };
static struct hashtab hashtab_data_pseudo_ops = { 0 };

static int read_character (const char *start, char **pp, unsigned long *val) {

    int ch, i;
    
    switch (ch = *((*pp)++)) {
    
        case '"':
        
            return 1;
        
        case '\0':
        
            report_at (get_filename (), get_line_number (), REPORT_WARNING, "null character in string; '\"' inserted");
            
            (*pp)--;                    /* Might be the end of line buffer. */
            return 1;
        
        case '\n':
        
            report_at (get_filename (), get_line_number (), REPORT_WARNING, "unterminated string; newline inserted");
            
            set_line_number (get_line_number () + 1);
            *val = ch;
            
            break;
        
        case '\\':
        
            switch (ch = *((*pp)++)) {
            
                case '0':   case '1':
                case '2':   case '3':
                case '4':   case '5':
                case '6':   case '7':
                
                    for (i = 0, *val = 0; isdigit (ch) && (i < 3); (ch = *((*pp)++)), i++) {
                        *val = *val * 8 + (ch - '0');
                    }
                    
                    (*pp)--;
                    break;
                
                case 'x':   case 'X':
                
                    ch = *((*pp)++);
                    
                    for (i = 0, *val = 0; isxdigit (ch) && (i < 3); (ch = *((*pp)++)), i++) {
                    
                        if (isdigit (ch)) {
                            *val = *val * 16 + (ch - '0');
                        } else if (isupper (ch)) {
                            *val = *val * 16 + ((ch = 'A') + 10);
                        } else {
                            *val = *val * 16 + ((ch = 'a') + 10);
                        }
                    
                    }
                    
                    (*pp)--;
                    break;
                
                case 'r':
                
                    *val = 13;
                    break;
                
                case 'n':
                
                    *val = 10;
                    break;
                
                case 't':
                
                    *val = 9;
                    break;
                
                case '\\':
                case '"':
                
                    *val = ch;
                    break;
                
                default:
                
                    report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, *pp - 1, "unknown escape sequence: '\\%c'", ch);
                    
                    *val = ch;
                    break;
            
            }
            
            break;
        
        default:
        
            *val = ch;
            break;
    
    }
    
    return 0;

}

static void align_bytes (char *start, char **pp, int first_arg_is_bytes) {

    int64_t alignment;
    int fill_specified;
    
    int64_t fill_value = 0, max_bytes_to_skip;
    int64_t i;
    
    alignment = get_result_of_absolute_expression (start, pp);

    if (first_arg_is_bytes) {

        /* Converts to log2. */    
        for (i = 0; (alignment & 1) == 0; alignment >>= 1, i++);
        
        if (alignment != 1) {
            report_at (get_filename (), get_line_number (), REPORT_ERROR, "alignment is not a power of 2");
        }
        
        alignment = i;

    }

    if (**pp != ',') {

        fill_specified = 0;
        max_bytes_to_skip = 0;

    } else {

        *pp = skip_whitespace (*pp + 1);

        if (**pp == ',') {
        
            fill_specified = 0;
            *pp = skip_whitespace (*pp + 1);
        
        } else {
            
            fill_value = get_result_of_absolute_expression (start, pp);
            fill_specified = 1;
            
        }
        
        
        if (**pp != ',') {
            max_bytes_to_skip = 0;
        } else {

            *pp = skip_whitespace (*pp + 1);
            max_bytes_to_skip = get_result_of_absolute_expression (start, pp);

        }

    }
    
    if (fill_specified) {
        frag_align (alignment, fill_value, max_bytes_to_skip);
    } else {

        if (current_section == text_section) {
            frag_align_code (alignment, max_bytes_to_skip);
        } else {
            frag_align (alignment, 0, max_bytes_to_skip);
        }

    }

}

void machine_dependent_number_to_chars (unsigned char *p, uint64_t number, unsigned int size);

static unsigned long array_to_integer (unsigned char *arr, int size) {

    unsigned long value = 0;
    int i;
    
    for (i = 0; i < size; i++) {
        value |= (unsigned long) arr[i] << (CHAR_BIT * i);
    }
    
    return value;

}

static void handle_constant (char *start, char **pp, int size) {

    struct expr expr, val;
    
    signed long count = 0, repeat, i;
    unsigned long val2;
    
    unsigned char *buf;
    char *temp, *arg;
    
    while (1) {
    
        *pp = skip_whitespace (*pp);
        
        if (**pp == '"') {
        
            unsigned long val;
            int i;
            
            (*pp)++;
            
            while (!read_character (start, pp, &val)) {
            
                for (i = 0; i < size; i++) {
                    frag_append_1_char ((val >> (8 * i)) & 0xff);
                }
            
            }
        
        } else {
        
            machine_dependent_simplified_expression_read_into (start, pp, &expr);
            
            if (!is_end_of_line[(int) **pp]) {
            
                temp = (*pp = skip_whitespace (*pp));
                
                if (is_name_beginner ((int) **pp)) {
                
                    if (!(arg = symname (pp))) {
                        goto no_repeat;
                    }
                    
                    if (xstrcasecmp (arg, "dup")) {
                    
                        free (arg);
                        goto no_repeat;
                    
                    }
                    
                    *pp = skip_whitespace (*pp);
                    
                    if (**pp != '(') {
                    
                        report_at (get_filename (), get_line_number (), REPORT_ERROR, "expected '(' after dup");
                        
                        ignore_rest_of_line (pp);
                        return;
                    
                    }
                    
                    if (expr.type == EXPR_TYPE_CONSTANT) {
                    
                        repeat = expr.add_number;
                        
                        if (repeat == 0) {
                        
                            report_at (get_filename (), get_line_number (), REPORT_WARNING, "dup repeat count is zero; ignored");
                            
                            ignore_rest_of_line (pp);
                            return;
                        
                        }
                        
                        if (repeat < 0) {
                        
                            report_at (get_filename (), get_line_number (), REPORT_WARNING, "dup repeat count is negative; ignored");
                            
                            ignore_rest_of_line (pp);
                            return;
                        
                        }
                        
                        (*pp)++;
                        
                        for (;;) {
                        
                            *pp = skip_whitespace (*pp);
                            
                            if (is_end_of_line[(int) **pp]) {
                                break;
                            }
                            
                            machine_dependent_simplified_expression_read_into (start, pp, &val);
                            
                            if (val.type != EXPR_TYPE_CONSTANT) {
                            
                                report_at (get_filename (), get_line_number (), REPORT_ERROR, "invalid value for dup");
                                
                                ignore_rest_of_line (pp);
                                return;
                            
                            }
                            
                            if (val.add_number != 0 && current_section == bss_section) {
                            
                                report_at (get_filename (), get_line_number (), REPORT_WARNING, "attempt to initialize memory in a nobits section; ignored");
                                val.add_number = 0;
                            
                            }
                            
                            if (val.add_number > 0xff) {
                            
                                report_at (get_filename (), get_line_number (), REPORT_WARNING, "dup value %lu truncated to %lu", val.add_number, val.add_number & 0xff);
                                val.add_number &= 0xff;
                            
                            }
                            
                            machine_dependent_number_to_chars (frag_increase_fixed_size (size), val.add_number, size);
                            count += size;
                            
                            if (*(*pp = skip_whitespace (*pp)) != ',') {
                                break;
                            }
                            
                            (*pp)++;
                        
                        }
                        
                        if (**pp != ')') {
                            report_at (get_filename (), get_line_number (), REPORT_ERROR, "expected ')'");
                        } else {
                            (*pp)++;
                        }
                        
                        buf = current_frag->buf + current_frag->fixed_size - count;
                        
                        for (; repeat > 1; repeat--) {
                        
                            for (i = 0; i < count; i += size) {
                            
                                val2 = array_to_integer (buf + i, size);
                                
                                buf = frag_increase_fixed_size (size);
                                machine_dependent_number_to_chars (buf, val2, size);
                                
                                buf -= (count + i);
                            
                            }
                        
                        }
                        
                        *pp = skip_whitespace (*pp);
                    
                    } else {
                    
                        struct symbol *expr_symbol = make_expr_symbol (&expr);
                        
                        unsigned char *p = frag_alloc_space (symbol_get_value (expr_symbol));
                        *pp = skip_whitespace (*pp);
                        
                        machine_dependent_simplified_expression_read_into (start, pp, &val);
                        
                        if (val.type != EXPR_TYPE_CONSTANT) {
                        
                            report_at (get_filename (), get_line_number (), REPORT_ERROR, "invalid value for dup");
                            
                            ignore_rest_of_line (pp);
                            return;
                        
                        }
                        
                        if (val.add_number != 0 && current_section == bss_section) {
                        
                            report_at (get_filename (), get_line_number (), REPORT_WARNING, "attempt to initialize memory in a nobits section; ignored");
                            val.add_number = 0;
                        
                        }
                        
                        if (val.add_number > 0xff) {
                        
                            report_at (get_filename (), get_line_number (), REPORT_WARNING, "dup value %lu truncated to %lu", val.add_number, val.add_number & 0xff);
                            val.add_number &= 0xff;
                        
                        }
                        
                        *p = val.add_number;
                        frag_set_as_variant (RELAX_TYPE_SPACE, 0, expr_symbol, 0, 0);
                    
                    }
                    
                    return;
                
                }
                
            no_repeat:
                
                *pp = temp;
            
            }
            
            if (expr.type == EXPR_TYPE_CONSTANT) {
                machine_dependent_number_to_chars (frag_increase_fixed_size (size), expr.add_number, size);
            } else if (expr.type != EXPR_TYPE_INVALID) {
            
                fixup_new_expr (current_frag, current_frag->fixed_size, size, &expr, 0, RELOC_TYPE_DEFAULT);
                frag_increase_fixed_size (size);
            
            } else {
            
                report_at (get_filename (), get_line_number (), REPORT_ERROR, "value is not a constant");
                return;
            
            }
        
        }
        
        *pp = skip_whitespace (*pp);
        
        if (**pp != ',') {
            break;
        }
        
        (*pp)++;
    
    }

}


static void handler_align (char *start, char **pp) {
    align_bytes (start, pp, 1);
}

static void handler_bss (char *start, char **pp) {

    (void) start;
    (void) pp;
    
    section_set (bss_section);

}

static void handler_byte (char *start, char **pp) {
    handle_constant (start, pp, 1);
}

static void handler_data (char *start, char **pp) {

    (void) start;
    (void) pp;
    
    section_set (data_section);

}

static void handler_end (char *start, char **pp) {

    char *name, *caret = skip_whitespace (*pp);
    
    if ((name = symname (pp))) {
    
        if (!(state->end_symbol = symbol_find (name))) {
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "undefined symbol '%s'", name);
        } else if (symbol_is_undefined (state->end_symbol) || state->end_symbol->scope == SYMBOL_SCOPE_EXTERN) {
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "undefined symbol '%s'", name);
        }
        
        free (name);
    
    }

}

static void handler_global (char *start, char **pp) {

    struct symbol *symbol;
    char *name, *caret;
    
    for (;;) {
    
        caret = (*pp = skip_whitespace (*pp));
        
        if (!(name = symname (pp))) {
        
            report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "expected symbol name");
            
            ignore_rest_of_line (pp);
            return;
        
        }
        
        if ((symbol = symbol_find (name))) {
        
            if (symbol->scope == SYMBOL_SCOPE_EXTERN) {
                report_line_at (get_filename (), get_line_number (), REPORT_ERROR, start, caret, "symbol '%s' is already defined", name);
            } else {
            
                symbol->scope = SYMBOL_SCOPE_GLOBAL;
                symbol_set_external (symbol);
            
            }
        
        } else {
        
            symbol = symbol_make (name);
            symbol_add_to_chain (symbol);
            
            symbol->scope = SYMBOL_SCOPE_GLOBAL;
            symbol_set_external (symbol);
        
        }
        
        *pp = skip_whitespace (*pp);
        
        if (**pp != ',') {
            break;
        }
        
        (*pp)++;
    
    }

}

static void handler_long (char *start, char **pp) {
    handle_constant (start, pp, 4);
}

static void handler_stack (char *start, char **pp) {

    struct section *curr_sect;
    struct expr expr;
    
    machine_dependent_simplified_expression_read_into (start, pp, &expr);
    
    if (expr.type == EXPR_TYPE_CONSTANT) {
    
        curr_sect = current_section;
        section_set (bss_section);
        
        memset (frag_increase_fixed_size (expr.add_number), 0, expr.add_number);
        section_set (curr_sect);
    
    } else {
    
        report_at (get_filename (), get_line_number (), REPORT_ERROR, "value is not a constant");
        return;
    
    }

}

static void handler_text (char *start, char **pp) {

    (void) start;
    (void) pp;
    
    section_set (text_section);

}

static void handler_word (char *start, char **pp) {
    handle_constant (start, pp, 2);
}

static struct pseudo_op_entry pseudo_op_table[] = {

    {   ".code",        &handler_text           },
    {   ".bss",         &handler_bss            },
    {   ".data",        &handler_data           },
    {   ".data?",       &handler_bss            },
    {   ".stack",       &handler_stack          },
    {   ".text",        &handler_text           },
    
    {   "align",        &handler_align          },
    {   "end",          &handler_end            },
    {   "global",       &handler_global         },
    {   "public",       &handler_global         },
    
    {   0,              0                       }

};

static struct pseudo_op_entry data_pseudo_op_table[] = {

    {   "db",           &handler_byte           },
    {   "dd",           &handler_long           },
    {   "dw",           &handler_word           },
    
    {   0,              0                       }

};

void install_pseudo_op_table (struct pseudo_op_entry *table) {

    struct pseudo_op_entry *entry;
    struct hashtab_name *key;
    
    for (entry = table; entry->name; entry++) {
    
        if (hashtab_get_key (&hashtab_pseudo_ops, entry->name)) {
        
            report_at (program_name, 0, REPORT_ERROR, "duplicate entry '%s'", entry->name);
            continue;
        
        }
        
        if (!(key = hashtab_alloc_name (entry->name))) {
        
            report_at (program_name, 0, REPORT_ERROR, "failed to allocate memory for '%s'", entry->name);
            continue;
        
        }
        
        hashtab_put (&hashtab_pseudo_ops, key, entry);
    
    }

}

void install_data_pseudo_op_table (struct pseudo_op_entry *table) {

    struct pseudo_op_entry *entry;
    struct hashtab_name *key;
    
    for (entry = table; entry->name; entry++) {
    
        if (hashtab_get_key (&hashtab_pseudo_ops, entry->name)) {
        
            report_at (program_name, 0, REPORT_ERROR, "duplicate entry '%s'", entry->name);
            continue;
        
        }
        
        if (!(key = hashtab_alloc_name (entry->name))) {
        
            report_at (program_name, 0, REPORT_ERROR, "failed to allocate memory for '%s'", entry->name);
            continue;
        
        }
        
        hashtab_put (&hashtab_pseudo_ops, key, entry);
    
    }
    
    for (entry = table; entry->name; entry++) {
    
        if (hashtab_get_key (&hashtab_data_pseudo_ops, entry->name)) {
        
            report_at (program_name, 0, REPORT_ERROR, "duplicate entry '%s'", entry->name);
            continue;
        
        }
        
        if (!(key = hashtab_alloc_name (entry->name))) {
        
            report_at (program_name, 0, REPORT_ERROR, "failed to allocate memory for '%s'", entry->name);
            continue;
        
        }
        
        hashtab_put (&hashtab_data_pseudo_ops, key, entry);
    
    }

}

struct pseudo_op_entry *find_poe (char *name) {

    struct hashtab_name *key;
    struct pseudo_op_entry *entry;
    
    char *lname = to_lower (name);
    
    if ((key = hashtab_get_key (&hashtab_pseudo_ops, lname))) {
    
        if ((entry = hashtab_get (&hashtab_pseudo_ops, key))) {
        
            free (lname);
            return entry;
        
        }
    
    }
    
    free (lname);
    return 0;

}

struct pseudo_op_entry *find_data_poe (char *name) {

    struct hashtab_name *key;
    struct pseudo_op_entry *entry;
    
    char *lname = to_lower (name);
    
    if ((key = hashtab_get_key (&hashtab_data_pseudo_ops, lname))) {
    
        if ((entry = hashtab_get (&hashtab_data_pseudo_ops, key))) {
        
            free (lname);
            return entry;
        
        }
    
    }
    
    free (lname);
    return 0;

}

void keywords_init (void) {

    install_pseudo_op_table (pseudo_op_table);
    install_data_pseudo_op_table (data_pseudo_op_table);

}
