;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger George Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 
;	@module 
;		util.s
;         
;	@description
;		Contains asembly routines to support kernel 
;         	operations, also a module for which one can
;         	add additional optimized code, written as pure
;         	assembly procedures, as opposed to C.
;		The C interface for these functions is
;		described in:
;		platform/i386/asm_core/io.h
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "common/macros.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  external routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
extern io_inb
extern io_outb

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  text section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section .text

; See http://wiki.osdev.org/Interrupts
; was int 0x1 - which may be reserved by Intel.
; was int 0x9 - which may be incorrect.
C_ENTRY raise_software_interrupt
    int 0x69
    ret

;
; asm_enable_interrupt:-
;       sets approperiate flag in EFLAGS, enabling interrupts.
;       void asm_enable_interrupt ( void )
;
C_ENTRY asm_enable_interrupt
        sti
        ret

;
; asm_disable_interrupt:-
;   sets approperiate flag in EFLAGS, disabling interrupts.
;   void asm_disable_interrupt ( void )
;
C_ENTRY asm_disable_interrupt
        cli
        ret

;
; asm_get_eflags:-
;   Returns the value of the eflags register
;   so the caller may save it.
;   unsigned long asm_get_eflags ( void )
;
C_ENTRY asm_get_eflags
	pushf
	pop dword eax
	ret
;
; asm_set_eflags:-
;   Sets the eflags register to the value passed
;   in as a parameter. NOTE: THIS MUST BE VALID!
;   void asm_set_eflags ( unsigned long flags )
;
C_ENTRY asm_set_eflags
    push dword [esp + 0x4] ; top of the stack is the return eip we want first param
	popfd
	ret

;
; asm_exit:-
;   Reboots the computer, if that fails, we
;   disable interrupts and halt the machine.
;
C_ENTRY asm_exit
	.loop:
		push dword 0x64
		call io_inb
		add  dword esp,4
		mov  byte  dl,al
		and  byte  dl,0x02
		test byte  dl,dl
		jne  .loop
		push dword 0xFE
		push dword 0x64
		call io_outb
		add  dword esp,8
	cli
	hlt	
	ret

C_ENTRY asm_hlt
	.loop jmp .loop
;
; EOF
;
