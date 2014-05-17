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
;	s2.s
; @description
;	x86 boot loader
;	(stage 2)
; @author
;	Roger George Doss
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Includes.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "include/boot.inc"
%include "include/debug.inc"
%include "include/exec.inc"
%include "include/gdt.inc"
%include "include/msg.inc"
%include "include/read_tracks.inc"
%include "include/clear_display.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Defines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define _K_LEN		0x9000	; 512kb == 0x90000 / 0x10
%define _K_BASE 	0x14000	; address where kernel loaded
%define _K_LOAD_SEG	0x1000	; segment where kernel loaded

%define _K_END_SEG	_K_LOAD_SEG + _K_LEN

%define _S1_BASE	0x7C00	; stage 1 base address

%define _S2_CURR_SECT	0x1	; chs == lba 0x21 == 33
%define _S2_CURR_HEAD	0x0
%define _S2_CURR_TRACK	0x0
%define _S2_NR_SECT	0x12	; 18 sectors per track

[BITS 16]
[ORG 0x600]
jmp start
nop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 			Data section.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_OX_MSG			; messages

boot_drive dw 0		; drive we were loaded from

			; nr_bytes dw 512
			; sectors dd 2880; (1.44 meg)
nr_sections dw	0	; number of ELF sections in kernel executable

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 			Start of Stage 2 loader.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
start:

; get boot disk information from stage 1 loader
xor ax,ax
mov ds,ax
mov ax,[_S1_BASE + 0x1FC]
mov [boot_drive],ax

; start the second stage loader proper
cli
enable_a20
enable_pmode
enable_rmode
sti

xor ax,ax
mov ds,ax

; we are ready to load in the kernel
mov si,OX_MSG
call bprint

; read kernel from disk
read_tracks _S2_CURR_SECT, _S2_CURR_HEAD, _S2_CURR_TRACK,_K_LOAD_SEG,_K_END_SEG,_S2_NR_SECT,boot_drive

call turn_off_floppy

; Determine if the file is ELF,
; if it is, exec the ELF file.
; Otherwise setup our kernel_start
; to be the agreed upon entry in start.s
; based on the address of _start.
; (see nm vmox.exe | grep _start)
mov edx, _K_BASE
is_elf edx

cmp eax,0
jne handleelf
; Handle flat binary.
mov dword [kernel_start], 0x33924
jmp loadkernel

handleelf:
; convert it to a flat binary
mov edx,_K_BASE
exec_elf_kernel edx,nr_sections,kernel_start

loadkernel:
; clear display for the kernel
clear_display

cli
enable_pmode
jmp dword (CODE-GDT):protected_mode
protected_mode:
[BITS 32]
	push dword 2
	popfd
	mov eax,[kernel_start]
	call eax
	call handle_err	; we probably crashed if we got here

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Utility functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "lib/turn_off_floppy.s"
%include "lib/memmove.s"
%include "lib/memzero.s"
%include "lib/bprint.s"
%include "lib/hprint.s"
%include "lib/track_read.s"
%include "lib/getkey.s"
%include "lib/reboot.s"
%include "lib/handle_err.s"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		GDT and kernel entry.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

__GDT__:
dw GDT_LENGTH
dd GDT
GDT:
NULL desc 0,0,0
CODE desc 0,0xFFFFF,D_CODE + D_READ  + D_BIG + D_BIG_LIM
DATA desc 0,0xFFFFF,D_DATA + D_WRITE + D_BIG + D_BIG_LIM
GDT_LENGTH equ $ - __GDT__ - 1

;
; start of kernel
;
kernel_start dd 0		; entry point for loaded kernel
times 16382-($-$$) db 0
dw 0xAA55			; signature at 0xFFE

;
; EOF
;
