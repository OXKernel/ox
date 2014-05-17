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
 * 	<ox/bool_t.h>
 *
 *		boolean type in C.  We used the C++ convention,
 *		rather than the "classic" C, roll your own.
 *		We will probably not use this much,
 *		but its nice to standardize...
 */
#ifndef _OX_BOOL_T_H
#define _OX_BOOL_T_H  1
#ifndef __cplusplus

typedef int bool;
#define false 0
#define true  1

#endif
#endif  /* _OX_BOOL_T_H */
