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
 * ctype.h
 *
 *   This is an optimized verson of ANSI
 *  ctype.h, we use these logical expressions
 *  instead of functions. The result is the
 *  same, only we don't have to make a function
 *  call. NOTE: if the expression is true,
 *  it will evaluate to 1, 0 other wise.
 *
 *********************************************************/
#ifndef _CTYPE_H
#define _CTYPE_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* isalnum
 * isalpha or isdigit == true
 */
#define isalnum(c) \
      (isalpha(c) || isdigit(c))

/* isalpha
 * isupper or islower == true
 */
#define isalpha(c) \
      (isupper(c) || islower(c))

/* iscntrl
 * control character
 */
#define iscntrl(c) \
       (((c >= 0) && (c <= 0x1f)) || (c == 0x7f))

/* isdigit
 * decimal character
 */
#define isdigit(c) \
       ((c >= 0x30) && (c <= 0x39))

/* isgraph
 * printing characters except 
 * space
 */
#define isgraph(c) \
       ((c > 0x20) && (c <= 0x7e)) 

/* islower
 * lower-case letter
 */
#define islower(c) \
       ((c >= 0x61) && (c <= 0x7a))

/* isprint
 * printing character including space
 */
#define isprint(c) \
       ((c >= 0x20) && (c <= 0x7e))

/* ispunct
 * printing character except space or letter
 * or digit
 */
#define ispunct(c) \
       (((c > 0x20) && (c <= 0x7e)) && !isalnum(c))

/* isspace
 * space, formfeed, newline, carriage return
 * tab, vertical tab
 */
#define isspace(c) \
       ((c == 0x20) || ((c >= 0x09) && (c <= 0x0d)))

/* isupper
 * upper-case letter
 */
#define isupper(c) \
       ((c >= 0x41) && (c <= 0x5a))

/* isxdigit
 * hexadecimal digit
 */
#define isxdigit(c) \
       (isdigit(c) || (((c >= 0x41) && (c <= 0x46)) || ((c >= 0x61) && (c <= 0x66))))


/* isalnum
 * isalpha or isdigit == true
 */
#define isalnum(c) \
      (isalpha(c) || isdigit(c))

/* isalpha
 * isupper or islower == true
 */
#define isalpha(c) \
      (isupper(c) || islower(c))

/* tolower
 * convert c to lower case
 */
#define tolower(c) \
       (isupper(c) ? (c + 0x20) : (c))

/* toupper
 * convert c to upper case
 */
#define toupper(c) \
       (islower(c) ? (c - 0x20) : (c))


#ifdef __cplusplus
 }
#endif
#endif
