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
 * @module
 * 	init.c 
 *	kernel init program 
 *	
 *	NOTE: This program is special. First, it is
 *	      represented in the kernel as a static
 *	      structure. Second, it does not recieve
 *	      signals/exceptions. Third, it is the parent
 *	      process of all processes in the system.
 *	      Forth, it does not use normal system call
 *	      mechanisms, instead it uses inline routines.
 *
 ********************************************************/

void
_start()
{
    main();
}

void
main()
{
}/* main */
