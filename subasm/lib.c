/******************************************************************************
 * @file            lib.c
 *****************************************************************************/
#include    <ctype.h>
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "lex.h"
#include    "lib.h"
#include    "report.h"

#if     defined (_WIN32)
# define    PATHSEP                     ';'
#else
# define    PATHSEP                     ':'
#endif

struct as_option {

    const char *name;
    int idx, flgs;

};

#define     AS_OPTION_NO_ARG            0
#define     AS_OPTION_HAS_ARG           1

#define     AS_OPTION_NONE              0
#define     AS_OPTION_DEFINE            1
#define     AS_OPTION_FORMAT            2
#define     AS_OPTION_HELP              3
#define     AS_OPTION_INCLUDE           4
#define     AS_OPTION_LISTING           5
#define     AS_OPTION_OUTFILE           6
#define     AS_OPTION_UNDEF             7

static struct as_option opts[] = {

    {   "-D",           AS_OPTION_DEFINE,       AS_OPTION_HAS_ARG   },
    {   "-I",           AS_OPTION_INCLUDE,      AS_OPTION_HAS_ARG   },
    {   "-U",           AS_OPTION_UNDEF,        AS_OPTION_HAS_ARG   },
    
    {   "-f",           AS_OPTION_FORMAT,       AS_OPTION_HAS_ARG   },
    {   "-l",           AS_OPTION_LISTING,      AS_OPTION_HAS_ARG   },
    {   "-o",           AS_OPTION_OUTFILE,      AS_OPTION_HAS_ARG   },
    
    {   "--help",       AS_OPTION_HELP,         AS_OPTION_NO_ARG    },
    {   0,              0,                      0                   }

};


static int _strstart (const char *val, const char **str) {

    const char *p = *str;
    const char *q = val;
    
    while (*q != '\0') {
    
        if (*p != *q) {
            return 0;
        }
        
        ++p;
        ++q;
    
    }
    
    *str = p;
    return 1;

}

static void _print_usage (void) {

    if (program_name) {
    
        fprintf (stderr, "Usage: %s [options] file\n\n", program_name);
        fprintf (stderr, "Options:\n\n");
        
        fprintf (stderr, "    -Dname[=value]        Define 'name' with value 'value'.\n");
        fprintf (stderr, "    -I DIR                Add DIR to include search path.\n");
        fprintf (stderr, "    -Uname                Undefine 'name'.\n");
        
        fprintf (stderr, "\n");
        fprintf (stderr, "    -f FORMAT             Specify the format of object file (default elks-ia16)\n");
        fprintf (stderr, "                              Supported formats are:\n");
        fprintf (stderr, "                                  elks-ia16, elks-i386,\n");
        fprintf (stderr, "                                  coff, win32, win64\n");
        fprintf (stderr, "    -l FILE               Print listings to file FILE.\n");
        fprintf (stderr, "    -o OBJFILE            Name the object-file output OBJFILE (default a.out).\n");
        
        fprintf (stderr, "\n");
        fprintf (stderr, "    --help                Print this help information.\n");
        
        fprintf (stderr, "\n");
    
    }

}


char get_symbol_name_end (char **pp) {

    char c = **pp;
    
    if (is_name_beginner ((int) (*pp)[0])) {
    
        while (is_name_part ((int) (*pp)[0])) {
            (*pp)++;
        }
        
        c = **pp;
    
    }
    
    **pp = '\0';
    return c;

}

char *skip_whitespace (char *__p) {

    while (*__p == ' ' || *__p == '\t') {
        __p++;
    }
    
    return __p;

}

char *symname (char **pp) {

    char *p = *pp;
    
    if (is_name_beginner ((int) **pp)) {
    
        while (is_name_part ((int) **pp)) {
            (*pp)++;
        }
        
        return xstrndup (p, *pp - p);
    
    }
    
    return 0;

}

char *to_lower (const char *__p) {

    int i, len;
    char *p;
    
    len = strlen (__p);
    p = xmalloc (len + 1);
    
    for (i = 0; i < len; i++) {
        p[i] = tolower ((int) __p[i]);
    }
    
    return p;

}

