/******************************************************************************
 * @file            coff.c
 *****************************************************************************/
#include    <stddef.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#include    "as.h"
#include    "coff.h"
#include    "fixup.h"
#include    "frag.h"
#include    "lib.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

static int output_relocation (FILE *outfile, struct fixup *fixup) {

    struct relocation_entry reloc_entry;
    unsigned int type = 0;
    
    if (fixup->add_symbol == NULL) {
    
        report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "+++output relocation fixup->add_symbol is NULL");
        exit (EXIT_FAILURE);
    
    }
    
    write_to_byte_array (reloc_entry.VirtualAddress, fixup->frag->address + fixup->where, 4);
    
    if (symbol_is_section_symbol (fixup->add_symbol) && !SECTION_IS_NORMAL (symbol_get_section (fixup->add_symbol))) {
        write_to_byte_array (reloc_entry.SymbolTableIndex, ~(unsigned long) 0, 4);
    } else {
        write_to_byte_array (reloc_entry.SymbolTableIndex, symbol_get_symbol_table_index (fixup->add_symbol), 4);
    }
    
    if (state->format == AS_OUTPUT_WIN64) {
    
        switch (fixup->reloc_type) {
        
            case RELOC_TYPE_DEFAULT:
            
                switch (fixup->size) {
                
                    case 8:
                    
                        type = IMAGE_REL_AMD64_ADDR64;
                        break;
                    
                    case 4:
                    
                        type = (fixup->pcrel ? IMAGE_REL_AMD64_REL32 : IMAGE_REL_AMD64_ADDR32);
                        break;
                    
                    default:
                    
                        report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "unsupported COFF relocation size %i for reloc_type RELOC_TYPE_DEFAULT", fixup->size);
                        exit (EXIT_FAILURE);
                
                }
                
                break;
            
            case RELOC_TYPE_RVA:
            
                if (fixup->size != 4) {
                            
                    report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "unsupported COFF relocation size %i for reloc_type RELOC_TYPE_RVA", fixup->size);
                    exit (EXIT_FAILURE);
                
                }
                
                type = IMAGE_REL_AMD64_ADDR32NB;
                break;
            
            default:
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "output_relocation invalid reloc_type", fixup->reloc_type);
                exit (EXIT_FAILURE);
        
        }
    
    } else {
    
        switch (fixup->reloc_type) {
        
            case RELOC_TYPE_DEFAULT:
            case RELOC_TYPE_FAR_CALL:
            
                switch (fixup->size) {
                
                    case 2:
                    
                        type = (fixup->pcrel ? IMAGE_REL_I386_REL16 : IMAGE_REL_I386_DIR16);
                        break;
                    
                    case 4:
                    
                        type = (fixup->pcrel ? IMAGE_REL_I386_REL32 : IMAGE_REL_I386_DIR32);
                        break;
                    
                    default:
                    
                        report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "unsupported COFF relocation size %i for reloc_type RELOC_TYPE_DEFAULT", fixup->size);
                        exit (EXIT_FAILURE);
                
                }
                
                break;
            
            case RELOC_TYPE_RVA:
            
                if (fixup->size != 4) {
                            
                    report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "unsupported COFF relocation size %i for reloc_type RELOC_TYPE_RVA", fixup->size);
                    exit (EXIT_FAILURE);
                
                }
                
                type = IMAGE_REL_I386_DIR32NB;
                break;
            
            default:
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "output_relocation invalid reloc_type", fixup->reloc_type);
                exit (EXIT_FAILURE);
        
        }
    
    }
    
    write_to_byte_array (reloc_entry.Type, type, 2);
    
    if (fwrite (&reloc_entry, sizeof (reloc_entry), 1, outfile) != 1) {
        return 1;
    }
    
    return 0;

}

#define     GET_UINT16(arr)             ((unsigned long) arr[0] | (((unsigned long) arr[1]) << 8))

