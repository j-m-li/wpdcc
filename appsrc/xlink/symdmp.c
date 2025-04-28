/*******************************/
/*                             */
/* Program to produce formated */
/*  dump of XOS SYM files      */
/*                             */
/*******************************/

#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <CTYPE.H>
#include <ERRNO.H>
#include <XOS.H>
#include <XOSERRMSG.H>
#include <XOSSTR.H>

#define VERSION 1
#define EDIT    0


#define SFLG_SELECT 0x80	// Symbol is a selector value
#define SFLG_MULT   0x40	// Symbol is multiply defined
#define SFLG_ADDR   0x20	// Symbol is an address
#define SFLG_SUPRSD 0x10	// Symbol is suppressed
#define SFLG_GLOBAL 0x08	// Symbol is global
#define SFLG_MSECT  0x04	// Symbol is a msect selector/offset value
#define SFLG_MODULE 0x02	// Symbol is a module name
#define SFLG_RELOC  0x01	// Symbol value is relocatable


FILE    *fp;
long     value;
long     reloc;
uchar    flags;
char     filespec[200];
char     name[128];
char     prgname[] = "SYMDMP";

typedef struct
{	long id;
	long length;
	long number;
} FHEAD;

FHEAD fhead;

int   getbyte(void);
ulong getlong(void);
int   getword(void);

void main(
    int   argc,
    char *argv[])
{
    char *pnt;
	char *prefix;
	long  number;
	int   cnt;
	char  ftext[16];
	char  relstr[8];
    char  chr;
	uchar type;

    fprintf(stderr, "SYMDMP - version %d.%d\n", VERSION, EDIT);
    if (argc != 2)
    {
        fputs("? SYMDMP: Command error, correct usage is:\n"
              "              RUNDMP filename\n", stderr);
        exit(1);
    }
    pnt = argv[1];
    if (strlen(pnt) > (sizeof(filespec) - 6))
    {
        fputs("? SYMDMP: File specification is too long\n", stderr);
        exit(1);
    }
    prefix = filespec;
    type = FALSE;
    while ((chr = *pnt++) != 0)
    {
        if (chr == '.')
            type = TRUE;
        *prefix++ = chr;
    }
    if (!type)
        strmov(prefix, ".SYM");
    fp = fopen(filespec, "rb");
    if (fp == NULL)
        femsg(prgname, -errno, filespec);

	// Read the file header

	if (fread((char *)&fhead, 1, sizeof(fhead), fp) < 0)
	{
        if (errno == 0)
        {
            printf("\n? RUNDMP: Unexpected end of file\n");
            exit(1);
        }
        else
            femsg(prgname, -errno, filespec);
    }
	if (fhead.id != 0x40222D4)
	{
		fputs("? SYMDMP: File header is incorrect", stderr);
		exit(1);
	}
	printf("%d bytes in file\n%d symbol%s in file\n", fhead.length,
			fhead.number, (fhead.number == 1) ? "" : "s");
	number = 0;
	while (number < fhead.number)
	{
		flags = getbyte();
		value = getlong();
		reloc = getword();
		pnt = name;
		cnt = 122;
		do
		{
			chr = getbyte();
			*pnt++ = chr & 0x7F;
		} while (--cnt > 0 && (chr & 0x80) == 0);
		if (cnt <= 0)
			*(long *)pnt = '...';
		else
			*pnt = 0;
		pnt = ftext;
		if (flags & 0x02)
			pnt = strmov(pnt, "Mod  ");
		else
		{
			*pnt++ = (flags & SFLG_ADDR) ? 'A' : (flags & SFLG_SELECT) ?
					'S' : (flags & SFLG_MSECT) ? 'M' : '.';
			*pnt++ = (flags & SFLG_MULT) ? '*' : '.';
			*pnt++ = (flags & SFLG_GLOBAL) ? '#' : '.';
			*pnt++ = (flags & SFLG_SUPRSD) ? '!' : '.';
			*pnt++ = (flags & SFLG_RELOC) ? 'R' : '.';
			*pnt = 0;
		}
		if (flags & SFLG_RELOC)
			sprintf(relstr, "%04X", reloc);
		else
			strcpy(relstr, "    ");
		printf("%6d %s %08X %s %s\n", number++, relstr, value, ftext, name);
	}
}


//*******************************************
// Function: getlong - Get long from rel file
// Returned: Value oflong
//*******************************************

ulong getlong(void)

{
    ulong word1;
    ulong word2;

    word1 = getword();
    word2 = getword();
    return ((word2<<16) + word1);
}

/*
 * Function to get word for rel file
 */

int getword(void)

{
    int byte1;
    int byte2;

    byte1 = getbyte();
    byte2 = getbyte();
    return ((byte2<<8) + byte1);
}


//*******************************************
// Function: getbyte - Get byte from rel file
// Returned: Value of byte
//*******************************************

int getbyte(void)

{
    int value;

    value = getc(fp);
    if (value < 0)
    {
        if (errno == 0)
        {
            printf("\n? RUNDMP: Unexpected end of file\n");
            exit(1);
        }
        else
            femsg(prgname, -errno, filespec);
    }
    return (value);
}
