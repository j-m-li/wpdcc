/*
 *	NMH's Simple C Compiler, 2013,2014
 *	Linux/386 environment
 */

#define OS		"Linux"
#define ASCMD		"as --32 -g -o %s %s"
#define LDCMD		"ld -m elf_i386 -o %s %s/lib/%scrt0.o"
#define SYSLIBC		""
#define DEFINES         "\0" /*"__SUBC__\0"*/
