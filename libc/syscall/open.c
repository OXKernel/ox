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
 *  open.c
 *   open system call
 *  
 ********************************************************/
#include <unistd.h>
#include <platform/call.h>
#include <platform/syscall.h>
#include <stdarg.h>

/* int open(const char *path,int flag,...); */
_syscall_3m(open,int,_open,const char *, path, int, flag, int, mode);

int open(const char *path, int flag, ...)
{
	if(flag & O_CREAT) {
		va_list arg;
		int mode;
      		va_start(arg, flag);
      		mode = va_arg(arg, int);
      		va_end(arg);
		_open(path,flag,mode);
	} else {
		_open(path,flag,0);
	}
}
