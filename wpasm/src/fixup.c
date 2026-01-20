/******************************************************************************
 * @file            fixup.c
 *****************************************************************************/
#include    <stdlib.h>

#include    "as.h"
#include    "expr.h"
#include    "fixup.h"
#include    "frag.h"
#include    "lib.h"
#include    "report.h"
#include    "section.h"
#include    "symbol.h"

static struct fixup *fixup_new_internal (struct frag *frag, unsigned long where, int size, struct symbol *add_symbol, struct symbol *sub_symbol, int64_t add_number, int pcrel, int reloc_type) {

    struct fixup *fixup = xmalloc (sizeof (*fixup));
    
    fixup->frag         = frag;
    fixup->size         = size;
    fixup->add_symbol   = add_symbol;
    fixup->sub_symbol   = sub_symbol;
    fixup->pcrel        = pcrel;
    fixup->reloc_type   = reloc_type;
    fixup->where        = where;
    fixup->add_number   = add_number;
    
    if (current_frag_chain->last_fixup) {
    
        current_frag_chain->last_fixup->next = fixup;
        current_frag_chain->last_fixup = fixup;
    
    } else {
        current_frag_chain->last_fixup = current_frag_chain->first_fixup = fixup;
    }
    
    return fixup;

}
struct fixup *fixup_new_expr (struct frag *frag, unsigned long where, int size, struct expr *expr, int pcrel, int reloc_type) {

    struct symbol *add_symbol = 0;
    struct symbol *sub_symbol = (void*)0;
	
    int64_t add_number = 0;
    
    switch (expr->type) {
    
        case EXPR_TYPE_ABSENT:
        
            break;
        
        case EXPR_TYPE_CONSTANT:
        
            add_number = expr->add_number;
            break;
        
        case EXPR_TYPE_SUBTRACT:
        
            sub_symbol = expr->op_symbol;
            /* fall through */
        
        case EXPR_TYPE_SYMBOL:
        
            add_symbol = expr->add_symbol;
            add_number = expr->add_number;
            
            break;
        
        default:
        
            add_symbol = make_expr_symbol (expr);
            break;
    
    }
    
    return fixup_new_internal (frag, where, size, add_symbol, sub_symbol, add_number, pcrel, reloc_type);

}


struct fixup *fixup_new (struct frag *frag, unsigned long where, int size, struct symbol *add_symbol, int64_t add_number, int pcrel, int reloc_type) {
    return fixup_new_internal (frag, where, size, add_symbol, (void*)0, add_number, pcrel, reloc_type);
}



int64_t machine_dependent_estimate_size_before_relax (struct frag *frag, struct section *section);
int64_t machine_dependent_relax_frag (struct frag *frag, struct section *section, int64_t change);

void machine_dependent_finish_frag (struct frag *frag);

static unsigned long relax_align (unsigned long address, unsigned long alignment) {

    unsigned long mask, new_address;
    
    mask = ~(~((unsigned int) 0) << alignment);
    new_address = (address + mask) & ~mask;
    
    return new_address - address;

}

