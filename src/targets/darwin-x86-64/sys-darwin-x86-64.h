/*
 *	NMH's Simple C Compiler, 2013,2014
 *	Darwin/x86-64 environment
 */

#define OS		"Darwin"
#define ASCMD		"/usr/bin/as -arch x86_64 -mmacos-version-min=10.15 -g -o %s %s"
#define LDCMD		"/usr/bin/ld -no_pie  -Z -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -arch x86_64 -macos_version_min 10.15 -o %s %s/lib/%scrt0.o"
#define SYSLIBC		"-lc"
#define DEFINES "\0"

