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
;
; @module
;	s1.s
;
; @description
;	x86 boot loader
;	(stage 1)
; @author
;	Roger G. Doss
;
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Includes.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "include/common.inc"
%include "include/msg.inc"
%include "include/exec.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Defines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define _S2_LEN		0x21	; (32 + 1) * 512 == 0x4000
%define _S2_BASE	0x600	; stage 2 base address
%define _S2_LOAD_SEG 	0x60	; segment to place s2 loader
%define _S2_SIGN_OFF	0x3FFE	; signature offset

%define _B_STACK_SEG	0x0
%define _B_STACK	0x400

[BITS 16]	; x86  boots in 16bit real-mode
[ORG 0x07C00 ]	; offset data is zero

jmp start	; jmp over data
nop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                       Data section.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_ERROR_MSG		; messages
_COPYING_MSG
_BOOT_MSG
_BOOT_ERR_MSG

load_len   dw _S2_LEN 	; length of stage 2 loader in sectors
nr_sectors dw 18
nr_heads   dw 2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                       Start of Stage 1 loader.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	1. BIOS has checked our magic number.
;	2. BIOS has read  us from sector 1,cylinder 0,head 0
;	   into 0x7C00 memory address.
;	3. The state of the machine is as follows:
;
;	   DL = Boot Drive, 0x0  primary floppy
;			  , 0x80 primary hard disk
;	   CS = 0
;	   IP = 0x7C00
;	4. Store the boot drive.
;	5. Load stage 2 loader.
;	6. Jump to stage 2 loader.
;
start:
	xor ax,ax
	mov ds,ax
	mov [boot_drive],dl		; record boot drive

	cli				; disable interrupts
		mov ax,_B_STACK_SEG	; set up stack
		mov ss,ax
		mov sp,_B_STACK
	sti				; enable interrupts

	mov dl,[boot_drive]
	mov si,COPYING_MSG		; display copyright mesg
	call bprint

	; set-up to load stage 2 loader from the disk
	; NOTE: we start at 1, so this
	; 	will actually load load_len - 1 sectors
	mov bx,_S2_LOAD_SEG
	mov es,bx
	mov ax,1
	mov cx,[load_len]
	mov di,1

	load_s2:
		call sector_read
		inc ax
		mov bx,es
		add bx,32
		mov es,bx

		cmp  ax,cx
		jne  load_s2

	call turn_off_floppy

	mov ax,[_S2_BASE + _S2_SIGN_OFF]
	cmp ax,0xAA55
	jne .load_err

	mov si,BOOT_MSG		; display loader msg
	call bprint
	exec_bin_kernel _S2_BASE

.load_err:
.386:
	mov si,BOOT_ERR_MSG
	call bprint
	call handle_err

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Utility functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "lib/bprint.s"
%include "lib/getkey.s"
%include "lib/reboot.s"
%include "lib/sector_read.s"
%include "lib/turn_off_floppy.s"
%include "lib/handle_err.s"

times 508-($-$$) db 0	; clear the rest, up to 508 bytes
boot_drive dw 0		; shared with stage 2 loader
dw   0xAA55		; set up boot sector signature, 2 bytes
