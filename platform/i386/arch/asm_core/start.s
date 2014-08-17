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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger G. Doss. All Rights Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	@module
;		start.s
;
;	@description
;
;		The first entry point of the ox kernel
;	its task is to kick off execution, and start initializing
;	the system. The very first thing we do is initialize
;	the kernel's GDT and IDT, and load them into the 
;	approperiate registers.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "common/macros.inc"
%include "common/cdef.inc"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external symbols
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
extern __GDT__				; protected_mode.c
extern __IDT__				; protected_mode.c
extern loadgdt              ; gdt_init.c - Used for debugging.
extern loadidt              ; gdt_init.c - Used for debugging.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; external routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
extern protect_init			; protected_mode.c
extern interrupt_init		; interrupt.c
extern main				    ; main.c
extern printk				; printk.c
extern asm_hlt				; util.s
extern debug_panic          ; panic.c

section data

; RGDDDEBUG
local_idt:
    resd 50*2

; RGDDEBUG
idtr:
    dw (50*8)-1
    dd local_idt

section text

; _start:-
;	ox kernel assembly entry point, called
;	from the boot loader
C_ENTRY _start

    ; The below two statements are for debugging.
    ; call debug_panic      ; RGDDEBUG
	; call asm_hlt			; stop  execution

	call protect_init		; set up GDT/IDT

	push dword 2
	popf				; clear EFLAGS
	lgdt	[__GDT__]		; load the GDT
	mov word ax,KERNEL_DS   ; set up for kernel data segment
	mov word ds,ax
	mov word es,ax
	mov word ss,ax
	mov word ax,KERNEL_DS; RGDDEBUG was USER_DS   ; set up for user data segment
	mov word fs,ax
	mov word gs,ax
	mov dword esp,0xFFFF		; stack at top of segment
	jmp continue			; clear pre-fetch queue
	nop
	nop
	continue:

	call interrupt_init		; set i8259 controller
	lidt	[__IDT__]		; load the IDT

	pushf
	and dword [esp],0xFFFFBFFF	; clear NT bit
	popf				; reset flags

	call main			; call  main
	call asm_hlt			; stop  execution
	ret
;
; EOF
;
