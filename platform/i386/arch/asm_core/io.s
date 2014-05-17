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
; Copyright (C) Roger George Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 
;	@module 
;		io.s
;
;	@description
;		Contains io routines to be exported to
;	the rest of the kernel. The interace to these
;	routines much match that of those defined in
;	platform/asm_core/io.h, to provide approperiate
;	C language usuage.  The io_in[out] routines
;	explicitly omit the frame pointer, and
;	any unncessary code for clearing registers prior
;	to a mov.  edx is saved on every call, but not
;	eax, whose value is set approperiately for
;	returns.  We access the stack directly, and
;	handle the argument sizes approperiately.
;	With io slow down macros, we save eax,
;	to assert that its value will be the correct
;	return value on the io_in routines.
;	The string io routines use the frame pointer.
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; include files
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "common/macros.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; local defines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%define PARAM1 		  0x8
%define PARAM2 		  0xC
%define PARAM3 		  0x10 ; This was 0xF but 0xC + 0x4 is 0x10
                           ; 16 not 15.
%define NON_EXISTANT_PORT 0x80
%define FAKE_DATA 	  0x0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; local macros
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro  IO_SLOW_DOWN_JMP 0
	jmp %%forword
	%%forword
	%endmacro

%macro  IO_SLOW_DOWN_OUT 0
	push dword eax
	mov  byte al,FAKE_DATA
	out NON_EXISTANT_PORT,al
	pop  dword eax
	%endmacro

%macro  IO_SLOW_SLOW_DOWN_OUT 0
	push dword eax
	mov  byte al,FAKE_DATA
	out NON_EXISTANT_PORT,al
	out NON_EXISTANT_PORT,al
	out NON_EXISTANT_PORT,al
	out NON_EXISTANT_PORT,al
	pop dword eax
	%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; text section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
section .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; byte, word, long word io operations
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; io_inb:
;   reads one byte from a specified port.
;   unsigned char io_inb (unsigned int port)
;
C_ENTRY io_inb
	push dword edx
	xor  byte al,al
	mov  dword edx,[esp + PARAM1]  ; port
	in   al,dx
	pop  dword edx
	ret

;
; io_outb:
;   writes a byte to a specified port.
;   void io_outb (unsigned int port, unsigned char byte)
;
C_ENTRY io_outb
	push dword  edx
	mov  byte   al, [esp + PARAM2] ; byte
	mov  dword  edx,[esp + PARAM1] ; port
	out  dx,al
	pop  dword edx
	ret

;
; io_inw:
;   read a word from a specified port.
;   unsigned short io_inw ( unsigned int port )
;
C_ENTRY io_inw
	push dword edx
	xor  word  ax,ax
	mov  dword edx,[esp + PARAM1]  ; port
	in   ax,dx
	pop  dword edx
	ret	

;
; io_outw:
;   writes a word to a specified port.
;   void io_outw ( unsigned int port, unsigned short word )
;
C_ENTRY io_outw
	push dword edx
	mov   word ax, [esp + PARAM2]  ; word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,ax
	pop  dword edx
	ret

;
; io_inl:
;   read a long word from a specified port.
;   unsigned long io_inl ( unsigned int port )
;
C_ENTRY io_inl
	push dword edx
	xor  dword eax,eax
	mov  dword edx,[esp + PARAM1]  ; port
	in   eax,dx
	pop  dword edx
	ret

