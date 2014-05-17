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
 * <string.h>
 * 
 * ANSI C string library
 ********************************************************/
#ifndef _STRING_H
#define _STRING_H  1
#ifdef __cplusplus
 extern "C" {
#endif
#include <sys/types.h>

/* strerror
 * returns a pointer to static
 * string providing a description
 * of an error that occured
 */
char *strerror(int errno);

/* strcpy
 * copy from src to dest, return dest
 * if src || dest end,  return dest
 */
char *strcpy(char  *dest,const char *src);

/*
 * strnlen
 * Return length of string up to len.
 */
size_t strnlen(const char *str, size_t len);

/* strncpy
 * copy n bytes from src to dest, return dest
 * if src is too small, dest will be '\0' padded
 * if dest too small,  return dest
 */
char *strncpy(char *dest,const char *src,size_t n);

/* strcat
 * concatenates src to the end of dest
 * returns dest
 * NOTE: dest must be large enough
 */
char *strcat(char *dest,const char *src);

/* strncat
 * concatenate at most n char of string src
 * to string dest, terminate dest with '\0'
 * return dest
 * NOTE: dest must be big enough
 */
char *strncat(char *dest,const char *src,size_t n);

/* strcmp
 * compare cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  strcmp(const char *cs,const char *ct);

/* strncmp
 * compare n bytes in cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  strncmp(const char *cs,const char *ct,size_t n);

/* strchr
 * returns pointer to first occurance
 * of c in cs, NULL if not present
 */
char *strchr(const char *cs,char c);

/* strrchr
 * returns pointer to last occurance of c in
 * cs, NULL if not present
 */
char *strrchr(const char *cs,char c);

/* strspn
 * return length of prefix cs
 * consisting of characters in ct
 */ 
size_t strspn(const char *cs, const char *ct);

/* strcspn
 * return length of prefix of cs
 * consisting of characters NOT in ct
 */
size_t strcspn(const char *cs, const char *ct);

/* strpbrk
 * return pointer to first occurance
 * in string cs of ANY character of 
 * string ct, NULL if not present
 */
char *strpbrk(const char *cs,const char *ct);

/* strstr
 * returns pointer to first occurance of
 * string ct in cs, NULL if not present
 */
char *strstr(const char *cs,const char *ct);

/* strlen
 * returns the length
 * of cs
 */
size_t strlen(const char *cs);

/* strtok
 * searches s for tokens delimeted
 * by characters in ct
 * a sequence of calls of strtok
 * splits s into tokens, each delimited
 * by a character from ct. The first call
 * in a sequence has a non-NULL s. It finds
 * the first token in s consisting of characters
 * NOT in ct; it terminates that by overwriting
 * the NEXT character of s with a '\0', and returns
 * a pointer to the token.
 * Each subsequent call, indicated by a NULL
 * s, returns the next such token, searching
 * from just past the end of the previous one.
 * strtok returns NULL if no further token is
 * found. ct maybe different on each call
 */
char *strtok(char *s,const char *ct);

/* memcpy
 * copy n bytes from src to dest, return dest
 */
void *memcpy(void *dest,const void *src, size_t n);

/* memmove
 * copy n bytes from src to dest, return dest
 * NOTE: memmove allows for overlapping memory
 */
void *memmove(void *dest,const void *src, size_t n);

/* memcmp
 * compare n bytes in cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  memcmp(const void *cs,const void *ct,size_t n);

/* memchr
 * return pointer to first occurance
 * of character c in cs, NULL if not
 * found 
 */
void *memchr(const void *cs,char c,size_t n);

/* memset
 * set c, in s, for n bytes
 */
void *memset(void *s,int c,size_t n);

#ifdef __cplusplus
 }
#endif
#endif /* _STRING_H */
