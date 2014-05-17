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
/*********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 * 
 *	@module
 *		panic.c
 *
 ********************************************************/
#include <stdarg.h>

#include <ox/error_rpt.h>
#include <ox/linkage.h>

#include <ox/lib/vsprintk.h>

#include <asm_core/util.h>

#include "platform/drivers/chara/console.h"

__clinkage__
void
debug_panic()
{
    // panic("We are here...");
    printk("We are here...\n");
    for(;;);
}

__clinkage__
void  
panic(const char *message,...)
{
	static char buf[MAX_BUFFER];
	va_list   param;

    memset(buf,0x0,MAX_BUFFER);
	va_start (param,message);
	vsprintk (buf,message,param);
	va_end   (param);

	printk("OX KERNEL PANIC: %s\n",buf);

	/* sync buffer cache
	 */
	/* asm_hlt will stop the CPU execution
	 */
	asm_hlt();

}/* panic */

/*
 * EOF
 */
