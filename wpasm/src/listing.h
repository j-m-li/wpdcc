/******************************************************************************
 * @file            listing.h
 *****************************************************************************/
#ifndef     _LISTING_H
#define     _LISTING_H

#include    "frag.h"

void add_listing_line (char *real_line, unsigned long real_line_len, const char *filename, unsigned long line_number);
void add_listing_message (char *message, const char *filename, unsigned long line_number);

void generate_listing (void);
void update_listing_line (struct frag *frag);

#endif      /* _LISTING_H */