static unsigned long translate_section_flags_to_Characteristics (unsigned int flags) {

    unsigned long Characteristics = 0;
    
    if (!(flags & SECTION_FLAG_READONLY)) {
        Characteristics |= IMAGE_SCN_MEM_WRITE;
    }
    
    if (flags & SECTION_FLAG_CODE) {
        Characteristics |= IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE;
    }
    
    if (flags & SECTION_FLAG_DATA) {
        Characteristics |= IMAGE_SCN_CNT_INITIALIZED_DATA;
    }
    
    if (flags & SECTION_FLAG_NEVER_LOAD) {
        Characteristics |= IMAGE_SCN_TYPE_NOLOAD;
    }
    
    if (flags & SECTION_FLAG_DEBUGGING) {
        Characteristics |= IMAGE_SCN_LNK_INFO;
    }
    
    if (flags & SECTION_FLAG_EXCLUDE) {
        Characteristics |= IMAGE_SCN_LNK_REMOVE;
    }
    
    if (!(flags & SECTION_FLAG_NOREAD)) {
        Characteristics |= IMAGE_SCN_MEM_READ;
    }
    
    if (flags & SECTION_FLAG_SHARED) {
        Characteristics |= IMAGE_SCN_MEM_SHARED;
    }
    
    /* .bss */
    if ((flags & SECTION_FLAG_ALLOC) && !(flags & SECTION_FLAG_LOAD)) {
        Characteristics |= IMAGE_SCN_CNT_UNINITIALIZED_DATA;
    }
    
    if (flags & SECTION_FLAG_LINK_ONCE) {
        Characteristics |= IMAGE_SCN_LNK_COMDAT;
    }
    
    return Characteristics;

}

