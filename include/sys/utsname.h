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
 * <sys/utsname.h>
 *
 * structure identifying the system:-
 *   POSIX Standard: 4.4 System Identification
 *******************************************************/
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H  1
#ifdef __cplusplus
 extern "C" {
#endif

#define _UTSNAME_LENGTH           65
#define _UTSNAME_NODENAME_LENGTH  _UTSNAME_LENGTH
#define _UTSNAME_DOMAIN_LENGTH    _UTSNAME_LENGTH

struct utsname
{
  char sysname[_UTSNAME_LENGTH];           /* OS implementation name */
  char nodename[_UTSNAME_NODENAME_LENGTH]; /* network node name */
  char release[_UTSNAME_LENGTH];           /* release of the OS */
  char version[_UTSNAME_LENGTH];           /* version of the OS */
  char machine[_UTSNAME_LENGTH];           /* type of hardware  */
  char domainname[_UTSNAME_DOMAIN_LENGTH]; /* domain name       */
};

extern int uname (struct utsname *name);

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_UTSNAME_H */ 
