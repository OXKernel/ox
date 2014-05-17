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
 * <sys/wait.h>
 *
 * system wait:- 
 *   POSIX Standard: 3.2.1 Wait for Process Termination
 *
 ********************************************************/
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H  1
#ifdef __cplusplus
 extern "C" {
#endif

#include <sys/types.h>

/* waitflags 
 */
#define WNOHANG       1    /* don't block waiting */
#define WUNTRACED     2    /* report status of blocked children */

#define WAIT_INT(status)   (status)
#define WAIT_STATUS        int *
#define WAIT_STATUS_DEFN   int *

/******** waitstatus **********************************/

/* exit status  
 */
#define WEXITSTATUS(st) (((st) & 0xFF00) >> 8)

/* signal value 
 */
#define WTERMSIG(st)    ((st) & 0x7F)

/* stop signal  
 */
#define WSTOPSIG(st)    (WEXITSTATUS(st))

/********* waitstatus test macros **********************/

/* if(WIFEXITED(st) != 0 ) => child normal termination 
 */
#define WIFEXITED(st)   (WTERMSIG(st) == 0)

/* if(WIFSTOPPED(st) != 0) => child stopped 
 */
#define WIFSTOPPED(st)  (((st) & 0xFF) == 0x7F)

/* if(WIFSIGNALED(st) != 0) => child terminated via signal 
 */
#define WIFSIGNALED(st) (!WIFEXITED(st) && !WIFSTOPPED(st))


/******** waitstatus generation macros ****************/

#define WEXITCODE(ret, sig)  ((ret) << 8 | (sig))
#define WSTOPCODE(sig)       ((sig) << 8 | 0x7f)
#define WCOREFLAG             0x80

/* if(WCOREDUMP(st) != 0) => child core dump 
 */
#define WCOREDUMP(st)     ((st) & WCOREFLAG)


extern pid_t wait (WAIT_STATUS stat_loc);
extern pid_t waitpid (pid_t pid,WAIT_STATUS stat_loc, int options);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_WAIT_H */
