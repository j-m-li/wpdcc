/******************************************************************************
 * @file            as.c
 *****************************************************************************/
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "lex.h"
#include    "lib.h"
#include    "listing.h"
#include    "process.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

struct as_state *state = 0;
const char *program_name = 0;

extern void output_binary (FILE *fp);
extern void output_coff (FILE *fp);
extern void output_elks (FILE *fp);

extern void keywords_init (void);
extern void sections_init (void);

static void cleanup (void) {

    if (state->ofp) { fclose (state->ofp); }
    
    if (get_error_count () > 0) {
    
        if (state->ofile) {
            remove (state->ofile);
        }
        
        if (state->lfile) {
            remove (state->lfile);
        }
    
    }

}

extern void fixup_code (void);
extern void machine_dependent_init (void);

int main (int argc, char **argv) {

    struct symbol *symbol;
    char *p, *root;
    
    if (argc && *argv) {
    
        program_name = *argv;
        
        if ((p = strrchr (program_name, '/')) || (p = strrchr (program_name, '\\'))) {
            program_name = (p + 1);
        }
    
    }
    
    atexit (cleanup);
    
    state = xmalloc (sizeof (*state));
    parse_args (argc, argv, 1);
    
    if (!state->ifile) {
    
        report_at (program_name, 0, REPORT_ERROR, "no input file specified");
        return EXIT_FAILURE;
    
    }
    
    if (state->ifile && strcmp (state->ifile, "-")) {
    
        if ((p = strrchr (state->ifile, '/')) || (p = strrchr (state->ifile, '\\'))) {
        
            unsigned int len = p - state->ifile;
            
            root = xmalloc (len + 2);
            sprintf (root, "%.*s/", (int) len, state->ifile);
            
            add_include_path (root);
            free (root);
        
        }
    
    }
    
    machine_dependent_init ();
    
    keywords_init ();
    sections_init ();
    
    if (preprocess_init ()) {
        return EXIT_FAILURE;
    }
    
    process_file (state->ifile);
    
    if (get_error_count () > 0) {
        return EXIT_FAILURE;
    }
    
    fixup_code ();
    
    if (state->lfile) {
        generate_listing ();
    }
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
    
        if (symbol_is_external (symbol) && symbol_get_section (symbol) == undefined_section) {
        
            if (symbol->scope == SYMBOL_SCOPE_GLOBAL) {
                report_at (program_name, 0, REPORT_ERROR, "undefined global symbol '%s'", symbol->name);
            }
        
        }
    
    }
    
    if (get_error_count () > 0) {
        return EXIT_FAILURE;
    }
    
    if (!(state->ofp = fopen (state->ofile, "wb"))) {
    
        report_at (program_name, 0, REPORT_ERROR, "failed to open '%s' for writing", state->ofile);
        return EXIT_FAILURE;
    
    }
    
    if (state->format == AS_OUTPUT_I386_ELKS || state->format == AS_OUTPUT_IA16_ELKS) {
        output_elks (state->ofp);
    } else if (state->format == AS_OUTPUT_COFF || state->format == AS_OUTPUT_WIN32 || state->format == AS_OUTPUT_WIN64) {
        output_coff (state->ofp);
    }
    
    if (get_error_count () > 0) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;

}
