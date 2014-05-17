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
 *	@module
 * 		printk.c
 *
 ********************************************************/
#include <stdarg.h>

#include <ox/linkage.h>
#include <ox/error_rpt.h>

#include <ox/lib/printk.h>
#include <ox/lib/vsprintk.h>

#include <drivers/chara/console.h>

__clinkage__
int
printk(const char *message,...)
{
	static char buf[MAX_BUFFER];
	int    count = 0;
	va_list  param;

    memset(buf,0x0,MAX_BUFFER);
	va_start (param,message);
	count = vsprintk (buf,message,param);
	va_end   (param);

	console_write(buf);
	return   (count);

}/* printk */

__clinkage__
int
sprintk(char *buf, const char *message,...)
{
	int    count = 0, i = 0;
	va_list  param;

    if(buf) {
        *buf = 0x0;
    } else {
        panic("sprintk:: error NULL buf\n");
    }
	va_start (param,message);
	count = vsprintk (buf,message,param);
	va_end   (param);

	return (count);

}/* sprintk */

__clinkage__
void print_reg(unsigned int reg)
{
    printk(" register [%d]\n", reg);
}

/*
 * EOF
 */