char *xstrdup (const char *__p) {

    char *p = xmalloc (strlen (__p) + 1);
    
    strcpy (p, __p);
    return p;

}

char *xstrndup (const char *__p, unsigned long __len) {

    char *p = xmalloc (__len + 1);
    
    memcpy (p, __p, __len);
    return p;

}

int xstrcasecmp (const char *__s1, const char *__s2) {

    const unsigned char *p1 = (const unsigned char *) __s1;
    const unsigned char *p2 = (const unsigned char *) __s2;
    
    while (*p1 != '\0') {
    
        if (tolower ((int) *p1) < tolower ((int) *p2)) {
            return (-1);
        } else if (tolower ((int) *p1) > tolower ((int) *p2)) {
            return (1);
        }
        
        p1++;
        p2++;
    
    }
    
    if (*p2 == '\0') {
        return (0);
    }
    
    return (-1);

}

int xstrncasecmp (const char *__s1, const char *__s2, unsigned long __len) {

    const unsigned char *p1 = (const unsigned char *) __s1;
    const unsigned char *p2 = (const unsigned char *) __s2;
    
    while (*p1 != '\0' && __len-- > 0) {
    
        if (tolower ((int) *p1) < tolower ((int) *p2)) {
            return (-1);
        } else if (tolower ((int) *p1) > tolower ((int) *p2)) {
            return (1);
        }
        
        p1++;
        p2++;
    
    }
    
    if (*p2 == '\0') {
        return (0);
    }
    
    return (-1);

}

void add_include_path (const char *__p) {

    char *in = xstrdup (__p);
    char *temp = in, *p;
    
    do {
    
        for (p = temp; *p != '\0' && *p != PATHSEP; p++) {
        
            if (*p == '\\') {
                *p = '/';
            }
        
        }
        
        if ((p - temp) > 0) {
        
            int len = (p - temp);
            char *path;
            
            if (*(p - 1) != '/') {
            
                path = xmalloc (2 + (p - temp) + 2);
                sprintf (path, "-I%.*s/", len, temp);
            
            } else {
            
                path = xmalloc (2 + (p - temp) + 1);
                sprintf (path, "-I%.*s", len, temp);
            
            }
            
            list_append (&state->pplist, path);
        
        }
        
        temp = (p + 1);
    
    } while (*p != '\0');
    
    free (in);

}

void write_to_byte_array (unsigned char *arr, unsigned long value, int size) {

    int i;
    
    for (i = 0; i < size; i++) {
        arr[i] = (value >> (8 * i)) & 0xff;
    }

}

void ignore_rest_of_line (char **pp) {

    while (!is_end_of_line[(int) **pp]) {
    
        if ((*pp)++[0] == '\"') {
        
            while (**pp && **pp != '\"') {
            
                if ((*pp)++[0] == '\\' && **pp) {
                    (*pp)++;
                }
            
            }
        
        }
    
    }

}

