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
/*************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *************************************************************
 * @module 
 * 	main.c
 *
 * @description
 *		The OX kernel is loaded by the boot program,
 *	which in turn calls _start, the assembly language
 *	entry point of the OS.  _start calls main(),
 *	the machine-dependent entry point,
 *	which in turn calls ox_main() the kernel-dependent
 *	entry point.
 *		Returning non-zero indicates error, which
 *	causes us to panic.
 */
#include <stddef.h>

#include <ox/linkage.h>
#include <ox/version.h>
#include <ox/error_rpt.h>

extern int ox_main( int argc, char **argv );	/* kernel/ox_main.c */

__clinkage__
void main( void )
{
	int    argc = 0;
	char **argv = NULL;

	printk("*********************************************\n"
	       "* Copyright (C) 2013. Roger George Doss.    *\n"
           "*           Released under GPL v2           *\n"
	       "*********************************************\n"
	       "*           OX kernel boot                  *\n"
	       "*           Version (%s)                *\n"
	       "*********************************************\n",OX_VERSION_STR);

	if(ox_main(argc,argv))
		panic("main(): kernel failed to initialize...");

    for(;;)
        /* do nothing */;

}/* startup_init */

/*
 * EOF
 */