static void relax_section (struct section *section) {

    struct frag *root_frag;
    struct frag *frag;
    int changed;
    
    unsigned long address, frag_count, max_iterations;
    unsigned long alignment_needed;
    
    section_set (section);
    
    root_frag = current_frag_chain->first_frag;
    address   = 0;
    
    for (frag_count = 0, frag = root_frag; frag; frag_count++, frag = frag->next) {
    
        frag->relax_marker  = 0;
        frag->address       = address;
        
        address += frag->fixed_size;
        
        switch (frag->relax_type) {
        
            case RELAX_TYPE_NONE_NEEDED:
            
                break;
            
            case RELAX_TYPE_ALIGN:
            case RELAX_TYPE_ALIGN_CODE:
            
                alignment_needed = relax_align (address, frag->offset);
                
                if (frag->relax_subtype != 0 && alignment_needed > frag->relax_subtype) {
                    alignment_needed = 0;
                }
                
                address += alignment_needed;
                break;
            
            case RELAX_TYPE_ORG:
            case RELAX_TYPE_SPACE:
            
                break;
            
            case RELAX_TYPE_MACHINE_DEPENDENT:
            
                address += machine_dependent_estimate_size_before_relax (frag, section);
                break;
            
            default:
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "%s: %lu: invalid relax type", frag->filename, frag->line_number);
                exit (EXIT_FAILURE);
        
        }
    
    }
    
    /**
     * Prevents an infinite loop caused by frag growing because of a symbol that moves when the frag grows.
     *
     * Example:
     *
     *      .org _abc + 2
     *      _abc:
     */
    max_iterations = frag_count * frag_count;
    
    /* Too many frags might cause an overflow. */
    if (max_iterations < frag_count) {
        max_iterations = frag_count;
    }
    
    do {
    
        long change = 0;
        changed = 0;
        
        for (frag = root_frag; frag; frag = frag->next) {
        
            long growth = 0;
            unsigned long old_address;
            
            unsigned long old_offset;
            unsigned long new_offset;
            
            frag->relax_marker = !frag->relax_marker;
            
            old_address = frag->address;
            frag->address += change;
            
            switch (frag->relax_type) {
            
                case RELAX_TYPE_NONE_NEEDED:
                
                    growth = 0;
                    break;
                
                case RELAX_TYPE_ALIGN:
                case RELAX_TYPE_ALIGN_CODE:
                
                    old_offset = relax_align (old_address + frag->fixed_size, frag->offset);
                    new_offset = relax_align (frag->address + frag->fixed_size, frag->offset);
                    
                    if (frag->relax_subtype != 0) {
                    
                        if (old_offset > frag->relax_subtype) {
                            old_offset = 0;
                        }
                        
                        if (new_offset > frag->relax_subtype) {
                            new_offset = 0;
                        }
                    
                    }
                        
                    growth = new_offset - old_offset;
                    break;
                
                case RELAX_TYPE_ORG: {
                
                    struct symbol *symbol = state->end_symbol;
                    unsigned long target = frag->offset;
                    
                    if (frag->symbol) {
                        target += symbol_get_value (frag->symbol);
                    }
                    
                    if (symbol) {
                    
                        if (frag != (void*)symbol->frag) {
                        
                            frag->relax_type = RELAX_TYPE_NONE_NEEDED;
                            growth = 0;
                            
                            frag->next->address += target;
                            break;
                        
                        }
                    
                    }
                    
                    growth = target - (frag->next->address + change);
                    
                    if (frag->address + frag->fixed_size > target) {
                    
                        report_at (frag->filename, frag->line_number, REPORT_ERROR, "attempt to move .org backwards");
                        growth = 0;
                        
                        /* Changes the frag so no more errors appear because of it. */
                        frag->relax_type = RELAX_TYPE_ALIGN;
                        frag->offset = 0;
                        frag->fixed_size = frag->next->address + change - frag->address;
                    
                    }
                    
                    break;
                
                }
                
                case RELAX_TYPE_SPACE:
                
                    growth = 0;
                    
                    if (frag->symbol) {
                    
                        long amount = symbol_get_value (frag->symbol);
                        
                        if (symbol_get_section (frag->symbol) != absolute_section || symbol_is_undefined (frag->symbol)) {
                        
                            report_at (frag->filename, frag->line_number, REPORT_WARNING, ".space specifies non-absolute value");
                            
                            /* Prevents the error from repeating. */
                            frag->symbol = (void*)0;
                        
                        } else if (amount < 0) {
                        
                            report_at (frag->filename, frag->line_number, REPORT_WARNING, ".space with negative value, ignoring");
                            frag->symbol = (void*)0;
                        
                        } else {
                            growth = old_address + frag->fixed_size + amount - frag->next->address;
                        }
                    
                    }
                    
                    break;
                
                case RELAX_TYPE_MACHINE_DEPENDENT:
                
                    growth = machine_dependent_relax_frag (frag, section, change);
                    break;
                
                default:
                
                    report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "%s: %lu: invalid relax type", frag->filename, frag->line_number);
                    exit (EXIT_FAILURE);
            
            }
            
            if (growth) {
            
                change += growth;
                changed = 1;
            
            }
        
        }
    
    } while (changed && --max_iterations);
    
    if (changed) {
    
        report_at ((void*)0, 0, REPORT_FATAL_ERROR, "Infinite loop encountered whilst attempting to compute the addresses in section %s", section_get_name (section));
        exit (EXIT_FAILURE);
    
    }

}

