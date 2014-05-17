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
 * <sys/stat.h>
 *
 * file system status
 *
 ********************************************************/
#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1
#ifdef __cplusplus
 extern "C" {
#endif

struct stat {
   dev_t   st_dev;        /* major and minor device numbers */
   ino_t   st_ino;        /* i-node number */
   mode_t  st_mode;       /* file mode */
   nlink_t st_nlink;      /* number of links */
   uid_t   st_uid;        /* user id  */
   gid_t   st_gid;        /* group id */
   dev_t   st_rdev;
   off_t   st_size;       /* file size */
   blksize_t st_blksize;  /* blocksize for file system I/O */
   blkcnt_t  st_blocks;   /* number of 512B blocks allocated */
   time_t  st_atime;      /* time last accessed */
   time_t  st_mtime;      /* time last modified */
   time_t  st_ctime;      /* time last changed  */ 
};

#define S_IFMT   00170000              
#define S_IFREG   0100000  /* standard file */
#define S_IFBLK   0060000  /* block special */
#define S_IFDIR   0040000  /* directory */
#define S_IFCHR   0020000  /* character special */
#define S_IFIFO   0010000  /* pipe/fifo */
#define S_ISUID   0004000  /* user id  set on exe */
#define S_ISGID   0002000  /* group id set on exe */
#define S_ISVTX   0001000  /* save swapped text */
#define S_IFLNK   0120000 /* Symbolic link.  */
#define S_IFSOCK  0140000 /* Socket.  */


/* POSIX Sec 5.6.1.1 */
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG) /* is regular file */
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR) /* is directory */
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR) /* is char special */
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK) /* is block special */
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO) /* is pipe/fifo */

/* POSIX st_mode masks */
/* user */
#define S_IRWXU 00700  /* rwx------ */
#define S_IRUSR 00400  /* r-------- */
#define S_IWUSR 00200  /* -w------- */
#define S_IXUSR 00100  /* --x------ */

/* group */
#define S_IRWXG 00070  /* ---rwx--- */
#define S_IRGRP 00040  /* ---r----- */
#define S_IWGRP 00020  /* ----w---- */
#define S_IXGRP 00010  /* -----x--- */

/* other */
#define S_IRWXO 00007  /* ------rwx */
#define S_IROTH 00004  /* ------r-- */
#define S_IWOTH 00002  /* -------w- */
#define S_IXOTH 00001  /* --------x */

extern int chmod (const char *_path, mode_t mode);
extern int fstat (int _fildes, struct stat *_buf);
extern int mkdir (const char *_path, mode_t mode);
extern int mkfifo(const char *_path, mode_t mode);
extern int stat  (const char *_path, struct stat *_buf);
extern mode_t umask(mode_t mask);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_STAT_H */
