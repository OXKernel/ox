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
//      dir.h
// 
// @description:
//      Implementation of directory specific functionality.
//      This is based on our inode implementation.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _DIR_H
#define _DIR_H

DIR *kopendir(const char *path);
int kclosedir(DIR *dir);
void krewinddir(DIR *dir);
struct dirent *kreaddir(DIR *dir);
int krmdir(const char *path);
int kmkdir(const char *path, mode_t mode);

#endif // _DIR_H
