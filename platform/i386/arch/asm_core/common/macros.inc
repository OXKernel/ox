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
;		common_macros.s
;
;	@description
;		Contains common and utility macros and defines.
;
;	@author Roger G. Doss
;

;
;	C_ENTRY:-
;		This macro provides a generic mechanism for
;	declaring an assembly routine that is accessible via
;	C code. NOTE: The ELF GCC Compiler does not append the
;	traditional '_' prefix to C symbols.
;
%macro C_ENTRY 1
	global %1
	%1:
	%endmacro

;
;	ENTRY:-
;		This macro provides a generic mechanism for
;	declaring an assembly routine that is accessible via
;	assembler.
;
%macro ENTRY 1
	global %1
	%1:
	%endmacro


;
;	C_PSTRUCT:-
;		This macro provides a generic mechanism to access
;	C pointer to structures in NASM.  It taks two arguments
;	a register destination, arg %1 and a c identifier representing
;	a C structure.
;
%macro C_PTR_STRUCT 2
	mov dword %1,[%2]
	mov dword %1,[%1]
	%endmacro

;
;	C_STRUCT:-
;		This macro provides a generic mechanism to access
;	a C  structures in NASM.  It taks two arguments
;	a register destination, arg %1 and a c identifier representing
;	a C structure.
;
%macro C_STRUCT 2
	mov dword %1,[%2]
	%endmacro

;
;	EOF
;
