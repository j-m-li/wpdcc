/******************************************************************************
 * @file            elks.c
 *****************************************************************************/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "elks.h"
#include    "fixup.h"
#include    "frag.h"
#include    "lib.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

static int output_relocation (struct fixup *fixup, unsigned long start_address_of_section, FILE *fp) {

    struct elks_relocation_info reloc;
    
    long log2_of_size, size;
    unsigned long r_symbolnum;
    
    write_to_byte_array (reloc.r_address, fixup->frag->address + fixup->where - start_address_of_section, 4);
    
    if (symbol_is_section_symbol (fixup->add_symbol)) {
    
        if (symbol_get_section (fixup->add_symbol) == text_section) {
            r_symbolnum = ELKS_N_TEXT;
        } else if (symbol_get_section (fixup->add_symbol) == data_section) {
            r_symbolnum = ELKS_N_DATA;
        } else if (symbol_get_section (fixup->add_symbol) == bss_section) {
            r_symbolnum = ELKS_N_BSS;
        } else {
        
            report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "invalid section %s", section_get_name (symbol_get_section (fixup->add_symbol)));
            exit (EXIT_FAILURE);
        
        }
    
    } else {
    
        struct symbol *symbol;
        long symbol_number;
        
        for (symbol = symbols, symbol_number = 0; symbol && (symbol != fixup->add_symbol); symbol = symbol->next) {
        
            if (symbol_is_external (symbol) || symbol_is_undefined (symbol)) {
                symbol_number++;
            }
        
        }
        
        r_symbolnum  = symbol_number;
        r_symbolnum |= ((long)1 << 31);
    
    }
    
    if (fixup->pcrel) {
        r_symbolnum |= ((long)1 << 28);
    }
    
    if (xstrcasecmp (fixup->add_symbol->name, "DGROUP") == 0) {
        r_symbolnum |= ((long)1 << 27);
    }
    
    for (log2_of_size = -1, size = fixup->size; size; size >>= 1, log2_of_size++);
    r_symbolnum |= ((unsigned long) log2_of_size << 29);
    
    write_to_byte_array (reloc.r_symbolnum, r_symbolnum, 4);
    
    if (fwrite (&reloc, sizeof (reloc), 1, fp) != 1) {
    
        report_at (program_name, 0, REPORT_ERROR, "error writing text relocations");
        return 1;
    
    }
    
    return 0;

}

