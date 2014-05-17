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
;
; @module
;	cpu_test.s
; @description
;	implementation of cpu_test
; @author
;	Roger G. Doss
;
%include "include/common.inc"
%include "include/msg.inc"

[BITS 16]

; cpu_test:
;	Tests if we are running on a x386 or above machine
;
ENTRY cpu_test
	jmp real_cpu_test
		; declare messages
		_CPU_CHECK_MSG
		_CPU_OK_MSG
		_CPU_8688_MSG
		_CPU_286_MSG
		_CPU_ERROR_MSG
real_cpu_test:
	mov  si,CPU_CHECK_MSG
	call bprint

	pushf			; save FLAGS

	xor ah,ah		; zero ah
	push ax	
	popf			; copy ax into flags, with bits 12-15 clear

	pushf
	pop ax			; copy flags into ax

	and ah,0xF0		; test cpu by checking if bits 12-15 are set
	cmp ah,0xF0
	je cpu_error_8688	; detected 8086,8088

	mov ah,0xF0		; set bits 12-15
	push ax
	popf			; copy ax into flags, with bits 12-15 set

	pushf
	pop ax			; copy flags into ax

	and ah,0xF0		; test cpy by checking if 12-15 are clear
	jz cpu_error_286	; detected x286

	popf			; restore original flags

	mov si,CPU_OK_MSG
	call bprint
	ret

cpu_error_8688:			; handle cpu errors
	mov  si,CPU_8688_MSG
	call bprint
	mov si,CPU_ERROR_MSG
	call bprint
	call handle_err

cpu_error_286:
	mov si,CPU_286_MSG
	call bprint
	mov si,CPU_ERROR_MSG
	call bprint
	call handle_err

;
; EOF
;
