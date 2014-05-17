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
 * <sys/times.h>
 *
 * system times 
 * partial implementation:-
 *  POSIX Standard: 4.5.2 Process Times
 *
 *******************************************************/
#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H  1
#include <sys/types.h>
#ifdef __cplusplus
 extern "C" {
#endif

struct tms {
      clock_t tms_utime;     /* User CPU time   */
      clock_t tms_stime;     /* System CPU time */
      clock_t tms_cutime;    /* User CPU time of child processes */
      clock_t tms_cstime;    /* System CPU time of child processes */
};

/* returns the time the
 * for a process, (clock_t) -1 on error
 */
extern clock_t times(struct tms *tp);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_TIMES_H */
