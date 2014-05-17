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
; 	reboot.s
; @description
;	implementation of reboot
; @author
;	Roger G. Doss
;
%include "include/common.inc"
%include "include/msg.inc"

[BITS 16]

;
; reboot:
;	reboots the machine
;
ENTRY reboot
	jmp real_reboot
		; declare messages
		_REBOOT_MSG
real_reboot:
	mov si,REBOOT_MSG
	call bprint
	call getkey

	db 0xEA			; machine code for jmp FFFF:0000
	dw 0x0000
	dw 0xFFFF

;
; EOF
;
