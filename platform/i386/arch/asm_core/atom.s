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
; @module: 
;
;	atom.s
;
; @description:	
;	
;	Atomic operations on bits.
;	Used to build primitive locking
;	mechanisms.
;	The C interface for these functions
;	is defined in ./platform/i386/include/asm_core/atom.h
;

; @author:
;
;	Roger G. Doss
;


%macro C_ENTRY 1
	global %1
	%1:
%endmacro

%ifdef _SMP
%define LOCK lock
%else
%define LOCK
%endif

extern debug_panic          ; panic.c

;
; atom_bit_set:-
; sets: eax, ecx, edx
;
; unsigned long 
; atom_bit_set(volatile unsigned long *data, unsigned long index );
C_ENTRY atom_bit_set
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; get the number of the bit
	mov edx,[esp + 8]
	; atomic test and set
	LOCK bts ecx,edx
	; get the results from the
	; carry flag
	mov edx,0
	adc edx,0

	; store results
	mov [eax],ecx
	; return the value of the previous bit
	mov eax,edx
	ret

;
; atom_bit_clear:-
; sets: eax, ecx, edx
;
; unsigned long
; atom_bit_clear(volatile unsigned long *data, unsigned long index );
C_ENTRY atom_bit_clear
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; get the number of the bit
	mov edx,[esp + 8]
	; atomic test and clear
	LOCK btr ecx,edx
	; get the results from the
	; carry flag
	mov edx,0
	adc edx,0

	; store results
	mov [eax],ecx
	; return the value of the previous bit
	mov eax,edx
	ret
;
; atom_increment:-
;
; void
; atom_increment( volatile unsigned long *data );
C_ENTRY atom_increment
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; increment
	LOCK inc ecx
	; store the results
	mov [eax],ecx
	ret

; atom_decrement:-
;
; void
; atom_decrement( volatile unsigned long *data );
C_ENTRY atom_decrement
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; decrement
	LOCK dec ecx
	; store the results
	mov [eax],ecx
	ret

; atom_inc_test:-
;
;unsigned long
;atom_inc_test ( volatile unsigned long *data );
C_ENTRY atom_inc_test
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; decrement
	LOCK inc ecx
	; set to 1 if ecx is now zero
	sete dl

	; store the results
	mov [eax],ecx
	; return results
	xor eax,eax
	mov al,dl
	ret

; atom_dec_test:-
;
;unsigned long
;atom_dec_test ( volatile unsigned long *data );
C_ENTRY atom_dec_test
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; decrement
	LOCK dec ecx
	; set to 1 if ecx is now zero
	sete dl

	; store the results
	mov [eax],ecx
	; return results
	xor eax,eax
	mov al,dl
	ret
;
; atom_set:-
;
; unsigned long
; atom_set      ( volatile unsigned long *data, unsigned long value );
C_ENTRY atom_set
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; get the number of the bit
	mov edx,[esp + 8]
	; atomic exchange
	LOCK xchg ecx,edx
	; save old value
	mov edx,[eax]
	; store results
	mov [eax],ecx
	; return the value of the previous
	mov eax,edx
	ret

; atom_add:-
;
;void
;atom_add      ( volatile unsigned long *data, unsigned long value );
C_ENTRY atom_add
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; get the number of the bit
	mov edx,[esp + 8]
	; atomic add
	LOCK add ecx,edx
	; store results
	mov [eax],ecx
	ret

; atom_dec:-
;
; void
; atom_dec      ( volatile unsigned long *data, unsigned long value );
C_ENTRY atom_dec
	; get the address of the data
	mov eax,[esp + 4]
	mov edx,[esp + 4]
	; dereference
	mov ecx,[edx]
	; get the number of the bit
	mov edx,[esp + 8]
	; atomic dec
	LOCK sub ecx,edx
	; store results
	mov [eax],ecx
	ret

;
; EOF
;
