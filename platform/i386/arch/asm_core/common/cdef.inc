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
; Copyright (C) Roger George Doss. All Rights Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;       @module
;              cdef.s
;
;       @description
;               Allows us to interface with C defines
;		essentially by redefining them for NASM.
;
;       @author
;               Roger George Doss
;

%define PROC_TRACE_SYSCALL 1	; ox/process_flags.h
%define __ENOSYS__  38          ; sys/errno.h
%define Nr_SYS_CALL 73          ; ox/defs.h

;
;       Special segments these must match
;       platform/segment_selectors.h
;       and the corresponding entries in the GDT
;       
%define KERNEL_CS               0x10
%define KERNEL_DS               0x18
%define USER_CS                 0x23
%define USER_DS                 0x2B

;
; EOF
;
