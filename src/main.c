/*
 *	NMH's Simple C Compiler, 2011--2014
 *	Main program
 */

#include "defs.h"
#define extern_
 #include "data.h"
#undef extern_
#include "decl.h"

static void cmderror(char *s, char *a) {
	fprintf(stderr, "scc: ");
	fprintf(stderr, s, a);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void cleanup(void) {
	if (!O_testonly && NULL != Basefile) {
		remove(newfilename(Basefile, 's'));
		remove(newfilename(Basefile, 'o'));
	}
}

char *newfilename(char *file, int sfx) {
	char	*ofile;

	if ((ofile = strdup(file)) == NULL)
		cmderror("too many file names", NULL);
	ofile[strlen(ofile)-1] = sfx;
	return ofile;
}

static int filetype(char *file) {
	int	k;

	k = strlen(file);
	if ('.' == file[k-2]) return file[k-1];
	return 0;
}

static int exists(char *file) {
	FILE	*f;

	if ((f = fopen(file, "r")) == NULL) return 0;
	fclose(f);
	return 1;
}

#ifdef __dos

static void compile(char *file, char *def) {
	char	*ofile;
	char	scc_cmd[129];

	if (!file)
		cmderror("pipe mode not supported in DOS version", NULL);
	if (O_testonly)
		cmderror("test mode not supported in DOS version", NULL);
	ofile = newfilename(file, 's');
	if (O_verbose) {
		if (O_verbose > 1)
			printf("sccmain.exe %s %s\n", file, ofile);
		else
			printf("compiling %s\n", file);
	}
	if (strlen(file)*2 + (def? strlen(def): 0) + 16 > 128)
		cmderror("file name too long", file);
	sprintf(scc_cmd, "sccmain.exe %s %s %s", file, ofile, def? def: "");
	if (system(scc_cmd))
		cmderror("compiler invokation failed", NULL);
}

#else /* !__dos */

static void compile(char *file, char *def) {
	char	*ofile;
	FILE	*in, *out, *tmp;

	in = stdin;
	out = stdout;
	tmp = tmpfile();
	if (tmp == NULL) {
		cmderror("cannot create tmpfile", NULL);
	}
	if (file) {
		ofile = newfilename(file, 's');
		if ((in = fopen(file, "r")) == NULL)
			cmderror("no such file: %s", file);
		if (!O_testonly) {
			if ((out = fopen(ofile, "r")) != NULL)
				cmderror("will not overwrite file: %s",
					ofile);
			if ((out = fopen(ofile, "w")) == NULL)
				cmderror("cannot create file: %s", ofile);
		}
	}
	if (O_testonly) out = NULL;
	if (O_verbose) {
		if (O_testonly)
			printf("cc -t %s\n", file);
		else
			if (O_verbose > 1)
				printf("cc -S -o %s %s\n", ofile, file);
			else
				printf("compiling %s\n", file);
	}
	program(file, in, out, tmp, def);
	if (file) {
		fclose(in);
		if (out) fclose(out);
	}
	fclose(tmp);
}

#endif /* !__dos */

static void preprocess(char *file, char *def) {
	char	*ofile;
	FILE	*in, *out, *tmp;

	in = stdin;
	out = stdout;
	tmp = tmpfile();
	if (tmp == NULL) {
		cmderror("cannot create tmpfile", NULL);
	}
	if (file) {
		ofile = newfilename(file, 'i');
		if ((in = fopen(file, "r")) == NULL)
			cmderror("no such file: %s", file);
		if ((out = fopen(ofile, "w")) == NULL)
			cmderror("cannot create file: %s", ofile);
	}
	printf("preprocessing %s\n", file);
	program(file, in, out, tmp, def);
	if (file) {
		fclose(in);
		if (out) fclose(out);
	}
	fclose(tmp);
}


static void collect(char *file, int temp) {
	if (O_componly || O_asmonly || O_preponly) return;
	if (Nf >= MAXFILES)
		cmderror("too many input files", NULL);
	Temp[Nf] = temp;
	Files[Nf++] = file;
}

static void assemble(char *file, int delete) {
	char	*ofile;
	char	cmd[TEXTLEN+1];

	file = newfilename(file, 's');
	if (O_componly && O_outfile)
		ofile = O_outfile;
	else
		collect(ofile = newfilename(file, 'o'), 1);
	if (strlen(file) + strlen(ofile) + strlen(ASCMD) >= TEXTLEN)
		cmderror("assembler command too long", NULL);
	sprintf(cmd, ASCMD, ofile, file);
	if (O_verbose > 1) printf("%s\n", cmd);
	if (system(cmd))
		cmderror("assembler invocation failed", NULL);
	if (delete) {
		if (O_verbose > 2) printf("rm %s\n", file);
		remove(file);
	}
}

static int concat(int k, char *buf, char *s) {
	int	n;

	n = strlen(s);
	if (k + n + 2 >= TEXTLEN)
		cmderror("linker command too long", buf);
	strcat(buf, " ");
	strcat(buf, s);
	return k + n + 1;
}

static void link(void) {
	int	i, k;
	char	cmd[TEXTLEN+2];
	char	cmd2[TEXTLEN+2];
	char	*ofile;

	ofile = O_outfile? O_outfile: AOUTNAME;
	if (strlen(ofile) + strlen(LDCMD) + strlen(SCCDIR)*2 >= TEXTLEN)
		cmderror("linker command too long", NULL);
	sprintf(cmd, LDCMD, ofile, SCCDIR, O_stdio? "": "n");
	k = strlen(cmd);
	for (i=0; i<Nf; i++)
		k = concat(k, cmd, Files[i]);
	k = concat(k, cmd, SCCLIBC);
	concat(k, cmd, SYSLIBC);
	sprintf(cmd2, cmd, SCCDIR);
	if (O_verbose > 1) printf("%s\n", cmd2);
	if (system(cmd2))
		cmderror("linker invocation failed", NULL);
	if (O_verbose > 2) printf("rm ");
	for (i=0; i<Nf; i++) {
		if (Temp[i]) {
			if (O_verbose > 2) printf(" %s", Files[i]);
			remove(Files[i]);
		}
	}
	if (O_verbose > 2) printf("\n");
}

static void usage(void) {
	printf("Usage: scc [-h] [-ctvNSVE] [-I dir] [-d opt] "
			"[-o file] [-D macro[=text]] file [...]\n");
}

static void longusage(void) {
	printf("\n");
	usage();
	printf(	"\n"
		"-c       compile only, do not link\n"
		"-d opt   activate debug option OPT, ? = list\n"
		"-o file  write linker output to FILE\n"
		"-t       test only, generate no code\n"
		"-v       verbose, more v's = more verbose\n"
		"-I       additional include directory\n"
		"-D m=v   define macro M with optional value V\n"
		"-N       do not use stdio (can't use printf, etc)\n"
		"-S       compile to assembly language\n"
		"-E       preprocess only\n"
		"-V       print version and exit\n"
		"\n" );
}

static void version(void) {
	printf("SubC version %s for %s/%s\n", VERSION, OS, CPU);
}

static char *nextarg(int argc, char *argv[], int *pi, int *pj) {
	char	*s;

	if (argv[*pi][*pj+1] || *pi >= argc-1) {
		usage();
		exit(EXIT_FAILURE);
	}
	s = argv[++*pi];
	*pj = strlen(s)-1;
	return s;
}

static int dbgopt(int argc, char *argv[], int *pi, int *pj) {
	char	*s;

	s = nextarg(argc, argv, pi, pj);
	if (!strcmp(s, "lsym")) return D_LSYM;
	if (!strcmp(s, "gsym")) return D_GSYM;
	if (!strcmp(s, "stat")) return D_STAT;
	if (!strcmp(s, "defi")) return D_DEFI;
	printf(	"\n"
		"scc: valid -d options are: \n\n"
		"lsym - dump local symbol tables\n"
		"gsym - dump global symbol table\n"
		"stat - print usage statistics\n"
		"defi - print usage declaration definition\n"
		"\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int	i, j;
	char	def[TEXTLEN+2];
	char	*d;
	char	*in;
	char 	*a;

	def[0] = '\0';
	def[1] = '\0';
	d = def;
	Includes[0] = '\0';
	Includes[1] = '\0';
	in = Includes;

	O_debug = 0;
	O_verbose = 0;
	O_componly = 0;
	O_preponly = 0;
	O_asmonly = 0;
	O_testonly = 0;
	O_stdio = 1;
	O_outfile = NULL;
	for (i=1; i<argc; i++) {
		if (*argv[i] != '-') break;
		if (!strcmp(argv[i], "-")) {
			compile(NULL, def);
			exit(Errors? EXIT_FAILURE: EXIT_SUCCESS);
		}
		for (j=1; argv[i][j]; j++) {
			switch (argv[i][j]) {
			case 'c':
				O_componly = 1;
				break;
			case 'd':
				O_debug |= dbgopt(argc, argv, &i, &j);
				O_testonly = 1;
				break;
			case 'h':
				longusage();
				exit(EXIT_SUCCESS);
			case 'o':
				O_outfile = nextarg(argc, argv, &i, &j);
				break;
			case 't':
				O_testonly = 1;
				break;
			case 'v':
				O_verbose++;
				break;
			case 'D':
				if (argv[i][j+1]) {
					a = &argv[i][j+1];
					j = strlen(argv[i])-1;
				} else {
					a = nextarg(argc, argv, &i, &j);
				}
				if (d - def + strlen(a) >= TEXTLEN) {
					cmderror("too many -D's", NULL);
				}
				strcat(d, a);
				d += strlen(a) + 1;
				*(d) = '\0';
				break;
			case 'I':
				if (argv[i][j+1]) {
					a = &argv[i][j+1];
					j = strlen(argv[i])-1;
				} else {
					a = nextarg(argc, argv, &i, &j);
				}
				if (in - Includes  + strlen(a) >= TEXTLEN) {
					cmderror("too many -I's", NULL);
				}
				strcat(in, a);
				in += strlen(a) + 1;
				*(in) = '\0';
				break;
			case 'N':
				O_stdio = 0;
				break;
			case 'E':
				O_preponly = 1;
				break;
			case 'S':
				O_componly = O_asmonly = 1;
				break;
			case 'V':
				version();
				exit(EXIT_SUCCESS);
			default:
				usage();
				exit(EXIT_FAILURE);
			}
		}
	}
	if (i >= argc) {
		usage();
		exit(EXIT_FAILURE);
	}
	Nf = 0;
	while (i < argc) {
		if (filetype(argv[i]) == 'c') {
			if (O_preponly) {
				preprocess(argv[i], def);
			} else {
				compile(argv[i], def);
			}
			if (Errors && !O_testonly)
				cmderror("compilation stopped", NULL);
			if (!O_asmonly && !O_testonly && !O_preponly)
				assemble(argv[i], 1);
			i++;
		}
		else if (filetype(argv[i]) == 's') {
			if (!O_testonly) assemble(argv[i++], 0);
		}
		else {
			if (!exists(argv[i])) cmderror("no such file: %s",
							argv[i]);
			collect(argv[i++], 0);
		}
	}
	if (!O_componly && !O_testonly && !O_preponly) link();
	return EXIT_SUCCESS;
}