;
; io_outl:
;  writes a long word to a specified port.
;  void io_outl ( unsigned int port, unsigned long long_word )
;
C_ENTRY io_outl
	push dword edx
	mov  dword eax,[esp + PARAM2]  ; long_word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,eax
	pop  dword edx
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; io pausing routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; io_inb_p:
;   reads one byte from a specified port followed by a pause.
;   unsigned char io_inb_p (unsigned int port)
;
C_ENTRY io_inb_p
	push dword edx
	xor  byte al,al
	mov  dword edx,[esp + PARAM1]  ; port
	in   al,dx
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_outb_p:
;   writes a byte to a specified port followed by a pause.
;   void io_outb_p ( unsigned int port, unsigned char byte )
;
C_ENTRY io_outb_p
	push dword edx
	mov  byte   al,[esp + PARAM2]  ; byte
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,al
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_inw_p:
;   read a word from a specified port followed by a pause.
;   unsigned short io_inw_p ( unsigned int port )
;
C_ENTRY io_inw_p
	push dword edx
	xor  word ax,ax
	mov  dword edx,[esp + PARAM1]  ; port
	in   ax,dx
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_outw_p:
;   writes a word to a specified port  followed by a pause.
;   void io_outw_p ( unsigned int port, unsigned short word )
;
C_ENTRY io_outw_p
	push dword edx
	mov   word ax, [esp + PARAM2]  ; word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,ax
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_inl_p:
;   read a long word from a specified port followed by a pause.
;   unsigned long io_inl_p ( unsigned int port )
;
C_ENTRY io_inl_p
	push dword edx
	xor  dword eax,eax
	mov  dword edx,[esp + PARAM1]  ; port
	in   eax,dx
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_outl_p:
;  writes a long word to a specified port followed by a pause.
;  void io_outl_p ( unsigned int port, unsigned long long_word )
;
C_ENTRY io_outl_p
	push dword edx
	mov  dword eax,[esp + PARAM2]  ; long_word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,eax
	IO_SLOW_DOWN_JMP
	pop  dword edx
	ret

;
; io_inb_p_:
;   reads one byte from a specified port followed by a pause.
;   unsigned long io_inb_p_ (unsigned int port)
;
C_ENTRY io_inb_p_
	push dword edx
	xor  byte al,al
	mov  dword edx,[esp + PARAM1]  ; port
	in   al,dx
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outb_p_:
;   writes a byte to a specified port followed by a pause.
;   void io_outb_p_ ( unsigned int port, unsigned char byte )
;
C_ENTRY io_outb_p_
	push dword edx
	mov  byte   al,[esp + PARAM2]  ; byte
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,al
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_inw_p_:
;   read a word from a specified port followed by a pause.
;   unsigned short io_inw_p_ ( unsigned int port )
;
C_ENTRY io_inw_p_
	push dword edx
	xor  word ax,ax
	mov  dword edx,[esp + PARAM1]  ; port
	in   ax,dx
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outw_p_:
;   writes a word to a specified port  followed by a pause.
;   void io_outw_p_ ( unsigned int port, unsigned short word )
;
C_ENTRY io_outw_p_
	push dword edx
	mov   word ax, [esp + PARAM2]  ; word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,ax
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_inl_p_:
;   read a long word from a specified port followed by a pause.
;   unsigned long io_inl_p_ ( unsigned int port )
;
C_ENTRY io_inl_p_
	push dword edx
	xor  dword eax,eax
	mov  dword  edx,[esp + PARAM1]  ; port
	in   eax,dx
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outl_p_:
;  writes a long word to a specified port followed by a pause.
;  void io_outl_p_ ( unsigned int port, unsigned long long_word )
;
C_ENTRY io_outl_p_
	push dword edx
	mov  dword eax,[esp + PARAM2]  ; long_word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,eax
	IO_SLOW_DOWN_OUT
	pop  dword edx
	ret


