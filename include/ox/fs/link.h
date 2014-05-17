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
//      link.h
// 
// @description:
//      Implementation of file system symlinks
//      and hardlinks.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _LINK_H
#define _LINK_H

#define MAX_RECURSION 1024
int klink(const char *oldpath, const char *newpath);
block_t kreadlink(int dev, int current_dir, char *pathcomponent, int *error);

#endif // _LINK_H
