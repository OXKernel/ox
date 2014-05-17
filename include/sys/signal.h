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
 * <sys/signal.h>
 *   signals, and signal codes
 *
 *******************************************************/
#ifndef _SYS_SIGNAL_H
#define _SYS_SIGNAL_H  1
#ifdef __cplusplus
 extern "C" {
#endif

typedef int sigatomic_t;

#ifdef  _POSIX_SOURCE
#ifndef _SIGSET_T
#define _SIGSET_T
typedef unsigned long sigset_t;
#endif
#endif

#define SIGHUP      1 /* hangup */
#define SIGINT      2 /* interrupt */
#define SIGQUIT     3 /* quit */
#define SIGILL      4 /* illegal instruction */
#define SIGTRAP     5 /* trace trap */
#define SIGABRT     6 /* IOT */
#define SIGIOT      6 /* IOT */
#define SIGUNUSED   7 /* unused */
#define SIGFPE      8 /* floating point */
#define SIGKILL     9 /* kill, cannot be handled */
#define SIGUSR1    10 /* user defined */
#define SIGSEGV    11 /* segmentation fault */
#define SIGUSR2    12 /* user defined */
#define SIGPIPE    13 /* broken pipe */
#define SIGALRM    14 /* alarm */
#define SIGTERM    15 /* terminate */
#define SIGCHLD    17 /* child process termination,stopped */
#define SIGCONT    18 /* continue */
#define SIGSTOP    19 /* stop, cannot be handled */
#define SIGTSTP    20 /* interactive stop */
#define SIGTTIN    21 /* background process wants to read */
#define SIGTTOU    22 /* background process wants to write */
#define SIGEMT      7 /* obsolete */
#define SIGBUS     10 /* obsolete */

#ifdef  _POSIX_SOURCE
#   define __sighandler_t sighandler_t
    typedef void (*sighandler_t) (int);
#else
    typedef void (*__sighandler_t) (int);
#endif

#define SIG_DFL   ((__sighandler_t)  0)  /* default */
#define SIG_IGN   ((__sighandler_t)  1)  /* ignore  */
#define SIG_HOLD  ((__sighandler_t)  2)  /* hold    */
#define SIG_CATCH ((__sighandler_t)  3)  /* catch   */
#define SIG_ERR   ((__sighandler_t) -1)  /* error   */

#ifdef _POSIX_SOURCE
struct sigaction 
{
  __sighandler_t sa_handler;    /* signal handler */
  sigset_t       sa_mask;       /* blocked signals during signal handling */
  int            sa_flags;      /* flags */
};

/* flags */
#define SA_ONESHOT   0x40000000
#define SA_NOMASK    0x80000000
#define SA_ONSTACK   0x0001      /* deliver signal on alternate stack */
#define SA_RESETHAND 0x0002      /* reset signal handler when signal caught */
#define SA_NODEFER   0x0004      /* don't block signal while catching */
#define SA_RESTART   0x0008      /* automatic system call restart */
#define SA_SIGINFO   0x0010      /* extended signal handling */
#define SA_NOCLDWAIT 0x0020      /* no zombie processes */
#define SA_NOCLDSTOP 0x0040      /* no SIGCHLD */

/* sigprocmask */
#define SIG_BLOCK    0           /* block signal    */
#define SIG_UNBLOCK  1           /* unblock signal  */
#define SIG_SETMASK  2           /* set signal mask */
#define SIG_INQUIRE  3           /* internal        */ 

extern int raise (int sig);
extern int kill  (pid_t pid, int sig);
//extern void  (*signal(int signum, void (*handler)(int)))(int);
int signal(int signum, sighandler_t handler);
extern int sigaction   (int signum,  const  struct  sigaction  *act, struct sigaction *oldact);
extern int sigprocmask (int  how,  const  sigset_t *set, sigset_t *oldset);
extern int sigpending  (sigset_t *set);
extern int sigsuspend  (const sigset_t *mask);

extern int sigemptyset (sigset_t *set);
extern int sigfillset  (sigset_t *set);
extern int sigaddset   (sigset_t *set, int signum);
extern int sigdelset   (sigset_t *set, int signum);
extern int sigismember (const sigset_t *set, int signum);

#endif /* _POSIX_SOURCE */
#ifdef __cplusplus
 }
#endif
#endif /* _SYS_SIGNAL_H */
