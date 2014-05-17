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
 *  <platform/i386/sigcontext.h>
 *    platform specific defines for sigcontext
 *  
 ********************************************************/
#ifndef  _PLATFORM_I386_SIGCONTEXT_H
#define  _PLATFORM_I386_SIGCONTEXT_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* sigcontext is a structure
 * representing CPU registers,
 * used in the sigreturn() call
 * its definition is based on
 * the iBSC2 standard
 */
struct fpreg {
        unsigned short significand[4];
        unsigned short exponent;
};

struct fpstate {
        unsigned long   cw;
        unsigned long   sw;
        unsigned long   tag;
        unsigned long   ipoff;
        unsigned long   cssel;
        unsigned long   dataoff;
        unsigned long   datasel;
        struct fpreg    st[8];
        unsigned long   status;
};

struct sigcontext {
        unsigned short   sc_gs, sc_gsh;
        unsigned short   sc_fs, sc_fsh;
        unsigned short   sc_es, sc_esh;
        unsigned short   sc_ds, sc_dsh;
        unsigned long    sc_edi;
        unsigned long    sc_esi;
        unsigned long    sc_ebp;
        unsigned long    sc_esp;
        unsigned long    sc_ebx;
        unsigned long    sc_edx;
        unsigned long    sc_ecx;
        unsigned long    sc_eax;
        unsigned long    sc_trapno;
        unsigned long    sc_err;
        unsigned long    sc_eip;
        unsigned short   sc_cs, sc_csh;
        unsigned long    sc_eflags;
        unsigned long    sc_esp_at_signal;
        unsigned short   sc_ss, sc_ssh;
        struct fpstate   *sc_fpstate;
        unsigned long    sc_oldmask;
        unsigned long    sc_cr2;
};

struct sigframe {
  void (*sf_retadr)(void);
  int sf_signo;
  int sf_code;
  struct sigcontext *sf_scp;
  int sf_fp;
  void (*sf_retadr2)(void);
  struct sigcontext *sf_scpcopy;
};

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_SIGCONTEXT_H */ 
