/******************************************************************************
 * @file            report.c
 *****************************************************************************/
#include    <stdarg.h>
#include    <stdio.h>
#include    <string.h>

#include    "report.h"
unsigned int errors = 0;

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

static void output_message (const char *filename, unsigned int lineno, unsigned int idx, int type, const char *fmt, va_list ap) {

    if (filename) {
    
        if (lineno == 0) {
            fprintf (stderr, "%s: ", filename);
        } else {
            fprintf (stderr, "%s:", filename);
        }
    
    }
    
    if (lineno > 0) {
    
        if (idx == 0) {
            fprintf (stderr, "%u: ", lineno);
        } else {
            fprintf (stderr, "%u:", lineno);
        }
    
    }
    
    if (idx > 0) {
        fprintf (stderr, "%u: ", idx);
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

unsigned int get_error_count (void) {
    return errors;
}

void report_at (const char *filename, unsigned int lineno, int type, const char *fmt, ...) {

    va_list ap;
    
    va_start (ap, fmt);
    output_message (filename, lineno, 0, type, fmt, ap);
    va_end (ap);

}

void report_line_at (const char *filename, unsigned int lineno, int type, const char *str, const char *caret, const char *fmt, ...) {

    int ident = 1;
    va_list ap;
    
    unsigned int idx = 0;
    if (str && caret) { idx = (caret - str) + 1; }
    
    va_start (ap, fmt);
    output_message (filename, lineno, idx, type, fmt, ap);
    va_end (ap);
    
    if (str && caret) {
    
        if (lineno > 0) {
            ident = fprintf (stderr, " %8u | ", lineno);
        } else {
            ident = fprintf (stderr, "%*s", 12, "");
        }
        
        fprintf (stderr, "%s", str);
        
        if (str[strlen (str) - 1] != '\n') {
            fprintf (stderr, "\n");
        }
        
        fprintf (stderr, "%*s^\n", (int) (caret - str) + ident, "");
    
    }

}
