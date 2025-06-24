
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

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
struct linux_dirent {
	char *name;
};

struct kstat {
	int s;
};

int main(int,char*[]);

void _start()
{
	main(0,0);
}

int setjmp(jmp_buf env){}
void longjmp(jmp_buf env, int v){}
void _exit(int rc){}
int _sbrk(int size){}
int _write(int fd, void *buf, int len){}
int _read(int fd, void *buf, int len){}
int _lseek(int fd, int pos, int how){}
int _creat(char *path, int mode){}
int _open(char *path, int flags){}
int _close(int fd){}
int _truncate(char *path, int size){}
int _getdents(int fd, struct linux_dirent *dirent, int count){}
int _strlen(char *buf)
{
}
int _stat(char *path, struct kstat *buf){}
int _access(char *path, int mode){}
int _mkdir(char *path, int mode){}
int _rmdir(char *path){}
int _unlink(char *path){}
int _rename(char *old, char *new){}
int _fork(void){}
int _wait(int *rc){}
int _execve(char *path, char *argv[], char *envp[]){}
int _time(void){}
int raise(int sig){}
int signal(int sig, int (*fn)()){}