void output_coff (FILE *outfile) {

    struct coff_header header;
    struct symbol *symbol;
    
    struct section *section;
    unsigned char temp[4];
    
    unsigned long string_table_size = 4;
    unsigned long NumberOfSymbols = 0;
    
    sections_number (1);
    memset (&header, 0, sizeof (header));
    
    if ((outfile = fopen (state->ofile, "wb")) == NULL) {
    
        report_at (NULL, 0, REPORT_ERROR, "failed to open '%s' as output file", state->ofile);
        return;
    
    }
    
    write_to_byte_array (header.Machine, (state->format == AS_OUTPUT_WIN64 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386), 2);
    write_to_byte_array (header.NumberOfSections, sections_get_count (), 2);
    write_to_byte_array (header.SizeOfOptionalHeader, 0, 2);
    
    if (state->format == AS_OUTPUT_WIN64) {
        write_to_byte_array (header.Characteristics, IMAGE_FILE_LINE_NUMS_STRIPPED, 2);
    } else {
        write_to_byte_array (header.Characteristics, IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_32BIT_MACHINE, 2);
    }
    
    if (fseek (outfile, (sizeof (header) + sections_get_count () * sizeof (struct section_table_entry)), SEEK_SET)) {
    
        report_at (NULL, 0, REPORT_ERROR, "failed to fseek");
        return;
    
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
    
        unsigned long SizeOfRawData = 0, PointerToRawData = 0;
        
        struct section_table_entry *section_header = xmalloc (sizeof (*section_header));
        section_set_object_format_dependent_data (section, section_header);
        
        memset (section_header, 0, sizeof (*section_header));
        
        if (strlen (section_get_name (section)) <= 8) {
            memcpy (section_header->Name, section_get_name (section), strlen (section_get_name (section)));
        } else {
        
            section_header->Name[0] = '/';
            sprintf (section_header->Name + 1, "%lu", string_table_size);
            
            string_table_size += strlen (section_get_name (section)) + 1;
        
        }
        
        write_to_byte_array (section_header->Characteristics, translate_section_flags_to_Characteristics (section_get_flags (section)), 4);
        
        if (section != bss_section) {
        
            PointerToRawData = ftell (outfile);
            section_set (section);
            
            SizeOfRawData = section_write_frag_chain (outfile);;
            
            if (SizeOfRawData == 0) {
                PointerToRawData = 0;
            }
        
        } else {
        
            struct frag *frag;
            
            PointerToRawData = 0;
            section_set (section);
            
            for (frag = current_frag_chain->first_frag; frag; frag = frag->next) {
            
                if (frag->fixed_size == 0) {
                    continue;
                }
                
                SizeOfRawData += frag->fixed_size;
            
            }
        
        }
        
        write_to_byte_array (section_header->SizeOfRawData, SizeOfRawData, 4);
        write_to_byte_array (section_header->PointerToRawData, PointerToRawData, 4);
    
    }
    
    write_to_byte_array (header.PointerToSymbolTable, ftell (outfile), 4);
    write_to_byte_array (header.NumberOfSymbols, 0, 4);
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
    
        struct symbol_table_entry sym_tbl_ent;
        
        unsigned long value = symbol_get_value (symbol);
        unsigned int section_number = 0;
        
        memset (&sym_tbl_ent, 0, sizeof (sym_tbl_ent));
        
        if (symbol->section == undefined_section) {
            section_number = IMAGE_SYM_UNDEFINED;
        } else if (symbol->section == absolute_section) {
            section_number = IMAGE_SYM_ABSOLUTE;
        } else {
            section_number = section_get_number (symbol->section);
        }
        
        write_to_byte_array (sym_tbl_ent.Type, (IMAGE_SYM_DTYPE_NULL << 8) | IMAGE_SYM_TYPE_NULL, 2);
        
        if (symbol_is_external (symbol) || symbol_is_undefined (symbol)) {
            sym_tbl_ent.StorageClass[0] = IMAGE_SYM_CLASS_EXTERNAL;
        } else if (symbol_is_section_symbol (symbol)) {
            sym_tbl_ent.StorageClass[0] = IMAGE_SYM_CLASS_STATIC;
        } else if (symbol_get_section (symbol) == text_section) {
            sym_tbl_ent.StorageClass[0] = IMAGE_SYM_CLASS_LABEL;
        } else {
            sym_tbl_ent.StorageClass[0] = IMAGE_SYM_CLASS_STATIC;
        }
        
        write_to_byte_array (sym_tbl_ent.Value, value, 4);
        sym_tbl_ent.NumberOfAuxSymbols[0] = 0;
        
        write_to_byte_array (sym_tbl_ent.SectionNumber, section_number, 2);
        symbol->write_name_to_string_table = 0;
        
        if (strlen (symbol->name) <= 8) {
            memcpy (sym_tbl_ent.Name, symbol->name, strlen (symbol->name));
        } else {
        
            memset (sym_tbl_ent.Name, 0, 4);
            
            write_to_byte_array ((unsigned char *) sym_tbl_ent.Name + 4, string_table_size, 4);
            string_table_size += strlen (symbol->name) + 1;
            
            symbol->write_name_to_string_table = 1;
        
        }
        
        if (fwrite (&sym_tbl_ent, sizeof (sym_tbl_ent), 1, outfile) != 1) {
        
            report_at (NULL, 0, REPORT_ERROR, "error writing symbol table!");
            return;
        
        }
        
        symbol_set_symbol_table_index (symbol, NumberOfSymbols);
        NumberOfSymbols++;
    
    }
    
    write_to_byte_array (header.NumberOfSymbols, NumberOfSymbols, 4);
    write_to_byte_array (temp, string_table_size, 4);
    
    if (fwrite (temp, 4, 1, outfile) != 1) {
    
        report_at (NULL, 0, REPORT_ERROR, "failed to write string table!");
        return;
    
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {

        if (strlen (section_get_name (section)) > 8) {

            if (fwrite (section_get_name (section), strlen (section_get_name (section)) + 1, 1, outfile) != 1) {
            
                report_at (NULL, 0, REPORT_ERROR, "failed to write string table!");
                return;
            
            }
        
        }

    }
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
    
        if (symbol->write_name_to_string_table) {
        
            if (fwrite (symbol->name, strlen (symbol->name) + 1, 1, outfile) != 1) {
            
                report_at (NULL, 0, REPORT_ERROR, "failed to write string table!");
                return;
            
            }
        
        }
    
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
    
        struct section_table_entry *section_header = section_get_object_format_dependent_data (section);
        struct fixup *fixup;
        
        unsigned long NumberOfRelocations = 0, PointerToRelocations = ftell (outfile);
        section_set (section);
        
        for (fixup = current_frag_chain->first_fixup; fixup; fixup = fixup->next) {
        
            if (fixup->done) {
                continue;
            }
            
            if (output_relocation (outfile, fixup)) {
            
                report_at (NULL, 0, REPORT_ERROR, "failed to write relocation!");
                return;
            
            }
            
            NumberOfRelocations++;
        
        }
        
        write_to_byte_array (section_header->NumberOfRelocations, NumberOfRelocations, 2);
        
        if (NumberOfRelocations == 0) {
            PointerToRelocations = 0;
        }
        
        write_to_byte_array (section_header->PointerToRelocations, PointerToRelocations, 4);
    
    }
    
    rewind (outfile);
    
    if (fwrite (&header, sizeof (header), 1, outfile) != 1) {
    
        report_at (NULL, 0, REPORT_ERROR, "failed to write header!");
        return;
    
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
    
        struct section_table_entry *section_header = section_get_object_format_dependent_data (section);
        
        if (fwrite (section_header, sizeof (*section_header), 1, outfile) != 1) {
        
            report_at (NULL, 0, REPORT_ERROR, "failed to write header!");
            return;
        
        }
    
    }

}
