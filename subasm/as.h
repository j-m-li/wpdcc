/******************************************************************************
 * @file            as.h
 *****************************************************************************/
#ifndef     _AS_H
#define     _AS_H

#include    <stdio.h>

#include    "list.h"
#include    "symbol.h"
#include    "vector.h"

struct proc {

    struct vector regs, args;
    char *name;
    
    char *filename;
    unsigned long line_number;

};

struct segment {

    char *name, *old_section, *old_cpu;
    int old_bits;
    
    char *filename;
    unsigned long line_number;

};

#define     AS_OUTPUT_IA16_ELKS         0x00
#define     AS_OUTPUT_I386_ELKS         0x01
#define     AS_OUTPUT_COFF              0x02
#define     AS_OUTPUT_WIN32             0x03
#define     AS_OUTPUT_WIN64             0x04

struct as_state {

    const char *ifile, *ofile, *lfile;
    FILE *ofp;
    
    struct list *pplist;
    int model, data_size;
    
    struct vector procs;
    struct vector segs;
    
    char *ext;
    
    struct symbol *end_symbol;
    int format;

};

extern struct as_state *state;
extern const char *program_name;

#endif      /* _AS_H */
