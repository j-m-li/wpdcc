
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

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
	if (state == 0) {
		state = 1;
		setup();
	}
	printf("event typ:%d, data_Len:%d dataptr:%x\n", ev[3], ev[4], ev[5]);
	_write(0, ev[5], ev[4]);
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
	printf("unsupported");
}
void longjmp(jmp_buf env, int v) {
	printf("unsupported");
}
int _fork(void) {
	printf("unsupported");
}
int _wait(int *rc) {
	printf("unsupported");
}
int _execve(char *path, char *argv[], char *envp[]) {
	printf("unsupported");
}
int raise(int sig) {
	printf("unsupported");
}
int signal(int sig, int (*fn)()) {
	printf("unsupported");
}

