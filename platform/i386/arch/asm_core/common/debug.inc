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
;	@module:
;		debug.inc
;
;	@description:
;		 Macros for debugging NASM instructions.
;
;	@author:
;		Roger G. Doss.
;

extern printk
extern exit
section .data

format  db '%d',10,0
fformat db '%s',10,0

%macro PRINT_LINE 0
	push dword __LINE__
	push dword format
	call printk
	add dword esp,8
	%endmacro

%macro PRINT_FILE 0
	push dword __FILE__
	push dword fformat
	call printk
	add  dword esp,8
	%endmacro

%macro PRINT_D 1
	push dword %1
	push dword format
	call printk
	add  dword esp,8
	%endmacro

;
; EOF
;