static void finish_frags_after_relaxation (struct section *section) {

    struct frag *root_frag, *frag;
    
    section_set (section);
    root_frag = current_frag_chain->first_frag;
    
    for (frag = root_frag; frag; frag = frag->next) {
    
        switch (frag->relax_type) {
        
            case RELAX_TYPE_NONE_NEEDED:
            
                break;
            
            case RELAX_TYPE_ORG:
            case RELAX_TYPE_ALIGN:
            case RELAX_TYPE_ALIGN_CODE:
            case RELAX_TYPE_SPACE: {
            
                int64_t i;
                
                unsigned char *p;
                unsigned char fill;
                
                frag->offset = frag->next->address - (frag->address + frag->fixed_size);
                
                if (((long) (frag->offset)) < 0) {
                
                    report_at (frag->filename, frag->line_number, REPORT_ERROR, "attempt to .org/.space backward (%lu)", frag->offset);
                    frag->offset = 0;
                
                }
                
                p = finished_frag_increase_fixed_size_by_frag_offset (frag);
                fill = *p;
                
                for (i = 0; i < frag->offset; i++) {
                    p[i] = fill;
                }
                
                break;
            
            }
            
            case RELAX_TYPE_MACHINE_DEPENDENT:
            
                machine_dependent_finish_frag (frag);
                break;
            
            default:
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "invalid relax type");
                exit (EXIT_FAILURE);
        
        }
    
    }

}

static void adjust_reloc_symbols_of_section (struct section *section) {

    struct fixup *fixup;
    section_set (section);
    
    for (fixup = current_frag_chain->first_fixup; fixup; fixup = fixup->next) {
    
        if (fixup->done) {
            continue;
        }
        
        if (fixup->add_symbol) {
        
            struct symbol *symbol = fixup->add_symbol;
            struct section *symbol_section;
            
            /* Resolves symbols that have not been resolved yet (expression symbols). */
            symbol_resolve_value (symbol);
            
            if (fixup->sub_symbol) {
                symbol_resolve_value (fixup->sub_symbol);
            }
            
            if (symbol_uses_reloc_symbol (symbol)) {
            
                fixup->add_number += symbol_get_value_expression (symbol)->add_number;
                
                symbol = symbol_get_value_expression (symbol)->add_symbol;
                fixup->add_symbol = symbol;
            
            }
            
            if (symbol_force_reloc (symbol)) {
                continue;
            }
            
            symbol_section = symbol_get_section (symbol);
            
            if (symbol_section == absolute_section) {
                continue;
            }
            
            fixup->add_number += symbol_get_value (symbol);
            fixup->add_symbol  = section_symbol (symbol_get_section (symbol));
        
        }
    
    }

}

#include    "inttypes.h"
int64_t machine_dependent_pcrel_from (struct fixup *fixup);

int machine_dependent_force_relocation_local (struct fixup *fixup);
void machine_dependent_apply_fixup (struct fixup *fixup, uint64_t value);

