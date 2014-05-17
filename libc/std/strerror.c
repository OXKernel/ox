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
 * strerror.c
 * 
 * Implementation of strerror(), definition of
 * syserr_tab 
 *
 ********************************************************/
#include <string.h>
#include <errno.h>

/* strerror
 * returns a pointer to static
 * string providing a description
 * of an error that occured
 *  see errno.h for possible errors
 */
char *strerror(int errno)
{
  static char *syserr_tab[] = {
         "OK",                              /* OK     */
         "NO PERMISSION",                   /* EPERM  */
         "NO ENTRY",                        /* ENOENT */
         "NO SUCH  PROCESS",                /* ESRCH  */
         "INTERRUPTED SYSTEM CALL",         /* EINTR  */
         "INPUT/OUTPUT ERROR",              /* EIO    */
         "NO SUCH DEVICE/ADDRESS",          /* ENXIO  */
         "ARGUMENT LIST TOO BIG",           /* E2BIG  */
         "INVALID EXECUTABLE FORMAT",       /* ENOEXEC */
         "BAD FILE DESCRIPTOR",             /* EBADF  */
         "NO SUCH CHILD PROCESS",           /* ECHILD */
         "RESOURCE TEMPORARILY N/A",        /* EAGAIN */
         "OUT OF MEMORY",                   /* ENOMEM */
         "NO ACCESS/PERMISSION DENIED",     /* EACCES */
         "BAD ADDRESS/POINTER",             /* EFAULT */
         "NOT A BLOCK SPECIAL FILE",        /* ENOTBLK */
         "RESOURCE BUISY",                  /* EBUSY  */
         "FILE EXISTS",                     /* EEXIST */
         "IMPROBER LINK",                   /* EXDEV  */
         "NO SUCH DEVICE",                  /* ENODEV */
         "NOT A DIRECTORY",                 /* ENOTDIR */
         "IS A DIRECTORY",                  /* EISDIR */
         "INVALID ARGUMENT",                /* EINVAL */
         "TOO MANY OPEN FILES IN SYSTEM",   /* ENFILE */
         "TOO MANY OPEN FIELS",             /* EMFILE */
         "INAPPROPERIATE I/O OPERATION",    /* ENOTTY */
         "NOT IN USE",                      /* ETXTBSY */
         "FILE TOO LARGE",                  /* EFBIG */
         "NO SPACE LEFT ON DEVICE",         /* ENOSPC */
         "INVALID SEEK",                    /* ESPIPE */
         "READ-ONLY FILE SYSTEM",           /* EROFS  */
         "TOO MANY LINKS",                  /* EMLINK */
         "BROKEN PIPE",                     /* EPIPE  */
         "DOMAIN ERROR",                    /* EDOM   */
         "RANGE  ERROR",                    /* ERANGE */
         "RESOURCE DEADLOCK AVOIDED",       /* EDEADLCK */
         "FILE NAME TOO LONG",              /* ENAMETOOLONG */
         "NO LOCKS AVAILABLE",              /* ENOLCK */
         "NOT IMPLEMENTED",                 /* ENOSYS */
         "DIRECTORY NOT EMPTY",             /* ENOTEMPTY */
         "TOO MANY SYMBOLIC LINKS",         /* ELOOP */

#ifdef __OX__NETWORKING__
         "INVALID PACKET SIZE",             /* EPACKSIZE */
         "NO BUFFERS AVAILABLE",            /* EOUTOFBUFS */
         "BAD ioctl",                       /* EBADIOCTL */
         "BAD MODE",                        /* EBADMODE */
         "OPERATION WOULD BLOCK",           /* EWOULDBLOCK */
         "BAD DESTINATION",                 /* EBADDEST */
         "DESTINATION NOT REACHABLE",       /* EDSTNOTRCH */
         "CONNECTION ALREADY ESTABLISHED",  /* EISCONN */
         "ADDRESS IN USE",                  /* EADDRINUSE */
         "CONNECTION REFUSED",              /* ECONNREFUSED */
         "CONNECTION RESET",                /* ECONNRESET */
         "TIMED OUT",                       /* ETIMEDOUT */
         "URGENT DATA",                     /* EURG */
         "NOT URGENT DATA",                 /* ENURG */
         "NOT CONNECTED",                   /* ENOTCONN */
         "WRITE ON SHUTDOWN CONNECTION",    /* ESHUTDOWN */
         "NO CONNECTION",                   /* ENOCONN */
#endif
   };

  if(errno < 0 || (errno >= ENERROR && errno != EGEN))
      return ("strerror: INVALID ERROR NUMBER");

  if(errno == EGEN) {
        return "GENERIC ERROR";
  }
  return (syserr_tab[errno]);
}
/*
 * eof
 */
