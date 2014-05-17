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
 * 	<ox/error_rpt.h>
 *    		 OX error reporting functions.
 *
 ********************************************************/
#ifndef _OX_ERROR_RPT_H
#define _OX_ERROR_RPT_H  1
#ifdef __cplusplus
 extern "C" {
#endif

#include <ox/lib/printk.h>

#ifdef __VERBOSE_ERROR_CHECKING__
#define PANIC(message)                                                  \
        panic("line (%d), file (%s)\nERROR (%s)\n",__LINE__,__FILE__,(message));
#else
#define PANIC(message)							\
	panic((message));
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define PRINTK(message)                                                  \
        printk("line (%d), file (%s)\nERROR (%s)\n",__LINE__,__FILE__,(message));
#else
#define PRINTK(message)							\
	printk((message));
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define LINE()                                                  \
        printk("line (%d), file (%s)\nERROR (%s)\n",__LINE__,__FILE__);
#else
#define LINE()							\
        ;
#endif

#define MAX_BUFFER 4096

extern void print_reg(unsigned int reg);
extern void debug_panic();
extern void panic (const char *message,...);

#ifdef __cplusplus
 }
#endif
#endif /* _OX_ERROR_RPT_H */