;
; io_inb_p__:
;   reads one byte from a specified port followed by a pause.
;   unsigned char io_inb_p__ (unsigned int port)
;
C_ENTRY io_inb_p__
	push dword edx
	xor  byte al,al
	mov  dword  edx,[esp + PARAM1]  ; port
	in   al,dx
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outb_p__:
;   writes a byte to a specified port followed by a pause.
;   void io_outb_p__ ( unsigned int port, unsigned char byte )
;
C_ENTRY io_outb_p__
	push dword edx
	mov  byte   al,[esp + PARAM2]  ; byte
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,al
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_inw_p__:
;   read a word from a specified port followed by a pause.
;   unsigned short io_inw_p__ ( unsigned int port )
;
C_ENTRY io_inw_p__
	push dword edx
	xor  word ax,ax
	mov  dword edx,[esp + PARAM1]  ; port
	in   ax,dx
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outw_p__:
;   writes a word to a specified port  followed by a pause.
;   void io_outw_p__ ( unsigned int port, unsigned short word )
;
C_ENTRY io_outw_p__
	push dword edx
	mov   word ax, [esp + PARAM2]  ; word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,ax
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_inl_p__:
;   read a long word from a specified port followed by a pause.
;   unsigned long io_inl_p__ ( unsigned int port )
;
C_ENTRY io_inl_p__
	push dword edx
	xor  dword eax,eax
	mov  dword edx,[esp + PARAM1]  ; port
	in   eax,dx
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;
; io_outl_p__:
;  writes a long word to a specified port followed by a pause.
;  void io_outl_p__ ( unsigned int port, unsigned long long_word )
;
C_ENTRY io_outl_p__
	push dword edx
	mov  dword eax,[esp + PARAM2]  ; long_word
	mov  dword edx,[esp + PARAM1]  ; port
	out  dx,eax
	IO_SLOW_SLOW_DOWN_OUT
	pop  dword edx
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; string io routines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; io_insb:
;   void io_insb ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_insb
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword edi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword edi,[ebp + PARAM2]   ; destination address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  insb
        pop  dword edi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; io_outsb:
;   void io_outsb ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_outsb
	push dword ebp
	mov  dword ebp,esp
	push dword ecx
	push dword edx
	push dword esi
	mov  dword ecx,[ebp + PARAM3]	; count
	mov  dword esi,[ebp + PARAM2]	; source address
	mov  dword edx,[ebp + PARAM1]	; port
	cld				; increment address in esi
	rep  outsb
	pop  dword esi
	pop  dword edx
	pop  dword ecx
	leave
	ret

;
; io_insw:
;   void io_insw ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_insw
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword edi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword edi,[ebp + PARAM2]   ; destination address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  insw
        pop  dword edi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; io_outsw:
;   void io_outsw ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_outsw
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword esi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword esi,[ebp + PARAM2]   ; source address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  outsw
        pop  dword esi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; io_insl:
;   void io_insl  ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_insl
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword edi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword edi,[ebp + PARAM2]   ; destination address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in edi
        rep  insd
        pop  dword edi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; io_outsl:
;   void io_outsl ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_outsl
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword esi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword esi,[ebp + PARAM2]   ; source address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  outsd
        pop  dword esi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	io memory operations
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; io_memset:
;   sets io address with count value, starting from address.
;   void io_memset ( unsigned port, unsigned value, unsigned long count )
;
C_ENTRY io_memset
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword esi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword esi,[ebp + PARAM2]   ; value
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  outsd
        pop  dword esi
        pop  dword edx
        pop  dword ecx
        leave
	ret


;
; io_memget:
;   copies from io to memory.
;   void io_memget ( unsigned port, void *address, unsigned long count ) 
;
C_ENTRY io_memget
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword edi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword edi,[ebp + PARAM2]   ; destination address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  insd
        pop  dword edi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; io_memput:
;   copies from memory to io.
;   void io_memput ( unsigned port, void *address, unsigned long count )
;
C_ENTRY io_memput
	push dword ebp
        mov  dword ebp,esp
        push dword ecx
        push dword edx
        push dword esi
        mov  dword ecx,[ebp + PARAM3]   ; count
        mov  dword esi,[ebp + PARAM2]   ; source address
        mov  dword edx,[ebp + PARAM1]   ; port
        cld                             ; increment address in esi
        rep  outsd
        pop  dword esi
        pop  dword edx
        pop  dword ecx
        leave
	ret

;
; EOF
;
