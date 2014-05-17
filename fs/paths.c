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
//      paths.c
//
// @description:
//      Utility routines for handling paths.
//
// @author:
//      Dr. Roger G. Doss
//
#include "paths.h"
#include "stddef.h"

#ifdef _TEST_PATHS_INC
#include <stdio.h>
#include <string.h>
#endif

//
// push_front:
// Given dst and src are strings,
// pre-pend src in front of dst
// leaving the final output in dst.
// So, if dst='path' and src='/', 
// then 'push_front(dst,src)' results
// in dst='/path'. Returns NULL of the
// length of the resulting string is greater
// than MAX_PATH.
//
char *push_front(char *dst, char *src)
{
    char res_path[MAX_PATH]={0}, *res = res_path, *final=dst;
    memset(res_path,0x0,MAX_PATH);
    if((strlen(dst) + strlen(src)) > MAX_PATH) {
        return NULL;
    }
    while(src && *src) *res++ = *src++;
    while(dst && *dst) *res++ = *dst++;
    strcpy(final,res_path);
    return final;
}

//
// get_next_path:
// start is an integer which is incremented
// as the iteration goes along.
// path is the same pointer from start,
// so we have:=
// while((ptr=get_next_path(&start,ptr_path))!=NULL) {
//     // ptr is the next path component.
//     start++;
// }
char *get_next_path(int *start, char *path)
{
   int i = *start, j = i;
   //printf("start=%d\n",i);
   if(path[i] == '.' && path[i+1] == '\0') {
      *start = *start + 1;
      return &path[i];
   }
   if(path[i] == '.' && path[i+1] == '.' && path[i+2] == '\0') {
      *start = *start + 2;
      return &path[i];
   }
   for(; i < MAX_PATH; i++) {
      if(path[i] == '/') {
         path[i] = '\0';
         *start = i;
         return &path[j];
      }
   }
   i = j;
   //printf("i=%d path[i]=%d\n",i,path[i]);
   if(path[i] != '\0') {
       /* Advance till the end. */
       /* Here there should be no '/' remaining. */
       while(path[i++] != '\0');
       *start = i;
       //printf("RGD path=%s\n",&path[j]);
       return &path[j];
   }
   return NULL;
}

void get_parent_dir(char *path, char *parent)
{
   int i = 0;
   int prev = 0;
   int last = -1;
   int len = strlen(path);
   if(!strcmp(path,".") || !strcmp(path,"..")) {
      strcpy(parent,path);
      return;
   }
   for(i = len - 1; i >= 0; i--) {
      if(path[i]=='/') {
         last = i;
         break;
      }
   }
   for(i=0; i < len; i++) {
      if(path[i]=='/') {
         prev = i;
         if(prev == last) {
            strcpy(parent,"/");
            return;
         }
         break;
      }
   }
   for(i = last-1; i >= 0; i--) {
      if(path[i]=='/') {
         strncpy(parent,&path[i+1],last-i);
         parent[last-(i+1)]='\0';
         return;
      }
   }
}

void get_parent_path(char *path, char *parent)
{
   char tmp[MAX_PATH];
   int i = 0;
   int len = strlen(path);
   int first = 0;
   strcpy(tmp,path);
   if(!strcmp(path,".") || !strcmp(path,"..")) {
      strcpy(parent,path);
      return;
   }
   for(i = 0; i < len; i++) {
     if(tmp[i]=='/') {
         first = i;
         break;
     }
   }
   for(i = len-1; i >= 0; i--) {
      if(tmp[i]=='/' && i != first) {
         tmp[i]='\0';
         strcpy(parent,tmp);
         break;
      } else if(tmp[i]=='/' && i == first) {
         strcpy(parent,"/");
         break;
      }
   }
}

#ifdef _TEST_PATHS
int
main()
{
   char parent[MAX_PATH];
   get_parent_dir("/",parent);
   printf("%s\n",parent);
   get_parent_dir(".",parent);
   printf("%s\n",parent);
   get_parent_dir("..",parent);
   printf("%s\n",parent);
   get_parent_dir("/foo/bar",parent);
   printf("%s\n",parent);
   get_parent_dir("/foo/../bar",parent);
   printf("%s\n",parent);

   get_parent_path("/",parent);
   printf("%s\n",parent);
   get_parent_path(".",parent);
   printf("%s\n",parent);
   get_parent_path("..",parent);
   printf("%s\n",parent);
   get_parent_path("/foo/bar",parent);
   printf("%s\n",parent);
   get_parent_path("/foo/../bar",parent);
   printf("%s\n",parent);
   get_parent_path("/foo/../bar/example/of/long/./././../file/path",parent);
   printf("%s\n",parent);
}
#endif