void output_elks (FILE *fp) {

    unsigned long start_address_of_data;
    struct fixup *fixup;
    
    unsigned long symbol_table_size;
    struct symbol *symbol;
    
    unsigned long string_table_pos;
    struct frag *frag;
    
    unsigned long text_size, data_size, bss_size;
    unsigned long tr_size, dr_size;
    
    struct elks_exec header;
    memset (&header, 0, sizeof (header));
    
    header.a_magic[0] = ((ELKS_MAGIC >> 8) & 0xff);
    header.a_magic[1] = (ELKS_MAGIC & 0xff);
    
    header.a_flags = 0x10;
    header.a_cpu = (state->format == AS_OUTPUT_I386_ELKS) ? 0x10 : 0x04;
    header.a_hdrlen = sizeof (header);
    
    if ((symbol = state->end_symbol)) {
        write_to_byte_array (header.a_entry, symbol_get_value (symbol), 4);
    }
    
    if (fseek (fp, sizeof (header), SEEK_SET)) {
    
        report_at (program_name, 0, REPORT_ERROR, "failed whilst seeking passed header");
        return;
    
    }
    
    section_set (text_section);
    text_size = section_write_frag_chain (fp);
    
    write_to_byte_array (header.a_text, text_size, 4);
    
    section_set (data_section);
    data_size = section_write_frag_chain (fp);
    
    write_to_byte_array (header.a_data, data_size, 4);
    
    section_set (bss_section);
    bss_size = 0;
    
    for (frag = current_frag_chain->first_frag; frag; frag = frag->next) {
    
        if (frag->fixed_size == 0) {
            continue;
        }
        
        bss_size += frag->fixed_size;
    
    }
    
    write_to_byte_array (header.a_bss, bss_size, 4);
    
    section_set (text_section);
    tr_size = 0;
    
    start_address_of_data = 0;
    
    for (fixup = current_frag_chain->first_fixup; fixup; fixup = fixup->next) {
    
        if (fixup->done) {
            continue;
        }
        
        if (output_relocation (fixup, start_address_of_data, fp)) {
            return;
        }
        
        tr_size += sizeof (struct elks_relocation_info);
    
    }
    
    write_to_byte_array (header.a_trsize, tr_size, 4);
    
    section_set (data_section);
    dr_size = 0;
    
    start_address_of_data = current_frag_chain->first_frag->address;
    
    for (fixup = current_frag_chain->first_fixup; fixup; fixup = fixup->next) {
    
        if (fixup->done) {
            continue;
        }
        
        if (output_relocation (fixup, start_address_of_data, fp)) {
            return;
        }
        
        dr_size += sizeof (struct elks_relocation_info);
    
    }
    
    write_to_byte_array (header.a_drsize, dr_size, 4);
    
    symbol_table_size = 0;
    string_table_pos = 4;
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
    
        if (symbol_is_external (symbol) || symbol_is_undefined (symbol)) {
        
            struct elks_nlist symbol_entry;
            memset (&symbol_entry, 0, sizeof (symbol_entry));
            
            write_to_byte_array (symbol_entry.n_strx, string_table_pos, 4);
            string_table_pos += strlen (symbol->name) + 1;
            
            if (state->ext) {
                string_table_pos += strlen (state->ext);
            }
            
            if (symbol->section == undefined_section) {
                symbol_entry.n_type = ELKS_N_UNDF;
            } else if (symbol->section == text_section) {
                symbol_entry.n_type = ELKS_N_TEXT;
            } else if (symbol->section == data_section) {
                symbol_entry.n_type = ELKS_N_DATA;
            } else if (symbol->section == bss_section) {
                symbol_entry.n_type = ELKS_N_BSS;
            } else if (symbol->section == absolute_section) {
                symbol_entry.n_type = ELKS_N_ABS;
            } else {
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "invalid section %s", section_get_name (symbol->section));
                exit (EXIT_FAILURE);
            
            }
            
            write_to_byte_array (symbol_entry.n_value, symbol_get_value (symbol), 4);
            symbol_entry.n_type |= ELKS_N_EXT;
            
            if (fwrite (&symbol_entry, sizeof (symbol_entry), 1, fp) != 1) {
            
                report_at (program_name, 0, REPORT_ERROR, "error writing symbol table");
                return;
            
            }
            
            symbol_table_size += sizeof (symbol_entry);
        
        }
    
    }
    
    write_to_byte_array (header.a_syms, symbol_table_size, 4);
    
    if (fwrite (&string_table_pos, 4, 1, fp) != 1) {
    
        report_at (program_name, 0, REPORT_ERROR, "failed to write string table");
        return;
    
    }
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
    
        if (symbol_is_external (symbol) || symbol_is_undefined (symbol)) {
        
            if (state->ext) {
            
                if (fwrite (state->ext, strlen (state->ext), 1, fp) != 1) {
                
                    report_at (program_name, 0, REPORT_ERROR, "failed to write string table");
                    return;
                
                }
            
            }
            
            if (fwrite (symbol->name, strlen (symbol->name) + 1, 1, fp) != 1) {
            
                report_at (program_name, 0, REPORT_ERROR, "failed to write string table");
                return;
            
            }
        
        }
    
    }
    
    rewind (fp);
    
    if (fwrite (&header, sizeof (header), 1, fp) != 1) {
    
        report_at (program_name, 0, REPORT_ERROR, "failed to write header");
        return;
    
    }

}
