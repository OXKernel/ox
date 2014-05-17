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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Copyright (C) Roger G. Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module
;	track_read.inc
; @description
;	Access disk reading track at a time
; @author
;	Roger George Doss
;
%include "include/common.inc"
%include "include/msg.inc"

[BITS 16]

;
; track_read:-
;	boot_dr
;	cur_sect
;	cur_head
;	cur_track
;
;	bx is assummed to be setup correctly
;	prior to invocation.
;
;	Uses the variable retry_count to count
;	the number of read attempts that occur.
;	If we try twice and fail, we output
;	an error and halt the load, otherwise
;	we continue, reseting the registers and stack
;	when we are done.
;
%define BOOT_DR	  0xA
%define CUR_SECT  0x8
%define CUR_HEAD  0x6
%define CUR_TRACK 0x4
ENTRY track_read
	jmp real_track_read
		; allocate message
		_READ_ERR_MSG
		; allocate local variables
		retry_count dw 0

	real_track_read:
	push bp
	mov bp,sp	; set-up frame pointer

	.retry:
		cmp word [retry_count],2
		je .read_err

		push ax
		push bx
		push cx
		push dx

		mov dx,[bp + CUR_TRACK]
		mov cx,[bp + CUR_SECT]
		inc cx
		mov ch,dl
		mov dx,[bp + CUR_HEAD]
		mov dh,dl
		mov dl,[bp + BOOT_DR]
		and dx,0x0100
		mov ah,0x2
		int 0x13
		jc .bad_read
		jmp .return

	.bad_read:
		inc word [retry_count]
		mov ax,0
		mov dx,0
		int 0x13
		pop dx
		pop cx
		pop bx
		pop ax
		jmp .retry

	.return:
		; restore registers
		; restore stack
		; reset retry_count
		; return
		pop dx
		pop cx
		pop bx
		pop ax
		mov word [retry_count],0
			; let user know we are loading...
			pusha
			mov ax,'.'+0x0E00
			mov bx,7
			int 0x10
			popa
			
		leave
		ret

	.read_err:
.386:
		; output error message
		; and wait
		mov si,READ_ERR_MSG
		call bprint
		call handle_err

;
; EOF
;
