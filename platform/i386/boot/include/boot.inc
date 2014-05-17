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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger George Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module
;	boot.inc
; @description
;	NASM macros to facilitate writing boot loaders
; @author
;	Roger George Doss
;
%ifndef _BOOT_INC
%define _BOOT_INC

;
; enable_a20:-
;	Enables A20 address line.
;
%macro enable_a20 0
%%1:	in al,0x64
	test al,0x2
	jnz %%1
	mov al,0xD1
	out 0x64,al
%%2:	in al,0x64
	test al,0x2
	jnz %%2
	mov al,0xDF
	out 0x60,al
	%endmacro

;
; enable_pmode:-
;	Enables x86 Protected mode by loading the GDT,
;	setting the approperiate bit in CR0, then
;	loading the segment selectors with
;	a selector for flat memory.
;	Uses ecx register.
;
%macro enable_pmode 0
	lgdt [__GDT__]
	mov ecx,CR0
	inc cx
	mov CR0,ecx
	mov ax,DATA-GDT
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	%endmacro

;
; enable_rmode:-
;	Enables x86 Real mode by resetting
;	the approperiate bit in CR0.
;	Uses ecx register.
;
%macro enable_rmode 0
	mov ecx,CR0
	dec cx
	mov CR0,ecx
	%endmacro

%endif ; _BOOT_INC
