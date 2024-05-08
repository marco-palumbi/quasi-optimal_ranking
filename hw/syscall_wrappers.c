/* Implement some system calls to shut up the linker warnings */

#include <errno.h>
#include <sys/times.h>
#undef errno
extern int errno;

int _open(char *file, int flags, int mode)
{
  (void) file;
  (void) flags;
  (void) mode;
  errno = ENOSYS;
  return -1;
}

int _close(int fd)
{
  errno = ENOSYS;
	(void) fd;
	return -1;
}

#include <sys/stat.h>

int _fstat(int fd, struct stat* buf)
{
  (void) fd;
  (void) buf;
  errno = ENOSYS;
	return -1;
}

int _getpid(void)
{
  errno = ENOSYS;
	return -1;
}

int _isatty(int file)
{
  (void) file;
  errno = ENOSYS;
  return 0;
}

int _kill(int pid, int sig)
{
  (void) pid;
  (void) sig;
  errno = ENOSYS;
	return -1;
}

int _lseek(int fd, int ptr, int dir)
{
  (void) fd;
  (void) ptr;
  (void) dir;
  errno = ENOSYS;
	return -1;
}

int _read(int fd, char* ptr, int len)
{
  (void) fd;
  (void) ptr;
  (void) len;
  errno = ENOSYS;
	return -1;
}

clock_t _times(struct tms *buf)
{
  (void)buf;
  errno = ENOSYS;
    return -1;
}