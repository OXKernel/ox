/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 *
 * <sys/unistd.h>
 *    UNIX STD system call interface
 *
 ********************************************************/
#ifndef _SYS_UNISTD_H
#define _SYS_UNISTD_H  1   
#ifdef __cplusplus
 extern "C" {
#endif
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utime.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/signal.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <dirent.h>

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef         long ssize_t;
#endif

/* POSIX Table 2-8 */
#define F_OK          0   /* access, file exists */
#define X_OK          1   /* access, file executable */
#define W_OK          2   /* access, file writable */
#define R_OK          4   /* access, file readable */

/* POSIX Table 2-9 */
#define SEEK_SET      0 
#define SEEK_CUR      1  /* offset, relative to current */
#define SEEK_END      2  /* offset, relative to end */

/* POSIX Table 2-10 */
#define _POSIX_VERSION_199009L 

/* POSIX section 8.2.1.2 */
#define STDIN_FILENO  0 
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* POSIX section 2.7.1.1 */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* POSIX Table 4-2 
 * system configurable
 */
#define _SC_ARG_MAX           1
#define _SC_CHILD_MAX         2
#define _SC_CLOCKS_PER_SEC    3
#define _SC_CLK_TCK           3
#define _SC_NGROUPS_MAX       4  
#define _SC_OPEN_MAX          5
#define _SC_JOB_CONTROL       6
#define _SC_SAVED_IDS         7
#define _SC_VERSION           8
#define _SC_STREAM_MAX        9
#define _SC_TZNAME_MAX        10

/* POSIX Table 5-2
 * configurable pathname variables
 */
#define _PC_LINK_MAX           1  /* max links */
#define _PC_MAX_CANON          2  /* max cannonicial queue */
#define _PC_MAX_INPUT          3  /* max type-ahead buffer */
#define _PC_NAME_MAX           4  /* max filename size */
#define _PC_PATH_MAX           5  /* max pathname */
#define _PC_PIPE_BUF           6  /* max pipe */
#define _PC_NO_TRUNC           7  /* truncate long name */
#define _PC_VDISABLE           8  /* disable tty */ 
#define _PC_CHOWN_RESTRICTED   9  /* if chown is restricted */

/* POSIX Features Implemented
 *
 * #define _POSIX_JOB_CONTROL
 * #define _POSIX_SAVED_IDS
 * #define _POSIX_NO_TRUNC
 * #define _POSIX_CHOWN_RESTRICTED
 * #define _VDISABLE
 */

/* POSIX - UNIX System Calls
 */
int          access (const char *path, mode_t mode);
int          acct   (const char *file);
unsigned int alarm  (unsigned int seconds);

int          brk    (void *end_data_segment);
void *       sbrk   (ptrdiff_t increment);

int          chdir  (const char *path);
int          chmod  (const char *path,mode_t mode);
int          chown  (const char *path,uid_t owner,gid_t group);
int         fchown  (int fd, uid_t owner, gid_t group);
int         lchown  (const char *path,uid_t owner,gid_t group);
int          chroot (const char *path); 
int          creat  (const char *path,mode_t mode);
int          close  (int fd);

int          dup    (int fd);
int          dup2   (int fd1,int fd2);

extern char **environ;
int          execl  (const char *path, const char *arg, ...);
int          execle (const char *file, const char *arg, ...);
int          execlp (const char *path, const char *arg, ...);
int          execv  (const char *file, char *const argv[], ...);
int          execve (const char *path, const char **argv,const char **envp);
int          execvp (const char *file, char *const argv[]);

volatile void exit(int status);

int          fcntl  (int fd,int request, ...);
pid_t        fork   (void);

pid_t        getpid (void);
gid_t        getegid(void);
gid_t        getgid (void);
uid_t        geteuid(void);
uid_t        getuid (void);
pid_t        getpgrp(void);
pid_t        getppid(void);
pid_t        getpgid(pid_t pid);
char *       getcwd (char *buf, size_t size);

int          ioctl  (int fd, int request, ...);

int          kill   (pid_t pid, int sig);

int          link   (const char *oldpath, const char *newpath);
off_t        lseek  (int fd, off_t offset, int whence);

int          mkdir  (const char *path, mode_t mode);
int          mknod  (const char *file, mode_t mode, dev_t dev);
int          mount  (const char *specialfile, const char * dir, int rwflag);

int          nice   (int val);

int          open   (const char *path, int flag, ...);

int          pause  (void);
int          pipe   (int filedes[2]);
int          ptrace (int request,pid_t pid,long addr,long data);

ssize_t      read   (int fd, void *buf,size_t count);
int          raise  (int sig);
int          rmdir  (const char *path);

int          setpgrp(void);
int          setpgid(pid_t pid, pid_t pgid);
int          setgid (gid_t gid);
int          setuid (uid_t uid);
pid_t        setsid (void);
int          setreuid(uid_t ruid, uid_t euid);
int          seteuid(uid_t euid);
int          setregid(gid_t rgid, gid_t egid);
int          setegid(gid_t egid);

int          sleep  (unsigned int seconds);
int          nanosleep(const struct timespec *req, struct timespec *rem);

//void         (*signal(int signum, void (*handler)(int)))(int);
int signal(int signum, sighandler_t handler);
int          sigaction   (int signum,  const  struct  sigaction  *act, struct sigaction *oldact);
int          sigprocmask (int  how,  const  sigset_t *set, sigset_t *oldset);
int          sigpending  (sigset_t *set);
int          sigsuspend  (const sigset_t *mask);
int          sigreturn   (unsigned long __unused);
int          sigemptyset(sigset_t *set);
int          sigfillset(sigset_t *set);
int          sigaddset(sigset_t *set, int signum);
int          sigdelset(sigset_t *set, int signum);
int          sigismember(const sigset_t *set, int signum);

int          stat    (const char *file, struct stat *stbuf);
int          fstat   (int fd, struct stat *stbuf);

int          sync    (void);

time_t       time    (time_t *t);
clock_t      times   (struct tms *buf);
int          ftime   (struct timeb *tp);
int          stime   (time_t *t);
int          utime   (const char *file, struct utimbuf *buf);
int          utimes  (char *file, struct timeval *tvp);

mode_t       umask   (mode_t mask);
int          umount  (const char *specialfile);
int          uname   (struct utsname *name);
int          unlink  (const char *path);
int          ustat   (dev_t dev, struct ustat *ubuf);

pid_t        waitpid (pid_t pid, WAIT_STATUS wait_stat,int options);
pid_t        wait    (WAIT_STATUS wait_stat);
ssize_t      write   (int fd,void *buf,size_t count);

int          reboot  (int magic, int magic_too, int flag);

// These calls were added recently...
int         lstat   (const char *path, struct stat *buf);
int         fchmod  (int fd, mode_t mode);
char        *getwd  (const char *buf);
char        *get_current_dir_name(void);
int         fchdir  (int fd);
int         rename  (const char *oldpath, const char *newpath);
DIR         *opendir(const char *path);
int         closedir(DIR *dir);
void        rewinddir(DIR *dir);
struct dirent *readdir(DIR *dir);
time_t      mktime(struct tm *tm);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_UNISTD_H */
