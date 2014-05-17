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
;	exec.inc
; @description
;	NASM macro to exec an ELF binary
;	and flat binary given a memory address
;	pointing to the in-memory location of the
;	entire file
;
; @author
;	Roger George Doss
;
%ifndef _EXEC_INC
%define _EXEC_INC

;
; exec_elf_kernel:-
;
;	Attempts to prepare a 32bit ELF kernel/image
; for protected-mode execution.
;

; special ELF numeric values
%define ELF_SIGNATURE 0x464C457F ; 'F','L','E',0x7F
%define ELFCLASS32   0x1	  ; 32bit ELF
%define ELFDATA2LSB  0x1	  ; LSB Intel Little Endian
%define EV_CURRENT   0x1	  ; current version
%define PT_LOAD	     0x1	  ; type of entry if it is loadable

; special ELF offsets
%define EI_CLASS     0x4	  ; file class
%define EI_DATA	     0x5	  ; data encoding
%define EI_VERSION   0x6	  ; file version
%define e_entry	     0x18	  ; program entry
%define e_phentsize  0x2A	  ; sizeof entry in program header
%define e_phnum      0x2C	  ; number of entries
%define e_phoff	     0x1C	  ; program header table offset
%define p_offset     0x4	  ; offset from beg of the file where segment start
%define p_filesz     0x10	  ; sizeof segment (in file   image)
%define p_paddr	     0x8	  ; memory address of segment
%define p_memsz	     0x14	  ; sizeof section (im memory image)

;
; Expects an ELF file STATICALLY linked.
;
;	%1 -> kernel base address in memory
;	%2 -> nr_sections number of ELF sections in kernel executable
;	%3 -> kernel start address
;
; This routine does not run the kernel/image, but is an
; intermediary, that converts the ELF file into a flat binary
; executable.  The actual execution is done in
; the code using this macro. A kernel built as an Intel/ELF image must be
; a 32bit executable, therefore it must run in protected mode,
; hence we need to allow for flexibility in switching to protected
; mode.  There is no such thing as a 16bit ELF Format.
;
%macro	exec_elf_kernel 3
	jmp %%check_elf

		_EXEC_ELF_MSG
		_EXEC_ELF_MSG0
		_EXEC_ELF_MSG1
		_EXEC_ELF_MSG2
		_EXEC_ELF_MSG3
		_EXEC_ELF_MSG4
		_EXEC_ELF_MSGN

		%%sig_error:
			mov si,EXEC_ELF_MSG
			call bprint
			mov si,EXEC_ELF_MSG0
			call bprint
			jmp %%error
		%%class_error:
			mov si,EXEC_ELF_MSG
			call bprint
			mov si,EXEC_ELF_MSG1
			call bprint
			jmp %%error
		%%byte_error:
			mov si,EXEC_ELF_MSG
			call bprint
			mov si,EXEC_ELF_MSG2
			call bprint
			jmp %%error
		%%ver_error:
			mov si,EXEC_ELF_MSG
			call bprint
			mov si,EXEC_ELF_MSG3
			call bprint
			jmp %%error
		%%exec_error:
			mov si,EXEC_ELF_MSG
			call bprint
			mov si,EXEC_ELF_MSG4
			call bprint
			jmp %%error
		%%error:
			mov si,EXEC_ELF_MSGN
			call bprint
			call handle_err

	%%check_elf:

	; verify that we have a valid ELF file
	cmp dword [%1],ELF_SIGNATURE	; if(Elf32_Ehdr.e_ident[0-3] == signature)
	jne %%sig_error
	cmp byte  [%1 + EI_CLASS],ELFCLASS32
	jne %%class_error
	cmp byte  [%1 + EI_DATA],ELFDATA2LSB
	jne %%byte_error
	cmp byte  [%1 + EI_VERSION],EV_CURRENT
	jne %%ver_error

	; obtain the program entry point
	mov eax,[%1 + e_entry]
	mov [%3],eax

	; obtain the number of sections
	xor ecx,ecx
	mov cx,[%1 + e_phnum]
	mov [%2],cx
	; this can't be zero to start with
	cmp word [%2],0	
	je  %%exec_error

	; e_phnum == 0 => object file not executable
	cmp word [%2],0
	je  %%exec_error

	%%while:
		cmp word [%2],0
		je  %%done
		dec word [%2]

		push edx
		xor eax,eax
		mov ax,[%1 + e_phentsize]	; eax <= e_phnum * e_phentsize
		xor edx,edx
		mov dx,[%2]
		mul dx
		pop edx

		xor ebx,ebx
		mov ebx,[%1 + e_phoff]
		add ebx,eax		; add pht location to pht entry offset
		add ebx,%1		; add to where the kernel is in memory

		cmp word [ebx],PT_LOAD	; if(Elf32_Phdr.p_type == PT_LOAD)
		jne %%while

		mov dword esi,[ebx + p_offset]
		mov dword ecx,[ebx + p_filesz]

		mov dword edi,[ebx + p_paddr]
		mov dword ebx,[ebx + p_memsz]

		; es:esi  source         (offset from location in memory)
		; ecx     sizeof segment (in file image)
		; es:edi  destination    (physical address)
		; ebx     sizeof section (in memory image)
		push ecx
		pusha
		add esi,%1
		call memmove
		popa
		pop eax

		sub ebx,eax	; if there is a difference between memory image
		jz  %%while	; and disk image size, zero out the difference

		add edi,eax	; es:edi has physical address
		xor eax,eax
		mov ecx,ebx
		call memzero

		jmp %%while
	%%done:

	%endmacro

;
; exec_bin_kernel:-
;
;	Attempts to execute a flat-binary kernel/image
;	assumes that we have a valid address for an instruction
;	pointer.
;
;	%1 -> IP for flat-binary kernel/image
;
%macro exec_bin_kernel 1
	jmp %1
	%endmacro

;
; is_elf
;
; Determines if the file is an ELF file, if it is
; it will return 1 otherwise 0.
;
; %1 kernel base address in memory
;
%macro is_elf 1
	; verify that we have a valid ELF file
	cmp dword [%1],ELF_SIGNATURE	; if(Elf32_Ehdr.e_ident[0-3] == signature)
	je %%no_error
        ; Not an ELF file.
        mov eax,0
        jmp %%done
    %%no_error:
        ; It is an ELF file.
        mov eax,1
    %%done:
    %endmacro

%endif ; _EXEC_INC