static unsigned long fixup_section (struct section *section) {

    unsigned long section_reloc_count = 0;
    uint64_t add_number;
    
    struct section *add_symbol_section = absolute_section;
    struct fixup *fixup;
    
    section_set (section);
    
    for (fixup = current_frag_chain->first_fixup; fixup; fixup = fixup->next) {
    
        add_number = fixup->add_number;
        
        if (fixup->add_symbol) {
            add_symbol_section = symbol_get_section (fixup->add_symbol);
        }
        
        if (fixup->sub_symbol) {
        
            struct section *sub_symbol_section;
            symbol_resolve_value (fixup->sub_symbol);
            
            sub_symbol_section = symbol_get_section (fixup->sub_symbol);
            
            if (fixup->add_symbol && add_symbol_section == sub_symbol_section && !symbol_force_reloc (fixup->add_symbol) && !symbol_force_reloc (fixup->add_symbol)) {
            
                add_number += symbol_get_value (fixup->add_symbol);
                add_number -= symbol_get_value (fixup->sub_symbol);
                
                fixup->add_number = add_number;
                fixup->add_symbol = (void*)0;
                fixup->sub_symbol = (void*)0;
            
            } else if (sub_symbol_section == section) {
            
                add_number -= symbol_get_value (fixup->sub_symbol);
                
                if (!fixup->pcrel) {
                    add_number += machine_dependent_pcrel_from (fixup);
                }
                
                fixup->sub_symbol = (void*)0;
                fixup->pcrel = 1;
            
            } else {
            
                report_at (__FILE__, __LINE__, REPORT_INTERNAL_ERROR, "+++fixup_section sub_symbol");
                exit (EXIT_FAILURE);
            
            }
        
        }
        
        if (fixup->add_symbol) {
        
            if ((add_symbol_section == section) && !machine_dependent_force_relocation_local (fixup)) {
            
                add_number += symbol_get_value (fixup->add_symbol);
                fixup->add_number = add_number;
                
                if (fixup->pcrel) {
                
                    add_number -= machine_dependent_pcrel_from (fixup);
                    fixup->pcrel = 0;
                
                }
                
                fixup->add_symbol = (void*)0;
            
            } else if (add_symbol_section == absolute_section || (fixup->reloc_type == RELOC_TYPE_FAR_CALL && !symbol_is_undefined (fixup->add_symbol))) {
            
                add_number += symbol_get_value (fixup->add_symbol);
                
                fixup->add_number = add_number;
                fixup->add_symbol = (void*)0;
            
            }
        
        }
        
        if (fixup->pcrel) {
        
            add_number -= machine_dependent_pcrel_from (fixup);
            
            if (!fixup->add_symbol && !fixup->done) {
                fixup->add_symbol = section_symbol (absolute_section);
            }
        
        }
        
        machine_dependent_apply_fixup (fixup, add_number);
        
        if (!fixup->done) {
            section_reloc_count++;
        }
        
        if (fixup->size < sizeof (uint64_t)) {
        
            uint64_t mask = -1;
            mask <<= fixup->size * 8 - !!fixup->fixup_signed;
            
            if ((add_number & mask) && (fixup->fixup_signed ? ((add_number & mask) != mask) : (-add_number & mask))) {
            
                report_at ((void*)0, 0, REPORT_ERROR, (add_number > 1000) ?
                    "value of %#"PRIx64" too large for field of %u byte%s at %#lx" : "value of %"PRIu64" too large for field of %u byte%s at %#lx",
                        add_number, fixup->size, ((fixup->size == 1) ? "" : "s"), fixup->frag->address + fixup->where);
            
            }
        
        }
    
    }
    
    return section_reloc_count;

}

void fixup_code (void) {

    struct section *section;
    struct symbol *symbol;
    
    for (section = sections; section; section = section_get_next_section (section)) {
        relax_section (section);
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
        finish_frags_after_relaxation (section);
    }
    
    if (state->format == AS_OUTPUT_I386_ELKS || state->format == AS_OUTPUT_IA16_ELKS) {
    
        unsigned long address, text_section_size;
        struct frag *frag;
        
        section_set (text_section);
        
        text_section_size = current_frag_chain->last_frag->address + current_frag_chain->last_frag->fixed_size;
        address = text_section_size;
        
        section_set (data_section);
        
        for (frag = current_frag_chain->first_frag; frag; frag = frag->next) {
        
            frag->address = address;
            address += frag->fixed_size;
        
        }
        
        section_set (bss_section);
        
        for (frag = current_frag_chain->first_frag; frag; frag = frag->next) {
        
            frag->address = address;
            address += frag->fixed_size;
        
        }
    
    }
    
    finalize_symbols = 1;
    
    for (symbol = symbols; symbol; symbol = symbol->next) {
        symbol_resolve_value (symbol);
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
        adjust_reloc_symbols_of_section (section);
    }
    
    for (section = sections; section; section = section_get_next_section (section)) {
        fixup_section (section);
    }

}
