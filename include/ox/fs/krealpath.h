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
//
// @file:
//      krealpath.h
// @description:
//      Implements kernel's realpath resolution.
//      This resolves a relative path into an absolute
//      path given a starting current_dir.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _KREALPATH_H
#define _KREALPATH_H

//
// krealpath:
//  dev = device we are running on
//  current_dir = block representing current directory
//  path =  the path to resolve
//  res_path = the resulting path from this function
//  error = the errno error value
//
//  NOTE: path and res_path are allocated outside of this function
//  and are normall just stack based arrays. They must be
//  MAX_PATH in length and the resulting path must also be 
//  MAX_PATH in length.
//
//  Function returns NULL in the event of an error.
//
char *krealpath(int dev, block_t current_dir, char *path, char *res_path, int *error);

#endif
