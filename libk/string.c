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
 * string.c
 *
 * Implementation of ANSI C string library
 * for OX kernel
 ********************************************************/
#include <string.h>

/* strcpy
 * copy from src to dest, return dest
 * if src || dest end,  return dest
 */
char *strcpy(char  *dest,const char *src)
{
   char *s = dest;
   
   while(*dest++ = *src++)
         ;
   return (s);
}

/* strnlen
 * Return length of string up to max.
 */
size_t strnlen(const char *str, size_t len)
{
    register const char *ptr;
    for(ptr = str; *ptr && len--; ++ptr)
            ;
    return(ptr - str);
}

/* strncpy
 * copy n bytes from src to dest, return dest
 * if src is too small, dest will be '\0' padded
 * if dest too small,  return dest
 */
char *strncpy(char *dest,const char *src,size_t n)
{
   size_t i;
   char *s = dest;
    
   for(i=0; ((i < n) && (*dest++ = *src++)); i++)
         ;
   for(   ; ((i < n) && (*dest++ = '\0'))  ; i++)
         ;
   return (s);
}

/* strcat
 * concatenates src to the end of dest
 * returns dest
 * NOTE: dest must be large enough
 */
char *strcat(char *dest,const char *src)
{
   char *s = dest;
  
   for( ; dest && *dest ; dest++ )
       ;
   while(*dest++ = *src++)
      ;
   return (s);
}

/* strncat
 * concatenate at most n char of string src
 * to string dest, terminate dest with '\0'
 * return dest
 * NOTE: dest must be big enough
 */
char *strncat(char *dest,const char *src,size_t n)
{
   size_t i;
   char *s = dest;

   for( ; dest && *dest ; dest++ )
       ;
   for(i=0; ((i < n) && (*dest++ = *src++)); i++)
          ;
   *dest = '\0';
   return (s);
}

/* strcmp
 * compare cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  strcmp(const char *cs,const char *ct)
{
   for(  ; *cs == *ct; cs++, ct++)
           if(*cs == '\0')
             return (0);
   return (*cs - *ct);
}

/* strncmp
 * compare n bytes in cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  strncmp(const char *cs,const char *ct,size_t n)
{
   size_t i;

   for(i=0; ((i < n) && (*cs == *ct)); i++,cs++,ct++)
            if(*cs == '\0')
               return (0);
   return (*cs - *ct); 
}

/* strchr
 * returns pointer to first occurance
 * of c in cs, NULL if not present
 */
char *strchr(const char *cs,char c)
{
   for( ; cs && *cs ; cs++)
       if(*cs == c)
          return ((char *)cs);
   return (NULL);
}

/* strrchr
 * returns pointer to last occurance of c in
 * cs, NULL if not present
 */
char *strrchr(const char *cs,char c)
{
   const char *s = cs;
   for( ; cs && *cs ; cs++)
        ;
   for( ; s != cs; cs--)
         if(*cs == c)
            return ((char *)cs);
   return (NULL);
}

/* strspn
 * return length of prefix cs
 * consisting of characters in ct
 */ 
size_t strspn(const char *cs, const char *ct)
{
  size_t i=0;
  for( ; cs && *cs && ct && *ct ; ct++)
       for( ; *ct == *cs; i++,cs++)
            ;
  return (i);
}

/* strcspn
 * return length of prefix of cs
 * consisting of characters NOT in ct
 */
size_t strcspn(const char *cs, const char *ct)
{
  size_t i=0;
  for( ; cs && *cs && ct && *ct ; ct++)
       for( ; *ct != *cs; i++,cs++)
            ;
  return (i);

}

/* strpbrk
 * return pointer to first occurance
 * in string cs of ANY character of 
 * string ct, NULL if not present
 */
char *strpbrk(const char *cs,const char *ct)
{
  size_t i;
  const char *t = ct;
  for( ; cs && *cs ; cs++) {
     for(i = 0,ct = t ; ct && *ct ; ct++,i++)
          if(*(cs + i) == *ct)
               return ((char *)cs);
  }
  return (NULL);
}

/* strstr
 * returns pointer to first occurance of
 * string ct in cs, NULL if not present
 */
char *strstr(const char *cs,const char *ct)
{
  size_t i;
  const char *t = ct;
  for( ; cs && *cs ; cs++) {
     for(i = 0,ct = t ; ct && *ct ; ct++,i++)
          if(*(cs + i) != *ct) 
             break;
     if(!*ct)
          return ((char *)cs);
  }
  return (NULL);
}

/* strlen
 * returns the length
 * of cs
 */
size_t strlen(const char *cs)
{
   size_t i=0;
   if(!cs) 
     return (i);
   while(*cs++) 
     i++;
   return (i);
}

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
#ifdef __OX_REENTRANT__
#define SAVESTR *___strtok
#else
#define SAVESTR  ___strtok
#endif

#ifdef __OX_REENTRANT__
char *strtok_r(char *s, const char *ct, char **___strtok)
#else
char *strtok(char *s, const char *ct)
#endif
{
#if (!__OX_REENTRANT__)
        static char  *___strtok;
#endif
        char   *i, *j, *k;

        i = (!s) ? SAVESTR : s;
        if(!i)
           return(NULL);

        j = i + strspn(i, ct);

        if(!*j)
           return(NULL);

        if(!(k = strpbrk(j, ct)))
                SAVESTR = 0;
        else {
                *k = '\0';
                SAVESTR = ++k;
        }

        return(j);
}

/* memcpy
 * copy n bytes from src to dest, return dest
 */
void *memcpy(void *dest,const void *src, size_t n)
{
   unsigned char *s;
   const unsigned char *t;
   void    *r;
   size_t   i;

   s = r = dest;
   t = (const unsigned char *) src;
   for(i=0; i < n ; i++)
        *s++ = *t++;
   return (r);
}

/* memmove
 * copy n bytes from src to dest, return dest
 * NOTE: memmove allows for overlapping memory
 */
void *memmove(void *dest,const void *src, size_t n)
{
    size_t i;
    char *d = (char *)dest;
    char *s = (char *)src;

    if(d > s)
        for(i=n-1; i >= 0; i--)
             d[i] = s[i];
    else 
       while(n--)
           *d++ = *s++;
    
    return (dest);
}

/* memcmp
 * compare n bytes in cs and ct
 * return < 0 if cs <  ct
 * return   0 if cs == ct
 * return > 0 if cs >  ct
 */
int  memcmp(const void *cs,const void *ct,size_t n)
{
   size_t i;
   const char *s,*t;
   s = (const char *)cs;
   t = (const char *)ct;

   for(i=0; ((i < n) && (*s == *t)); i++,s++,t++)
        ;
   if(i == n)
      return (0);
   return (*s - *t);
}

/* memchr
 * return pointer to first occurance
 * of character c in cs, NULL if not
 * found 
 */
void *memchr(const void *cs,char c,size_t n)
{
  size_t i;
  const char *s = (const char *)cs;

  for(i=0; i < n; i++,s++)
        if(*s == c)
           return (s);
  return (NULL);
}

/* memset
 * set c, in s, for n bytes
 */
void *memset(void *s,int c,size_t n)
{
   unsigned char *t = (unsigned char *)s;
   void *r = s;
   
   while(n--)
     *t++ = (unsigned char)c;
    
   return (r);
}
/*
 * eof
 */
