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
 *  <sys/timeb.h>
 *   ftime system call
 *  
 ********************************************************/
#ifndef  _SYS_TIMEB_H
#define  _SYS_TIMEB_H  1
#ifdef __cplusplus
 extern "C" {
#endif
struct timeb
{
    time_t time;                /* Seconds since epoch, as from `time'.  */
    unsigned short int millitm; /* Additional milliseconds.  */
    short int timezone;         /* Minutes west of GMT.  */
    short int dstflag;          /* Nonzero if Daylight Savings Time used.  */
};

extern int ftime(struct timeb *tp);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_TIMEB_H */
