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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger G. Doss. All Rights Reserved. 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module: 
;	debug_exec.inc
; @description: 
;	Debug macros for exec_elf_kernel
; 	these are specific to the assembly
;	implementation of exec_elf_kernel.
; @author: 
;	Roger G. Doss 
;
%ifndef _DEBUG_EXEC_INC
%define _DEBUG_EXEC_INC

;
; print_ph_num:-
;
; For use in printing
; out the value of ph_num
; macro parameter %2.
;
%macro print_ph_num 0
	push eax
	xor eax,eax
	mov ax,[%2]
	push eax
	call hprint
	add sp,4
	pop eax
	%endmacro

;
; print_param:-
; 
; Output parameter information
; needed to convert an elf file
; into a 32bit flat Intel 80x86
; binary.
;
%macro print_param 0
	jmp %%real_print_param
		p1 db 'p_hoff: ',13,10,0
		p2 db 'p_hentsize: ',13,10,0
		p3 db 'base address: ',13,10,0
	%%real_print_param:
	mov si,p1
	call bprint
	print_reg ebx
	mov si,p2
	call bprint
	print_reg eax
	mov si,p3
	call bprint
	print_reg edx
	%endmacro

;
; check_ptload:-
;
; For use after the comparison of
; the sections program header, 
; Elf32_Phdr.p_type. The result
; should be 1, we make a comparison
; in the code to see if this is true,
; if it is not, then the message
; pt_load will be outputted.
;
; NOTE: This is a great test to
;	see if the exec is working
;	properly.  This value must
;	be one. If not, the kernel
;	image is either not an executable
;	or its currupt.  More importantly,
;	we may have a memory access problem
;	in the loader.
;
%macro check_ptload 0
	je go
		jmp nn
			msg db 'pt_load',13,10,0
		nn:
		mov si,msg
		call bprint
		stop
	go:
	%endmacro

%macro print_ptload 0
	xor eax,eax
	mov ax,[ebx]
	push dword eax
	call hprint
	add sp,4
	%endmacro

%endif ; _DEBUG_EXEC_INC
