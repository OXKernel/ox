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
 *  fcntl.c
 *  file control (fcntl) system call
 *  
 ********************************************************/
#include <unistd.h>
#include <platform/call.h>
#include <platform/syscall.h>
#include <stdarg.h>

/* int fcntl  (int fd,int request, ...); */
_syscall_3m(fcntl,int,_fcntl,int,fd,int,request,unsigned,arg);

int fcntl(int fd, int request, ...)
{
	unsigned param;
	if(request & F_GETFD || request & F_SETFD) {
		va_list arg;
		va_start(arg, request);
		param = (unsigned)va_arg(arg,long);
		va_end(arg);
		_fcntl(fd,request,param);
	} else if(request & F_GETFL || request & F_SETFL || request & F_SETLKW) {
		va_list arg;
		va_start(arg, request);
		param = (unsigned)va_arg(arg,struct flock *);
		va_end(arg);
		_fcntl(fd,request,param);
	} else {
		_fcntl(fd,request,0);
	}
}