void parse_args (int argc, char **argv, int optind) {

    struct as_option *popt;
    const char *optarg, *r;
    
    if (argc <= optind) {
    
        _print_usage ();
        exit (EXIT_SUCCESS);
    
    }
    
    while (optind < argc) {
    
        r = argv[optind++];
        
        if (r[0] != '-' || r[1] == '\0') {
        
            if (state->ifile) {
            
                report_at (program_name, 0, REPORT_ERROR, "more than one file passed as input");
                exit (EXIT_FAILURE);
            
            }
            
            state->ifile = xstrdup (r);
            continue;
        
        }
        
        for (popt = opts; ; popt++) {
        
            const char *p1 = popt->name;
            const char *r1 = r;
            
            if (!p1) {
            
                report_at (program_name, 0, REPORT_ERROR, "invalid option -- '%s'", r);
                exit (EXIT_FAILURE);
            
            }
            
            if (!_strstart (p1, &r1)) {
                continue;
            }
            
            optarg = r1;
            
            if (popt->flgs & AS_OPTION_HAS_ARG) {
            
                if (*r1 == '\0') {
                
                    if (optind >= argc) {
                    
                        report_at (program_name, 0, REPORT_ERROR, "argument to '%s' is missing", r);
                        exit (EXIT_FAILURE);
                    
                    }
                    
                    optarg = argv[optind++];
                
                }
            
            } else if (*r1 != '\0') {
                continue;
            }
            
            break;
        
        }
        
        switch (popt->idx) {
        
            case AS_OPTION_DEFINE : {
            
                char *arg;
                
                if (!strchr (optarg, '=')) {
                
                    arg = xmalloc (2 + strlen (optarg) + 3);
                    sprintf (arg, "-D%s=1", optarg);
                
                } else {
                
                    arg = xmalloc (2 + strlen (optarg) + 1);
                    sprintf (arg, "-D%s", optarg);
                
                }
                
                list_append (&state->pplist, arg);
                break;
            
            }
            
            case AS_OPTION_FORMAT: {
            
                if (xstrcasecmp (optarg, "elks-ia16") == 0) {
                
                    state->format = AS_OUTPUT_IA16_ELKS;
                    break;
                
                }
                
                if (xstrcasecmp (optarg, "elks-i386") == 0) {
                
                    state->format = AS_OUTPUT_I386_ELKS;
                    break;
                
                }
                
                if (xstrcasecmp (optarg, "coff") == 0) {
                
                    state->format = AS_OUTPUT_COFF;
                    break;
                
                }
                
                if (xstrcasecmp (optarg, "win32") == 0) {
                
                    state->format = AS_OUTPUT_WIN32;
                    break;
                
                }
                
                if (xstrcasecmp (optarg, "win64") == 0) {
                
                    state->format = AS_OUTPUT_WIN64;
                    break;
                
                }
                
                report_at (program_name, 0, REPORT_ERROR, "unrecognised output format '%s'", optarg);
                exit (EXIT_FAILURE);
            
            }
            
            case AS_OPTION_HELP: {
            
                _print_usage ();
                exit (EXIT_SUCCESS);
            
            }
            
            case AS_OPTION_INCLUDE: {
            
                add_include_path (optarg);
                break;
            
            }
            
            case AS_OPTION_LISTING: {
            
                if (state->lfile) {
                
                    report_at (program_name, 0, REPORT_ERROR, "multiple listing files provided");
                    exit (EXIT_FAILURE);
                
                }
                
                state->lfile = xstrdup (optarg);
                break;
            
            }
            
            case AS_OPTION_OUTFILE: {
            
                if (state->ofile) {
                
                    report_at (program_name, 0, REPORT_ERROR, "multiple output files provided");
                    exit (EXIT_FAILURE);
                
                }
                
                state->ofile = xstrdup (optarg);
                break;
            
            }
            
            case AS_OPTION_UNDEF: {
            
                char *arg = xmalloc (2 + strlen (optarg) + 1);
                sprintf (arg, "-U%s", optarg);
                
                list_append (&state->pplist, arg);
                break;
            
            }
            
            default: {
            
                report_at (program_name, 0, REPORT_ERROR, "unsupported option '%s'", r);
                exit (EXIT_FAILURE);
            
            }
        
        }
    
    }
    
    if (!state->ofile) { state->ofile = "a.out"; }

}

void *xmalloc (unsigned long __size) {

    void *ptr = malloc (__size);
    
    if (!ptr && __size) {
    
        report_at (program_name, 0, REPORT_ERROR, "memory full (malloc)");
        exit (EXIT_FAILURE);
    
    }
    
    memset (ptr, 0, __size);
    return ptr;

}

void *xrealloc (void *__ptr, unsigned long __size) {

    void *ptr = realloc (__ptr, __size);
    
    if (!ptr && __size) {
    
        report_at (program_name, 0, REPORT_ERROR, "memory full (realloc)");
        exit (EXIT_FAILURE);
    
    }
    
    return ptr;

}


static const char *filename = 0;
static unsigned long line_number = 0;

const char *get_filename (void) {
    return filename;
}

unsigned long get_line_number (void) {
    return line_number;
}

void get_filename_and_line_number (const char **__filename_p, unsigned long *__line_number_p) {

    *__filename_p = filename;
    *__line_number_p = line_number;

}

void set_filename_and_line_number (const char *__filename, unsigned long __line_number) {

    filename = __filename;
    line_number = __line_number;

}

void set_line_number (unsigned long __line_number) {
    line_number = __line_number;
}
