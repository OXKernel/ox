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
/**************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *	@module
 *		strtoul.c	
 *
 **************************************************************/
#include <ox/types.h>
#include <ox/fs/bool.h>
#include <ox/lib/conversions.h>
#include <ox/lib/ctype.h>

/*
 * strtoul:-
 * 	This version of strtoul works similarly to 
 * 	the ANSI C definition.  The string charp
 * 	is converted into an unsigned long based on the
 * 	base.  endptr contains the position we left
 * 	off during processing.
 */ 	
unsigned long 
strtoul( const char *charp, char **endptr, int base)
{
	unsigned long res = 0;
	unsigned long val = 0;

	if(!charp)
		return (res);

	if(base == 0) {
		/* assume decimal */
		base = DEC;
		if(*charp == '0') {
		/* assume octal */
			charp++;
			base = OCT;
		}
		if(base == OCT && *charp == 'x' && isxdigit(*(charp + 1))) {
		/* assume hex   */
			charp++;
			base = HEX;
		}
	}
	/* accumulate result */
	while(true) {
		if(isdigit(*charp))
			val = *charp - '0';
		else if (islower(*charp) && (*charp >= 'a' && *charp <= 'f'))
			val = *charp - 'a' + 10;	
		else if (isupper(*charp) && (*charp >= 'A' && *charp <= 'F'))
			val = *charp - 'A' + 10;	
		else if (*charp == '0' || *charp == 'x') {
			charp++;
			continue;
		}
		else 
			break;
		/* do the actual conversion */
		res = res * base + val;
		charp++;
	}

	/* set end pointer */
	if(endptr)
		*endptr = charp;
	
	/* return result */
	return (res);

}/* strtoul */

/*
 * strtol:-
 * 	This version of strtoul works similarly to 
 * 	the ANSI C definition.  The string charp
 * 	is converted into a long based on the
 * 	base.  endptr contains the position we left
 * 	off during processing.
 */ 	
long 
strtol( const char *charp, char **endptr, int base)
{
	/* check if it is negative */
	if(*charp == '-')
		return (-strtoul(++charp,endptr,base));
	else
		return (strtoul(charp,endptr,base));

}/* strtoul */

/*
 * atoi:-
 * 	This version of atoi works similarly to
 * 	the ANSI C definition.  The string charp
 * 	is converted into an integer.
 */
int
atoi ( char *charp )
{
	return ((int)strtol(charp,(char **)NULL,DEC));	

}/* atoi */

/*
 * EOF
 */
