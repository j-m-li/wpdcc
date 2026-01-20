/******************************************************************************
 * @file            ll.c
 *****************************************************************************/
#include    <stddef.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#include    "ll.h"
#include    "report.h"

struct load_line_data {

    char *line, *real_line;
    
    unsigned long capacity, read_size;
    unsigned long end_of_prev_real_line;
    
    unsigned long *new_line_number_p;

};

#define     CAPACITY_INCREMENT          256
extern void get_filename_and_line_number (const char **__filename_p, unsigned long *__line_number_p);

extern void *xmalloc (unsigned long __size);
extern void *xrealloc (void *__ptr, unsigned long __size);

int load_line (char **line_p, char **line_end_p, char **real_line_p, unsigned long *real_line_len_p, unsigned long *newlines_p, FILE *ifp, void **load_line_internal_data_p) {

    struct load_line_data *ll_data = *load_line_internal_data_p;
    unsigned long pos_in_line = 0, pos_in_real_line = 0, newlines = 0;
    
    int in_escape = 0, in_double_quote = 0, in_single_quote = 0;
    int in_line_comment = 0, skipping_spaces = 0;
    
    if (ll_data->end_of_prev_real_line) {
    
        memmove (ll_data->real_line, ll_data->real_line + ll_data->end_of_prev_real_line, ll_data->read_size - ll_data->end_of_prev_real_line);
        ll_data->read_size -= ll_data->end_of_prev_real_line;
    
    }
    
    while (1) {
    
        if (pos_in_line >= ll_data->capacity || pos_in_real_line >= ll_data->capacity) {
        
            ll_data->capacity += CAPACITY_INCREMENT;
            
            ll_data->line = xrealloc (ll_data->line, ll_data->capacity + 2);
            ll_data->real_line = xrealloc (ll_data->real_line, ll_data->capacity + 1);
        
        }
        
        if (pos_in_real_line >= ll_data->read_size) {
        
            ll_data->read_size = fread (ll_data->real_line + pos_in_real_line, 1, ll_data->capacity - pos_in_real_line, ifp) + pos_in_real_line;
            
            if (ferror (ifp)) {
                return 4;
            }
            
            ll_data->real_line[ll_data->read_size] = '\0';
        
        }
        
    copying:
        
        if (in_line_comment) {
        
            while (pos_in_real_line < ll_data->read_size) {
            
                if (ll_data->real_line[pos_in_real_line] == '\n') {
                
                    in_line_comment = 0;
                    break;
                
                }
                
                pos_in_real_line++;
            
            }
        
        }
        
        if (skipping_spaces) {
        
            while (pos_in_real_line < ll_data->read_size) {
            
                if (ll_data->real_line[pos_in_real_line] != ' ' && ll_data->real_line[pos_in_real_line] != '\t') {
                
                    skipping_spaces = 0;
                    break;
                
                }
                
                pos_in_real_line++;
            
            }
        
        }
        
        while (pos_in_real_line < ll_data->read_size && pos_in_line < ll_data->capacity) {
        
            ll_data->line[pos_in_line] = ll_data->real_line[pos_in_real_line++];
            
            if (in_double_quote || in_single_quote) {
            
                if (in_escape) {
                    in_escape = 0;
                } else if (in_double_quote && ll_data->line[pos_in_line] == '"') {
                    in_double_quote = 0;
                } else if (in_single_quote && ll_data->line[pos_in_line] == '\'') {
                    in_single_quote = 0;
                } else if (ll_data->line[pos_in_line] == '\\') {
                    in_escape = 1;
                }
                
                if (ll_data->line[pos_in_line] == '\n') {
                
                    int pos = pos_in_line;
                    
                    if (pos > 0 && ll_data->line[pos - 1] == '\r') {
                        ll_data->line[--pos] = '\n';
                    }
                    
                    if (pos > 0) {
                    
                        if (ll_data->line[pos - 1] != '\\') {
                    
                            ll_data->line[pos + 1] = '\0';
                            ll_data->end_of_prev_real_line = pos_in_real_line;
                            
                            *line_p = ll_data->line;
                            *line_end_p = ll_data->line + pos;
                            
                            *real_line_p = ll_data->real_line;
                            *real_line_len_p = pos_in_real_line;
                            
                            *newlines_p = newlines;
                            return 0;
                        
                        } else {
                        
                            pos_in_line = pos - 1;
                            
                            newlines++;
                            goto copying;
                        
                        }
                    
                    }
                
                }
            
            } else {
            
                if (ll_data->line[pos_in_line] == ' ' || ll_data->line[pos_in_line] == '\t') {
                
                    /*ll_data->line[pos_in_line++] = ' ';*/
                    
                    /*skipping_spaces = 1;
                    goto copying;*/
                    
                    if (ll_data->line[pos_in_line] == '\t') {
                    
                        int cnt = 4 - (pos_in_line % 4);
                        int i = 0;
                        
                        ll_data->capacity += CAPACITY_INCREMENT;
                        
                        ll_data->line = xrealloc (ll_data->line, ll_data->capacity + 2);
                        ll_data->real_line = xrealloc (ll_data->real_line, ll_data->capacity + 1);
                        
                        for (; i < cnt; i++) {
                            ll_data->line[pos_in_line++] = ' ';
                        }
                        
                        continue;
                    
                    }
                
                } else if (ll_data->line[pos_in_line] == '\n') {
                
                    if (pos_in_line > 0 && ll_data->line[pos_in_line - 1] == '\r') {
                        ll_data->line[--pos_in_line] = '\n';
                    }
                    
                    ll_data->line[pos_in_line + 1] = '\0';
                    ll_data->end_of_prev_real_line = pos_in_real_line;
                    
                    *line_p = ll_data->line;
                    *line_end_p = ll_data->line + pos_in_line;
                    
                    *real_line_p = ll_data->real_line;
                    *real_line_len_p = pos_in_real_line;
                    
                    *newlines_p = newlines;
                    return 0;
                
                } else if (ll_data->line[pos_in_line] == '\\') {
                
                    /*ll_data->line[pos_in_line] = ' ';*/
                    pos_in_line--;
                    
                    while (pos_in_real_line < ll_data->read_size) {
                    
                        if (ll_data->real_line[pos_in_real_line] == '\r' || ll_data->real_line[pos_in_real_line] == '\n') {
                        
                            if (ll_data->real_line[pos_in_real_line] == '\r') {
                                pos_in_real_line++;
                            }
                            
                            if (ll_data->real_line[pos_in_real_line] == '\n') {
                                pos_in_real_line++;
                            }
                            
                            break;
                        
                        }
                        
                        pos_in_real_line++;
                    
                    }
                    
                    newlines++;
                    continue;
                
                } else if (ll_data->line[pos_in_line] == '"') {
                    in_double_quote = 1;
                } else if (ll_data->line[pos_in_line] == '\'') {
                    in_single_quote = 1;
                } else if (ll_data->line[pos_in_line] == ';') {
                
                    in_line_comment = 1;
                    goto copying;
                
                }
            
            }
            
            pos_in_line++;
        
        }
        
        if (feof (ifp)) {
        
            const char *filename;
            unsigned long line_number;
            
            if (ll_data->read_size == 0) {
                return 1;
            }
            
            ll_data->line[pos_in_line] = '\n';
            ll_data->line[pos_in_line + 1] = '\0';
            
            get_filename_and_line_number (&filename, &line_number);
            
            if (ll_data->new_line_number_p) {
                line_number = *(ll_data->new_line_number_p);
            } else {
                line_number = 0;
            }
            
            report_at (filename, line_number, REPORT_WARNING, "end of file not at end of line; newline inserted");
            
            ll_data->end_of_prev_real_line = 0;
            ll_data->read_size = 0;
            
            *line_p = ll_data->line;
            *line_end_p = ll_data->line + pos_in_line;
            
            *real_line_p = ll_data->real_line;
            *real_line_len_p = pos_in_real_line;
            
            *newlines_p = newlines;
            return 0;
        
        }
    
    }

}

void load_line_destroy_internal_data (void *load_line_internal_data) {

    struct load_line_data *ll_data;
    
    if (load_line_internal_data) {
    
        ll_data = load_line_internal_data;
        
        free (ll_data->line);
        free (ll_data->real_line);
        free (ll_data);
    
    }

}

void *load_line_create_internal_data (unsigned long *new_line_number_p) {

    struct load_line_data *ll_data = xmalloc (sizeof (*ll_data));;
    
    ll_data->capacity = 0;
    ll_data->line = NULL;
    ll_data->real_line = NULL;
    
    ll_data->read_size = 0;
    ll_data->end_of_prev_real_line = 0;
    
    ll_data->new_line_number_p = new_line_number_p;
    return ll_data;

}
