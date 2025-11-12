/******************************************************************************
 * @file            lib.h
 *****************************************************************************/
#ifndef     _LIB_H
#define     _LIB_H

#define     ARRAY_SIZE(arr)             (sizeof (arr) / sizeof (arr[0]))

char get_symbol_name_end (char **pp);

char *skip_whitespace (char *__p);
char *symname (char **pp);

char *xstrdup (const char *__p);
char *xstrndup (const char *__p, unsigned long __len);

int xstrcasecmp (const char *__s1, const char *__s2);
int xstrncasecmp (const char *__s1, const char *__s2, unsigned long __len);

char *to_lower (const char *__p);

void ignore_rest_of_line (char **pp);
void write_to_byte_array (unsigned char *arr, unsigned long value, int size);


void add_include_path (const char *__p);
void parse_args (int argc, char **argv, int optind);

void *xmalloc (unsigned long __size);
void *xrealloc (void *__ptr, unsigned long __size);


const char *get_filename (void);
unsigned long get_line_number (void);

void get_filename_and_line_number (const char **__filename_p, unsigned long *__line_number_p);
void set_line_number (unsigned long __line_number);
void set_filename_and_line_number (const char *__filename, unsigned long __line_number);

#endif      /* _LIB_H */
