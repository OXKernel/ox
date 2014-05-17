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
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *	@module
 *		ultostr.c
 *
 **************************************************************/
#include <ox/defs.h>

#include <ox/lib/conversions.h>

/*
 * ultostr:-
 * 	Converts unsigned long values into NULL terminated
 * 	char strings based on base.  This routine is the
 * 	complement of strtoul. base == 0 is assumed to
 * 	be DEC.
 */
int
ultostr ( char *buf, unsigned long val, int base )
{
	char tmp = 0;
	static char tbuf[Nr_DIGITS + 2];
	char *tp = tbuf, *t = buf;

	if(!t)
		return (0);

	if(base == 0) {
		/* assume decimal */
		base = DEC;
	}

	/* handle formatting */
	if(base == OCT)
		*t++ = '0';

	if(base == HEX) {
		*t++ = '0';
		*t++ = 'x';
	}

	/* handle zero */
	if(!val) {
		*t++ = '0';
		*t   = '\0';
		goto EXIT;
	}

	/* do the actual conversion */
	while( val ) {
		tmp   = val % base;
		*tp++ = (tmp >= 0xA)? tmp - 0xA + 'A':tmp + '0';
		val  /= base;
	}

	/* copy back into t */
	for(tp-- ; tp >= tbuf; tp--,t++)
	  	*t = *tp;

	*t = '\0';
EXIT:
	return (t - buf);

}/* ultostr */

/*
 * ltostr:-
 * 	Converts long values into NULL terminated
 * 	char strings based on base.  This routine is the
 * 	complement of strtol. base == 0 is assumed to
 * 	be DEC.
 */
int
ltostr ( char *buf, long val, int base )
{
	/* only negative decimal numbers have a sign */
	if(val < 0 && (base == DEC || base == 0)) {
		*buf++ = '-';
		return(ultostr(buf,-val,DEC) + 1);
	}
	else
		return(ultostr(buf,val,base));

}/* ltostr */

/*
 * itoa:-
 * 	Converts integer values into NULL terminated
 * 	char strings with the base assumed to be decimal.
 *      This routine is the complement of atoi.
 */
int
itoa   ( char *buf,int  val )
{
	return(ltostr(buf,val,DEC));

}/* itoa */

/*
 * EOF
 */
