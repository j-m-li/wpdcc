
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

int _get_arg(int index, void *buf, int len);
void _exit(int rc);

#define IMPORT(f) __attribute__((import_module("env"), import_name(f))) 

int main(int,char*[]);
void setup(void);
void loop(void);

int     errno = EOK;

FILE    *_files[FOPEN_MAX];
DIR     *_dirs[OPENDIR_MAX];


FILE *stdin = 0;
FILE *stdout = 0;
FILE *stderr = 0;
char **environ = 0;

struct jmp_buf_ {
	int eax;
};
#define jmp_buf struct jmp_buf_

void _event(int *ev) {
	static int state = 0;
	char buf[80];
	int l;
	if (state == 0) {
		state = 1;
		setup();
	}
	fprintf(stderr, "event typ:%d, data_Len:%d dataptr:%x\n", ev[3], ev[4], ev[5]);
	_write(1, ev[5], ev[4]);
	l = _get_arg(1, buf, sizeof(buf));
	if (l > 0) {
		_write(1, buf, l);
		_write(1, "\n", 1);
		loop();
		_exit(1);
		return;
	}
	loop();
}


void *_init(void) {
	static int _ev[6 + 256 / 4];
        int     i;
        for (i=0; i<FOPEN_MAX; i++)
                _files[i] = NULL;
        for (i=0; i<OPENDIR_MAX; i++)
                _dirs[i] = NULL;
        stdin = fdopen(0, "r");
        stdout = fdopen(1, "w");
        stderr = fdopen(2, "w");
        stdin->mode = _IOLBF;
        stdout->mode = _IOLBF;
        stderr->mode = _IOLBF;

	return _ev;
}

IMPORT("_get_arg") int _get_arg(int index, void *buf, int len);
IMPORT("_system") int _system(char *cmd);
IMPORT("_exit") void _exit(int rc);
IMPORT("_write") int _write(int fd, void *buf, int len);
IMPORT("_read") int _read(int fd, void *buf, int len);
IMPORT("_lseek") int _lseek(int fd, int pos, int how);
IMPORT("_creat") int _creat(char *path, int mode);
IMPORT("_open") int _open(char *path, int flags);
IMPORT("_close") int _close(int fd);
IMPORT("_truncate") int _truncate(char *path, int size);
IMPORT("_getdents") int _getdents(int fd, struct dirent *dirent, int count);
IMPORT("_access") int _access(char *path, int mode);
IMPORT("_stat") int _stat(char *path, struct stat *buf);
IMPORT("_mkdir") int _mkdir(char *path, int mode);
IMPORT("_rmdir") int _rmdir(char *path);
IMPORT("_unlink") int _unlink(char *path);
IMPORT("_rename") int _rename(char *old, char *new);
IMPORT("_time") int _time(void);

void _start()
{

}


int _sbrk(int size) {
	static int old = 0;
	int r = old;
	size += 0xFFFF + old;
	size &= ~0xFFFF;
	if (size > 0) {
		__builtin_wasm_memory_grow(0, size >> 16);
	}
	old = __builtin_wasm_memory_size(0) * 0x10000;
	return r;
}


int _strlen(char *buf)
{
	int i = 0;
	while (buf[i]) i++;
	return i;
}

int setjmp(jmp_buf env) {
	fprintf(stderr, "unsupported setjmp function");
}
void longjmp(jmp_buf env, int v) {
	fprintf(stderr, "unsupported longjmp function");
}
int _fork(void) {
	fprintf(stderr, "unsupported fork function");
}
int _wait(int *rc) {
	fprintf(stderr, "unsupported wait function");
}
int _execve(char *path, char *argv[], char *envp[]) {
	fprintf(stderr, "unsupported execve function");
}
int raise(int sig) {
	fprintf(stderr, "unsupported raise function");
}
int signal(int sig, int (*fn)()) {
	fprintf(stderr, "unsupported signal function");
}

