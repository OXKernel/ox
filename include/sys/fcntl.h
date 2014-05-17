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
 * <sys/fcntl.h>
 *   file control
 *
 *******************************************************/
#ifndef _SYS_FCNTL_H
#define _SYS_FCNTL_H  1
#ifdef __cplusplus
 extern "C" {
#endif
/* POSIX Table 6-1 */
/* various fcntl requests */
#define F_DUPFD       0  /* dup file descriptor */
#define F_GETFD       1  /* get file decriptor flags */
#define F_SETFD       2  /* set file descriptor flags */
#define F_GETFL       3  /* get file status flags */
#define F_SETFL       4  /* set file status flags */
#define F_GETLK       5  /* get file locking information */
#define F_SETLK       6  /* set file locking information */
#define F_SETLKW      7  /* set file locking information; if blocked,wait */

/* POSIX Table 6-2 */
#define FD_CLOEXEC    1  /* close on exec flag */

/* POSIX Table 6-3 */
#define F_RDLCK       1 /* read lock  - shared */
#define F_WRLCK       2 /* write lock - exclusive */ 
#define F_UNLCK       3 /* unlock */

/* POSIX Table 6-4 */
#define O_CREAT      00100 /* create file if it does not exist */
#define O_EXCL       00200 /* exclusive */
#define O_NOCTTY     00400 /* no controlling terminal */
#define O_TRUNC      01000 /* truncate */

/* POSIX Table 6-5 */
#define O_APPEND     02000 /* append */
#define O_NONBLOCK   04000 /* no delay */

/* POSIX Table 6-6 */
#define O_RDONLY     01     /* read only */
#define O_WRONLY     02     /* write only */
#define O_RDWR       04     /* read/write */

/* POSIX Table 6-7 */
#define O_ACCMODE   03     /* file access mode */

/* POSIX Table 6-8 */
struct flock {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
};

extern int creat (const char *path,mode_t mode); 
extern int fcntl (int fd,int request, ...);
extern int open  (const char *path, int flag, ...);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_FCNTL_H */
