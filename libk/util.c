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
 ***************************************************************
 *
 *	@module
 *		util.c
 *
 ***************************************************************/
#include <ox/types.h>

/*
 * to_cp:-
 * 	Converts a wide char string to
 * 	a char string.
 */
inline
void
to_cp( char *buf, wchar_t *widech )
{
	wchar_t *wp;
	for( wp = widech; wp && *wp && buf && *buf; wp++ ) {
		*buf++  = *wp & 0xFF;
		*buf++  = *wp >> 0x8;
	}

}/* to_cp */

inline
void
cl_buf( char *buf )
{
	char *p;
	for ( p = buf; p && *p; p++)
		*p = 0x0;

}/* cl_buf */

inline
void
cl_bufn( char *buf, char fill, size_t size )
{
	size_t i;
	char *p;

	for ( i = 0, p = buf; p && *p; p++,i++ )
		if(i < size )
			*p = 0x0;
		else
			break;
}/* cl_bufn */

inline
void
fl_buf( char *buf, char fill )
{
	char *p;
	for ( p = buf; p && *p; p++)
		*p = fill;

}/* fl_buf */

inline
void
fl_bufn( char *buf, char fill, size_t size )
{
	size_t i;
	char *p;

	for ( i = 0, p = buf; p && *p; p++,i++ )
		if(i < size )
			*p = fill;
		else
			break;
	
}/* fl_bufn */

/*
 * EOF
 */
