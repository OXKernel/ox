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
 * <platform/i386/sycall.h>
 *    Machine dependent syscall mechanism.
 *    NOTE: These are macros. not functions
 *     	    used to create library functions that 
 *     	    pass control to the kernel.
 *
 ********************************************************/
#ifndef _PLATFORM_I386_SYSCALL_H
#define _PLATFORM_I386_SYSCALL_H  1
#ifdef __cplusplus
 extern "C" {
#endif
extern errno;

/* _syscall_0
 *
 * 0 parameter advisory call
 */
#define _syscall_0(return_type,func_name) \
return_type func_name(void) \
{ \
 \
  long __rtvl; \
 \
   __asm__ volatile ("int $0x80" \
          : "=a" (__rtvl) \
          : "0"  (__CALL_##func_name)); \
 \
    if (__rtvl >= 0) \
        return (__rtvl); \
 \
    errno = - __rtvl; \
 \
    return (-1); \
}

/* _syscall_1
 * 
 * 1 parameter system call
 */
#define _syscall_1(return_type,func_name,type,param) \
return_type func_name(type param) \
{ \
 \
  long __rtvl; \
 \
  __asm__ volatile ("int $0x80" \
          : "=a" (__rtvl) \
          : "0"  (__CALL_##func_name), \
          "b" ((long)(param))); \
 \
  if (__rtvl >= 0) \
      return (__rtvl); \
 \
  errno = - __rtvl; \
 \
  return (-1); \
}

/* _syscall_2
 *
 * 2 parameter system call
 */
#define _syscall_2(return_type,func_name,type1,param1,type2,param2) \
return_type func_name(type1 param1, type2 param2) \
{ \
 \
  long __rtvl; \
 \
  __asm__ volatile ("int $0x80" \
          : "=a" (__rtvl) \
          : "0"  (__CALL_##func_name), \
          "b" ((long)(param1)), "c" ((long)(param2))); \
 \
  if (__rtvl >= 0) \
      return (__rtvl); \
 \
  errno = - __rtvl; \
 \
  return (-1); \
}

/* _syscall_3
 *
 * 3 parameter advisory call
 */
#define _syscall_3(return_type,func_name,type1,param1,type2,param2,type3,param3) \
return_type func_name(type1 param1,type2 param2,type3 param3) \
{ \
 \
  long __rtvl; \
 \
   __asm__ volatile ("int $0x80" \
           : "=a" (__rtvl) \
           : "0"  (__CALL_##func_name), \
           "b" ((long)(param1)),"c" ((long)(param2)), "d" ((long)(param3))); \
 \
  if (__rtvl >= 0) \
     return (__rtvl); \
 \
  errno = - __rtvl; \
 \
  return (-1); \
}

/* _syscall_4
 *
 * 4 parameter advisory call
 */
#define _syscall_4(return_type,func_name,type1,param1,type2,param2,type3,param3,type4,param4) \
return_type func_name(type1 param1,type2 param2,type3 param3,type4 param4) \
{ \
 \
  long __rtvl; \
 \
  __asm__ volatile ("int $0x80" \
          : "=a" (__rtvl) \
          : "0"  (__CALL_##func_name), \
          "b" ((long)(param1)),"c" ((long)(param2)),"d" ((long)(param3)),"S" ((long)(param4))); \
 \
  if(__rtvl >= 0) \
    return (__rtvl); \
 \
  errno = - __rtvl; \
 \
  return (-1); \
}


/* _syscall_5
 *
 * 5 parameter advisory call
 */
#define _syscall_5(return_type,func_name,type1,param1,type2,param2,type3,param3,type4,param4,type5,param5) \
return_type func_name(type1 param1,type2 param2,type3 param3,type4 param4,type5 param5) \
{ \
 \
 long __rtvl; \
 \
 __asm__ volatile ("int $0x80" \
                : "=a" (__rtvl) \
                : "0"  (__CALL_##func_name), \
                "b" ((long)(param1)), "c" ((long)(param2)), "d" ((long)(param3)), "S" ((long)(param4)), "D" ((long)(param5))); \
 \
 if(__rtvl >= 0) \
   return (__rtvl); \
 \
 errno = - __rtvl; \
 \
 return (-1); \
}
 
/* _syscall_3m
 *
 * 3 parameter advisory call, with user specified CALL_funcname
 * this is used to support variadic system calls such as open,
 * where the actual system call is wrapped in a C routine that
 * handles the variadic parameters and calls an internal
 * system call '_open' to do the work.
 *
 */
#define _syscall_3m(call_name,return_type,func_name,type1,param1,type2,param2,type3,param3) \
return_type func_name(type1 param1,type2 param2,type3 param3) \
{ \
 \
  long __rtvl; \
 \
   __asm__ volatile ("int $0x80" \
           : "=a" (__rtvl) \
           : "0"  (__CALL_##call_name), \
           "b" ((long)(param1)),"c" ((long)(param2)), "d" ((long)(param3))); \
 \
  if (__rtvl >= 0) \
     return (__rtvl); \
 \
  errno = - __rtvl; \
 \
  return (-1); \
}
                
#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_SYSCALL_H */
